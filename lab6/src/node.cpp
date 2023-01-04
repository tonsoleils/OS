#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "zmq_f.h"

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) { // если нам не передали аргументы - ошибка
        std::cout << "Wrong arguments. Not enough parameters!\n";
        exit(1);
    }
    int current_id = std::atoi(argv[1]); // считываем первый аргумент - айди ноды
    int child_id = -1; // айди ноды-потомка
    if (argc == 3) { // если нам передали два аргумента, значит передали айди потомка
        child_id = std::atoi(argv[2]); // считываем его
    }
    std::string adr = argv[1];

    zmq::context_t context; // контекст
    zmq::socket_t parent_socket(context, ZMQ_REP); // создаем сокет
    connect(parent_socket, current_id); // коннектимся к сокету

    zmq::socket_t child_socket(context, ZMQ_REQ); // создаем сокет потомка
    child_socket.setsockopt(ZMQ_SNDTIMEO, 5000); // Maximum time before a send operation returns with EAGAIN
    parent_socket.setsockopt(ZMQ_SNDTIMEO, 5000); // Maximum time before a send operation returns with EAGAIN

    std::string message;

    std::map<std::string, int> storage; // локальный словарь ноды

    while (1) {
        zmq::message_t message_main;
        message = receive_message(parent_socket); // получаем сообщение
        std::string recieved_message(static_cast<char*>(message_main.data()), message_main.size());
        std::istringstream request(message); // отправляем его в строковый поток
        int dest_id;

        request >> dest_id; // считываем айди ноды

        std::string command;
        request >> command; // считываем комманду

        if (command == "heartbit") {
            std::string ans = std::to_string(current_id) + ":Ok; "; // подготавливаем сообщение - ответ
            if (child_id != -1) {
                int timeout;
                request >> timeout; // считываем таймаут
                int fl = 0;
                for (int i = 0; i < 4; i++) { // 4 раза пингуем ноду
                    send_message(child_socket, message);
                    std::string repl = receive_message(child_socket);
                    if (repl != "Error") { // если не получили ошибку
                        ans += repl;
                        fl = 1;
                        break; // прекращаем
                    }
                    sleep(timeout / 1000); // ждём timeout / 1000 секунд
                }
                if (fl == 0) {
                    ans += "Node " + std::to_string(child_id) + " is not avail"; // если нода возвращает Error
                }
            }
            send_message(parent_socket, ans); // отправляем ответ
        } else if (dest_id == current_id) {
            if (command == "pid") { // если команда - pid
                send_message(parent_socket, "OK: " + std::to_string(getpid()));
            } else if (command == "create") { // если команда - create
                int new_child_id;
                request >> new_child_id; // считываем id новой ноды
                if (child_id != -1) {
                    unbind(child_socket, child_id); // анбиндим сокет
                }
                bind(child_socket, new_child_id); // биндим сокет на новый id
                pid_t pid = fork(); // создаем новый процесс
                if (pid < 0) { // если ошибка
                    perror("Can't create new process!\n");
                    exit(1);
                }
                if (pid == 0) {
                    // запускаем новую ноду
                    execl("node", "node", std::to_string(new_child_id).c_str(), std::to_string(child_id).c_str(), NULL);
                    perror("Can't create new process!\n");
                    exit(1);
                }
                send_message(child_socket, std::to_string(new_child_id) + "pid"); // отправляем сообщение с новым id
                child_id = new_child_id;
                send_message(parent_socket, receive_message(child_socket)); // отправляем сообщение родителю
            } else if (command == "remove") { // если команда - удалить ноду
                send_message(parent_socket, "OK"); // отправляем сообщение
                disconnect(parent_socket, current_id); // и дисконнектимся от сокета
                break;
            } else if (command == "exec") { // если команда - exec
                std::string msg = "OK:" + std::to_string(dest_id); // подготавливаем сообщение
//                request >> mapKey;
//                request >> mapValue;
                size_t count = 0; // количество параметров (1 или 2)
                std::string mapKey; // ключ
                int mapValue; // значение

                request >> count; // считываем количество параметров
                request >> mapKey; // считываем ключ (он по-любому придёт)
                if (count == 2) { // если пришло 2 параметра
                    request >> mapValue; // считываем значение
                }

                if (count == 1) { // если пришёл только ключ
                    if (storage.count(mapKey) > 0) { // если ключ присутствует в словаре
                        msg += ": " + std::to_string(storage.at(mapKey)); // добавляем в сообщение значение
                    } else {
                        msg += ": '" + mapKey + "' not found"; // иначе возвращаем ошибку
                    }
                } else if (count == 2) { // если пришло 2 параметра
                    storage[mapKey] = mapValue; // добавляем в словарь по ключу значение
                }
                send_message(parent_socket, msg); // отправляем сообщение
            }
        } else if (child_id != -1) {
            send_message(child_socket, message);
            send_message(parent_socket, receive_message(child_socket));
            if (child_id == dest_id && command == "remove") {
                child_id = -1;
            }
        } else {
            send_message(parent_socket, "Error: node is unavailable!\n"); // ошибка, что нода не доступна
        }
    }
}
