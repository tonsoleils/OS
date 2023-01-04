#include <iostream>
#include <cstring>
#include <cmath>

// мотематика
#define DIV_ROUNDUP(A, B) \
({ \
    typeof(A) _a_ = A; \
    typeof(B) _b_ = B; \
    (_a_ + (_b_ - 1)) / _b_; \
})

// еще больше мотематики
#define ALIGN_UP(A, B) \
({ \
    typeof(A) _a__ = A; \
    typeof(B) _b__ = B; \
    DIV_ROUNDUP(_a__, _b__) * _b__; \
})


struct BuddyBlock { // структура блока
    size_t blockSize; // размер
    bool isFree; // свободен или занят
};

class BuddyAllocator {
private:
    BuddyBlock *head = nullptr; // голова
    BuddyBlock *tail = nullptr; // хвост
    void *data = nullptr; // данные

    bool expanded = false;

    BuddyBlock* next(BuddyBlock *block) { // получить следующий блок
        return reinterpret_cast<BuddyBlock*>(reinterpret_cast<uint8_t*>(block) + block->blockSize);
    }

    BuddyBlock* split(BuddyBlock *block, size_t size) { // разделить блок
        if (block != nullptr && size != 0) { // если блок не пустой или размер не равен нулю
            while (size < block->blockSize) { // пока размер меньше размера входного блока
                size_t sz = block->blockSize >> 1; // вычисляем размер
                block->blockSize = sz; // присваиваем его входному блоку
                block = this->next(block); // присваиваем блоку следующий блок
                block->blockSize = sz; // присваиваем ему размер
                block->isFree = true; // делаем его свободным
            }
            if (size <= block->blockSize) return block; // если всё ок возвращаем его
        }
        return nullptr; // иначе возвращаем нуллптр
    }

    BuddyBlock* findBest(size_t size) { // первый доступный блок
        if (size == 0) return nullptr;

        BuddyBlock *bestBlock = nullptr; // заводим переменные
        BuddyBlock *block = this->head;
        BuddyBlock *buddy = this->next(block); // двойник

        if (buddy == this->tail && block->isFree) { // если двойник - хвост и блок свободен
            return this->split(block, size); // возвращаем сплит
        }

        while (block < this->tail && buddy < this->tail) { // пока наш блок меньше чем хвост и близнец меньше чем хвост
            if (block->isFree && buddy->isFree && block->blockSize == buddy->blockSize) {
                // если блок и двойник свободны и размер блока равен размеру двойника
                block->blockSize <<= 1;  // вычисляем размер
                if (size <= block->blockSize && (bestBlock == nullptr || block->blockSize <= bestBlock->blockSize)) {
                    // если переданный размер меньше чем блок и лучший блок - нуллптр или размер блока меньше чем размер
                    // наиболее подходящего блока
                    bestBlock = block; // лучший блок - текущий блок
                }

                block = this->next(buddy); // присваиваем текущему блоку следующий
                if (block < this->tail)  { // если блок меньше чем хвост
                    buddy = this->next(block); // двойнику присваиваем следующий блок
                }
                continue;
            }

            // если блок - свободен и размер меньше чем размер блока и размер блока меньше чем размер лучшего блока
            if (block->isFree && size <= block->blockSize && (bestBlock == nullptr || block->blockSize <= bestBlock->blockSize))  {
                bestBlock = block;
            }
            // тоже самое
            if (buddy->isFree && size <= buddy->blockSize && (bestBlock == nullptr || buddy->blockSize < bestBlock->blockSize)) {
                bestBlock = buddy;
            }

            // если размер блока меньше чем размер двойника
            if (block->blockSize <= buddy->blockSize) {
                block = this->next(buddy); // присваиваем блоку следующий
                if (block < this->tail) { // если блок меньше хвоста
                    buddy = this->next(block); // двойнику присваиваем следующий
                }
            } else {
                block = buddy;  // блоку присваиваем двойника
                buddy = this->next(buddy);  // двойнику присваиваем следующий блок
            }
        }

        // если мы нашли лучший блок
        if (bestBlock != nullptr) {
            return this->split(bestBlock, size); // разбиваем его
        }

        return nullptr;
    }

