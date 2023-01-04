#include <iostream>
#include <unistd.h>
#include "BlockAllocator.h"
#include "BuddyAllocator.h"
#include <vector>
#include <chrono>

int main() {
    // инициализируем аллокаторы
    BlockAllocator all = BlockAllocator(1024);
    BuddyAllocator allocator = BuddyAllocator(1024);

    // вектор для памяти
    std::vector<void*> v(100000);

    auto t1 = std::chrono::high_resolution_clock::now();

    // двойники, маллок
    for (int i = 0; i < v.size(); i++) {
        v[i] = allocator.malloc(i + (i / 2 % 5 + 1));
    }

    auto t1_end = std::chrono::high_resolution_clock::now();
    auto t1_duration = std::chrono::duration_cast<std::chrono::microseconds>(t1_end - t1).count();
    std::cout << "Алгоритм двойников malloc: " << t1_duration << std::endl;

    auto t2 = std::chrono::high_resolution_clock::now();

    // двойники, освобождение
    for (auto & i : v) {
        allocator.free(i);
    }

    auto t2_end = std::chrono::high_resolution_clock::now();
    auto t2_duration = std::chrono::duration_cast<std::chrono::microseconds>( t2_end - t2 ).count();
    std::cout << "Алгоритм двойников free: " << t2_duration << std::endl;

    t1 = std::chrono::high_resolution_clock::now();

    // список свободных блоков, маллок
    for (int i = 0; i < v.size(); i++) {
        v[i] = all.allocate(i + (i / 2 % 5 + 1));
    }

    t1_end = std::chrono::high_resolution_clock::now();
    t1_duration = std::chrono::duration_cast<std::chrono::microseconds>(t1_end - t1).count();
    std::cout << "Список свободных блоков malloc: " << t1_duration << std::endl;

    t2 = std::chrono::high_resolution_clock::now();

    // список свободных блоков, освобождение
    for (auto & i : v) {
        all.deallocate(i);
    }
    t2_end = std::chrono::high_resolution_clock::now();
    t2_duration = std::chrono::duration_cast<std::chrono::microseconds>( t2_end - t2 ).count();
    std::cout << "Список свободных блоков free: " << t2_duration << std::endl;

    return 0;
}
