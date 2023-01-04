#ifndef LAB6_TOPOLOGY_H
#define LAB6_TOPOLOGY_H

#include <list>
#include <stdexcept>

class Topology { // Топология
private:
    std::list<std::list<int>> container; // список списков, наши ноды

public:
    void Insert(int id, int parent_id) { // вставить новую ноду. id - ид ноды, parent_id - ид родителя
        if (parent_id == -1) { // если родитель - управляющий узел
            std::list<int> new_list; // подготавливаем новый список
            new_list.push_back(id); // вставляем туда ноду
            container.push_back(new_list); // и отправляем в хранилище нод
        } else {
            int list_id = Find(parent_id); // ищем родителя по айди
            if (list_id == -1) {
                throw std::runtime_error("Wrong parent id"); // если не нашли - ошибка
            }
            auto it1 = container.begin();
            std::advance(it1, list_id);
            for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
                if (*it2 == parent_id) { // иттерируемся по хранилищу и ищем родителя.
                    it1->insert(++it2, id); // если нашли родителя - вставляем.
                    return;
                }
            }
        }
    }

    int Find(int id) { // ищем ноду по айди
        int cur_list_id = 0;
        for (auto it1 = container.begin(); it1 != container.end(); ++it1) {
            for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
                if (*it2 == id) { // иттерируемся по хранилищу и ищем нужную ноду по айди
                    return cur_list_id; // если нашли - возвращаем её
                }
            }
            ++cur_list_id;
        }
        return -1; // если не нашли - возвращаем -1
    }

    void Erase(int id) { // удалить ноду по айди
        int list_id = Find(id); // ищем ноду по айди
        if (list_id == -1) {
            throw std::runtime_error("Wrong id"); // если не нашли - пробрасываем ошибку.
        }
        auto it1 = container.begin();
        std::advance(it1, list_id);
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (*it2 == id) { // иттерируемся по списку и ищем ноду
                it1->erase(it2, it1->end()); // если нашли - удаляем её
                if (it1->empty()) { // если список после удаления оказался пустым
                    container.erase(it1); // удаляем список
                }
                return;
            }
        }
    }

    int GetFirstId(int list_id) { // получить первый id в списке
        auto it1 = container.begin(); // итератор
        std::advance(it1, list_id); // смещаем итератор
        if (it1->begin() == it1->end()) { // если итератор попал на конец коллекции
            return -1; // возвращаем -1
        }
        return *(it1->begin()); // возвращаем первый элемент
    }
};

#endif // LAB6_TOPOLOGY_H
