#ifndef LAB6_ZMQ_F_H
#define LAB6_ZMQ_F_H

#include <iostream>
#include <string>
#include <zmq.hpp>

const int MAIN_PORT = 4040;

void send_message(zmq::socket_t& socket, const std::string& msg) { // отправить сообщение
    zmq::message_t message(msg.size()); // подготавливаем переменную
    memcpy(message.data(), msg.c_str(), msg.size()); // копирует size байтов из msg.c_str() в message.data()
    socket.send(message); // возврат результата, отсылает очередь сообщений, созданных в message
}

std::string receive_message(zmq::socket_t& socket) { // получить сообщение
    zmq::message_t message; // подготавливаем переменную
    int chars_read; // переменная для количества считанных символов
    try {
        chars_read = (int)socket.recv(&message);  // получение сообщений
    }
    catch (...) {
        chars_read = 0; // отлавливаем исключение
    }
    if (chars_read == 0) {
        return "Error"; // если поймали исключение или считали 0 символов - ошибка
    }
    std::string received_msg(static_cast<char*>(message.data()), message.size());
    return received_msg;
}

void connect(zmq::socket_t& socket, int id) {
    std::string adress = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.connect(adress);
}

void disconnect(zmq::socket_t& socket, int id) {
    std::string adress = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.disconnect(adress);
}

void bind(zmq::socket_t& socket, int id) {
    std::string adress = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.bind(adress); // Асинхронно привязывается к конечной точке (endpoint).
}

void unbind(zmq::socket_t& socket, int id) {
    std::string adress = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.unbind(adress); // Отменяет привязку к ранее привязанной конечной точке.
}

#endif // LAB6_ZMQ_F_H