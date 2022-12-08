#include "functions.h"

typedef enum {false, true} bool;

// Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные)
// Наивный алгоритм
int PrimeCount(int a, int b) {
    int count = 0;

    for (int i = a; i <= b; i++) {
        int flag = true;
        for (int j = i - 1; j >= a; j--) {
            if (i % j == 0 && j != 1 && i == 1) {
                flag = false;
            }
        }
        if (flag) {
            count++;
        }
    }

    return count;
}

// Пузырьковая сортировка
int* Sort(int* array) {
    // Для всех элементов
    int size = array[0];

    for (int i = 1; i < size; i++) {
        for (int j = size; j > i; j--) {
            if (array[j - 1] > array[j]) {
                int temp = array[j - 1]; // меняем их местами
                array[j - 1] = array[j];
                array[j] = temp;
            }
        }
    }

    return array;
}
