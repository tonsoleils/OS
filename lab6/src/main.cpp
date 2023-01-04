#include <unistd.h>
#include <iostream>
#include <vector>
#include <zmq.hpp>
#include <sstream>

#include "topology.h"
#include "zmq_f.h"

int main() {
    Topology network; // наша топология
    std::vector<zmq::socket_t> branches; // вектор сокетов
    zmq::context_t context; // контекст. Класс context_t инкапсулирует функции, связанные с инициализацией и завершением контекста
    std::string command; // переменная для комманды
    zmq::socket_t main_socket(context, ZMQ_REP); // главный сокет
    std::string message;
    std::cout << "> ";
    while (std::cin >> command) { // пока мы получаем на вход комманды
        std::cout << "> ";
        if (command == "create") { // если команда - создать ноду
            int node_id, parent_id;
            std::cin >> node_id >> parent_id; // считываем айди ноды и айди ноды-родителя

            if (network.Find(node_id) != -1) {  // Поиск id ноды среди существующих
                std::cout << "Error: already exists!\n"; // если существует - ошибка
            } else if (parent_id == -1) { // если родитель - управляющий узел
                pid_t pid = fork();  // Создание дочернего узла
                if (pid < 0) {
                    perror("Can't create new process!\n");
                    exit(EXIT_FAILURE);
                } if (pid == 0) {
                    execl("node", "node", std::to_string(node_id).c_str(), NULL); // запускаем ноду
                    perror("Can't execute new process!\n");
                    exit(EXIT_FAILURE);
                }
                branches.emplace_back(context, ZMQ_REQ); // добавляем в вектор
                branches[branches.size() - 1].setsockopt(ZMQ_SNDTIMEO, 5000); // добавляем опцию таймаута
                bind(branches[branches.size() - 1], node_id); // биндим сокет
                send_message(branches[branches.size() - 1], std::to_string(node_id) + "pid"); // отправляем сообщение

                std::string reply = receive_message(branches[branches.size() - 1]); // и получаем ответ - pid
                std::cout << reply << "\n"; // выводим его
                network.Insert(node_id, parent_id); // и вставляем в топологию новую ноду
            } else if (network.Find(parent_id) == -1) { // если не нашли родителя
                std::cout << "Error: parent not found!\n";
            } else { // если родитель - не вычислительынй узел
                int branch = network.Find(parent_id); // ищем ноду
                // и отправляем сообщение о создании ноды
                send_message(branches[branch], std::to_string(parent_id) + "create " + std::to_string(node_id));

                std::string reply = receive_message(branches[branch]); // получаем в ответ pid
                std::cout << reply << "\n"; // выводим

                network.Insert(node_id, parent_id); // вставляем в топологию ноду
            }
        } else if (command == "remove") { // если команда - удалить ноду
            int id;
            std::cin >> id; // считываем id
            int branch = network.Find(id); // ищем ноду по айди
            if (branch == -1) {
                std::cout << "Error: incorrect node id!\n";
            } else {
                bool is_first = (network.GetFirstId(branch) == id); // проверяем, первая ли нода
                send_message(branches[branch], std::to_string(id) + " remove"); // оправляем сообщение о удалении

                std::string reply = receive_message(branches[branch]);
                std::cout << reply << std::endl; // получаем и выводим ответ
                network.Erase(id); // удаляем ноду из топологии
                if (is_first) { // если это первая нода
                    unbind(branches[branch], id); // анбиндим сокет
                    branches.erase(std::next(branches.begin(), branch)); // удаляем всё что после
                }
            }
        } else if (command == "exec") { // если команда - exec
            size_t count = 0; // количество параметров
            int destId;
            std::cin >> destId; // считываем id ноды, на которой запускаем

            std::string s;
            std::getline(std::cin,s); // считываем последующие параметры
            std::istringstream iss(s);

            // тут мы парсим команды для универсальности (чтобы можно было 1 или 2 параметра передавать)
            std::vector<std::string> params;
            std::string param;
            while (iss >> param) {
                count++;
                params.push_back(param);
            }

            int branch = network.Find(destId); // ищем ноду по айди
            if (branch == -1) {
                std::cout << "Error: incorrect node id!\n";
            } else {
                if (params.size() == 2) { // если нам передали 2 параметра
                    send_message(branches[branch], std::to_string(destId) + "exec " + std::to_string(count) + " " + params[0] + " " + params[1]);
                } else if (params.size() == 1) { // и если 1 параметр
                    send_message(branches[branch], std::to_string(destId) + "exec " + std::to_string(count) + " " + params[0]);
                }
                std::string reply = receive_message(branches[branch]);
                std::cout << reply << "\n"; // получаем и выводим ответ
            }
        } else if (command == "heartbit") { // если команда - heartbit
            int TIME;
            std::cin >> TIME; // считываем таймаут
            for (int i = 0; i < 10; i++) { // для примера - ограничим количество пингов до 10
                for (int i = 0; i < branches.size(); ++i) { // идем по всем сокетам
                    // отправляем команду на пинг ноды
                    send_message(branches[i], std::to_string(i) + " heartbit " + std::to_string(TIME));
                    std::cout << receive_message(branches[i]) << "\n---\n"; // выводим ответ
                }
                sleep(TIME / 1000); // ждём TIME / 1000 секунд
            }
        } else if (command == "exit") { // если команда - выйти
            for (size_t i = 0; i < branches.size(); ++i) { // идём по всем сокетам
                int first_node_id = network.GetFirstId(i); // получаем id первой ноды
                send_message(branches[i], std::to_string(first_node_id) + " remove"); // удаляем её
                std::string reply = receive_message(branches[i]); // получаем ответ
                if (reply != "OK") {
                    std::cout << reply << "\n"; // если не всё ок - выводим
                } else {
                    unbind(branches[i], first_node_id); // иначе - анбиндим сокет
                }
            }
            exit(0);
        } else {
            std::cout << "Incorrect command: " << command << "<!\n"; // если ввели херню - вежливо скажем об этом
        }
    }
}
