#ifndef COURSEWORK_ALLOCATOR_H
#define COURSEWORK_ALLOCATOR_H

class Allocator {
public:
    //  определение типов
    typedef void value_type;
    typedef value_type* pointer;
    typedef size_t size_type;

    Allocator() = default; // конструктор

    ~Allocator() { // деструктор
        ::free(startPointer);
    }

    virtual pointer allocate(size_type size) = 0;

    virtual void deallocate(pointer ptr) = 0;

    void free() {
        auto* header = static_cast<Header*>(startPointer);
        header->isAvailable = true;
        header->size = (totalSize - headerSize);
        usedSize = headerSize;
    }

protected:
    struct Header {  // заголовок
    public:
        size_type size; // размер
        size_type previousSize; // предыдущий размер
        bool isAvailable; // доступность (свободен/занят)

        inline Header* next() { // следующий заголовок
            return (Header*)((char*) (this + 1) + size);
        }

        inline Header* previous() { // предыдущий заголовок
            return (Header*)((char*) this - previousSize) - 1;
        }
    };

    const size_type headerSize = sizeof(Header); // размер заголовка
    const size_type blockAlignment = 4; // выравнивание блока
    pointer startPointer = nullptr; // указатель на начало
    pointer endPointer = nullptr; // указатель на конец
    size_type totalSize = 0; // общий размер
    size_type usedSize = 0; // используемый размер

    Header* find(size_type size) { // найти свободную память
        auto* header = static_cast<Header*>(startPointer);  // начинаем с начала
        while (!header->isAvailable || header->size < size) {  // идём пока занято
            header = header->next();
            if (header >= endPointer) {
                return nullptr; // если дошли до конца - возвращаем нуллптр
            }
        }
        return header;  // возвращаем хедер пустого блока
    }

    void splitBlock(Header* header, size_type chunk) { // разделить блок
        size_type blockSize = header->size; // считываем параметры
        header->size = chunk;
        header->isAvailable = false;
        if (blockSize - chunk >= headerSize) {  // если размер блока минус чанк меньше размера хедера
            auto *next = header->next(); // получаем следующий заголовок
            next->previousSize = chunk; // сохраняем предыдущий размер
            next->size = blockSize - chunk - headerSize; // вычисляем текущий размер
            next->isAvailable = true; // освобождаем следущий блок
            usedSize += chunk + headerSize; // вычисляем используемый размер
            auto *followed = next->next(); // следующий хедер за следующим
            if (followed < endPointer) {  // если он за концом
                followed->previousSize = next->size; // присваиваем прошлому размеру размер текущего
            }
        } else {
            header->size = blockSize; // присваиваем размер
            usedSize += blockSize; // увеличиваем используемый размер
        }
    }

    bool validateAddress(void* ptr) { // проверка адреса
        auto* header = static_cast<Header*>(startPointer);  // начинаем с начала
        while (header < endPointer) { // пока не дошли до конца
            if (header + 1 == ptr) {  // проверка
                return true // если дошли до конца и указатели совпали возвращаем true
            }
            header = header->next();
        }
        return false; // иначе возвращаем false
    }
};


#endif //COURSEWORK_ALLOCATOR_H
