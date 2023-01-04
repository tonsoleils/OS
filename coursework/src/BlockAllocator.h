#ifndef COURSEWORK_BLOCKALLOCATOR_H
#define COURSEWORK_BLOCKALLOCATOR_H

#include "Allocator.h"

class BlockAllocator : public Allocator {
public:
    explicit BlockAllocator(size_type size) {
        if ((startPointer = malloc(size)) == nullptr) {  // если не смогли аллоцировать столько памяти
            std::cout << "cant allocate such memory" << std::endl; // выводим ошибку
            return;
        }
        totalSize = size; // общий размер
        endPointer = static_cast<void*>(static_cast<char*>(startPointer) + totalSize); // указатель на конец
        auto* header = (Header*) startPointer; // указатель на начало
        header->isAvailable = true; // устанавливаем доступность
        header->size = (totalSize - headerSize); // вычисляем размер
        header->previousSize = 0; // предыдущий размер - 0
        usedSize = headerSize; // устанавливаем используемый размер
    }

    pointer allocate(size_type size) override {  // аллоцировать память
        if (size <= 0) {  // если переданный размер памяти = 0 - ошибка
            std::cout << "blockSize must be > 0" << std::endl;
            return nullptr;
        }
        auto* header = find(size);  // ищем свободную память
        if (header == nullptr) { // если не удалось найти
            //throw std::bad_alloc();
            return nullptr; // возвращаем нуллптр
        }
        splitBlock(header, size); // разделяем блок
        return header + 1;
    }

    void deallocate(pointer ptr) override { // освободить память
        if (!validateAddress(ptr)) { // если что-то не так с адресами
            return; // ничего не делаем и выходим
        }
        auto* header = static_cast<Header*>(ptr) - 1; // хедер
        header->isAvailable = true; // помечаем, что он свободен
        usedSize -= header->size; // и вычитаем размер из используемой памяти
    }

private:
    bool isPrevious(Header* header) { // проверка прошлого хедера
        auto* previous = header->previous();
        return header != startPointer && previous->isAvailable; // хедер не указывает на начало и прошлый хедер доступен
    }

    bool isNextFree(Header* header) { // свободен ли следующий
        auto* next = header->next();
        return header != endPointer && next->isAvailable; // хедер не указывает на конец и следующий доступен
    }
};


#endif //COURSEWORK_BLOCKALLOCATOR_H