    size_t requiredSize(size_t size) {  // вычисление необходимого размера
        size_t actual_size = sizeof(BuddyBlock); // текущий размер

        size += sizeof(BuddyBlock); // прибавляем размер
        size = ALIGN_UP(size, sizeof(BuddyBlock)); // производим математику

        while (size > actual_size) { // пока размер меньше текущего
            actual_size <<= 1; // вычисляем текущий размер
        }

        return actual_size; // возвращаем текущий размер
    }

    void coalescence() { // объединение
        while (true) {
            BuddyBlock *block = this->head; // заводим блок
            BuddyBlock *buddy = this->next(block); // заводим близнеца, равного следующему блоку

            bool noCoalescence = true;
            // пока блок меньше хвоста и близнец меньше хвоста
            while (block < this->tail && buddy < this->tail) {
                // если блок и двойник свободны и их размеры равны
                if (block->isFree && buddy->isFree && block->blockSize == buddy->blockSize) {
                    block->blockSize <<= 1; // вычисляем размер блока
                    block = this->next(block); // берем следующий блок
                    if (block < this->tail) { // если блок меньше хвоста
                        buddy = this->next(block); // присваиваем двойнику следующий блок
                        noCoalescence = false; // флаг
                    }
                // если размер блока меньше размера близнеца
                } else if (block->blockSize < buddy->blockSize) {
                    block = buddy;  // смещаем блок и близнеца
                    buddy = this->next(buddy);
                } else {  // в любом другом случае
                    block = this->next(buddy); // перемещаем блок за двойника
                    if (block < this->tail) { // если блок меньше чем хвост
                        buddy = this->next(block); // ставим двойника за блок
                    }
                }
            }

            if (noCoalescence) { // если флаг
                return;  // прекращаем это безобразие
            }
        }
    }

public:
    bool debug = false; // флаг для дебага

    BuddyAllocator(size_t size) { // конструктор
        this->expand(size);
    }

    ~BuddyAllocator() { // деструктор
        this->head = nullptr; // "опустошаем" всё
        this->tail = nullptr;
        std::free(this->data);
    }

    void expand(size_t size) { // расширить блок
        if (this->head) { // если это голова
            size += this->head->blockSize; // расширяем размер
        }
        size = pow(2, ceil(log(size) / log(2))); // вычисляем размер


        this->data = std::realloc(this->data, size); // перемещаем данные

        this->head = static_cast<BuddyBlock*>(data); // перемещаем указатель головы на данные
        this->head->blockSize = size; // присваиваем размер
        this->head->isFree = true; // делаем блок свободным

        this->tail = next(head); // присваиваем хвосту следущий блок за головой

        if (this->debug) { // отладочные выводы
            std::cout << "Expanded the heap. Current blockSize: " << size << " bytes" << std::endl;
        }
    }

    void setsize(size_t size) { // установить размер
        size -= this->head->blockSize;
        this->expand(size);
    }

    void *malloc(size_t size) { // выделить память
        if (size == 0) return nullptr; // если передали 0 - шлём куда подальше

        size_t actualSize = this->requiredSize(size); // вычисляем текущий размер

        BuddyBlock *found = this->findBest(actualSize); // ищем лучший блок
        if (found == nullptr) { // если ничего не нашли
            this->coalescence(); // объединяем блоки
            found = this->findBest(actualSize); // снова ищем лучший
        }

        if (found != nullptr) { // если нашли
            found->isFree = false; // помечаем, что блок занят
            this->expanded = false;
            return reinterpret_cast<void*>(reinterpret_cast<char*>(found) + sizeof(BuddyBlock)); // возвращаем блок
        }

        if (this->expanded) { // если расширили блок
            this->expanded = false;
            return nullptr; // возвращаем нуллптр
        }
        this->expanded = true;
        this->expand(size); // расширяем блок
        return this->malloc(size); // снова запускаем маллок на бОльшем блоке
    }


    void free(void *ptr) { // освобождение
        if (ptr == nullptr) {  // если нам подсунули нуллптр
            return;  // запускаем форк бомбу
        }

        // получаем блок
        BuddyBlock *block = reinterpret_cast<BuddyBlock*>(reinterpret_cast<char*>(ptr) - sizeof(BuddyBlock));
        block->isFree = true; // помечаем его свободным

        if (this->debug) { // отладочная информация
            std::cout << "Freed " << block->blockSize - sizeof(BuddyBlock) << " bytes" << std::endl;
        }

        // объединяем блоки
        this->coalescence();
    }
};