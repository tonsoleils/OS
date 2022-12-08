#include "functions.h"
#include <stdlib.h>

// Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные)
// Решето эратосфена
int PrimeCount(int a, int b) {
    int count = 0;
    int *arr = malloc(sizeof(int) * (b + 1));

    for (int i = a; i <= b; i++) {
        arr[i] = i;
    }

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
    mid = array[(f + l) / 2]; //вычисление опорного элемента
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
    if (first < l) quicksort(array, first, l);
    if (f < last) quicksort(array, f, last);
}
