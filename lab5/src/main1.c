#include <stdio.h>
#include <stdlib.h>
#include "functions.h"


int main() {
    int choice; // переменная выбора
    printf("Enter type of function (1 - PrimeCount, 2 - Sort): ");
    scanf("%d", &choice); // считываем ее
    if (choice == 1) {
        printf("A = ");
        int a;
        scanf("%d", &a);
        printf("B = ");
        int b;
        scanf("%d", &b);
        printf("Prime count = %d\n", PrimeCount(a, b)); // запускаем и выводим результат работы
    } else if (choice == 2) {
        printf("Size = ");
        int size;
        scanf("%d", &size);
        int* array = malloc(sizeof(int) * (size + 1)); // подготавливаем массив
        array[0] = size; // прячем на первое место его размер
        for (int i = 1; i <= size; i++) {
            int temp;
            scanf("%d", &temp); // считывем элементы массива
            array[i] = temp;
        }
        int* result = Sort(array); // сортируем
        for (int i = 1; i <= size; i++) {
            printf("%d ", result[i]); // выводим результат
        }
        printf("\n");
    } else {
        return 0;
    }
    return 0;
}
