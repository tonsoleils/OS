#include "functions.h"
#include <stdlib.h>

// Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные)
// Решето эратосфена
int PrimeCount(int a, int b) {
    int count = 0; // счётчик
    int *arr = malloc(sizeof(int) * (b + 1)); // выделяем массив

    for (int i = a; i <= b; i++) {
        arr[i] = i; // заполняем числами от А до Б
    }

    // берем первое число, удаляем все числа от p^2 до B + 1 с шагом p (p^2, p^2 + p, p^2 + 2p и т.д.)
    // далее находим первое ненулевое число и повторяем снова
    for (int p = 2; p < b + 1; p++) {
        if (arr[p] != 0) {
            count++;
            for (int j = p*p; j < b + 1; j += p) {
                arr[j] = 0;
            }
        }
    }

    return count;
}

void quicksort(int *array, int first, int last);

// Сортировка Хоара
int* Sort(int* array) {
    // Для всех элементов
    int size = array[0];
    quicksort(array, 1, size);
    return array;
}

void quicksort(int *array, int first, int last) {
    int mid, count;
    int f = first;
    int l = last;
    mid = array[(f + l) / 2]; // Выбираем опорный элемент
    // Перераспределяем элементы в массиве таким образом,
    // что элементы, меньшие опорного, помещаются перед ним, а большие или равные - после.
    do {
        while (array[f] < mid) f++;
        while (array[l] > mid) l--;
        if (f <= l) {
            count = array[f];
            array[f] = array[l];
            array[l] = count;
            f++;
            l--;
        }
    } while (f < l);
    // Рекурсивно применить первые два шага к двум подмассивам слева и справа от опорного элемента.
    // Рекурсия не применяется к массиву, в котором только один элемент или отсутствуют элементы.
    if (first < l) quicksort(array, first, l);
    if (f < last) quicksort(array, f, last);
}
