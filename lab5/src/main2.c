#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "functions.h"


int main() {
    int choice;
    int (*PrimeCount)(int A, int B);
    int* (*Sort)(int* array);
    char* libs[] = {"libfunc1.so", "libfunc2.so"};
    int lib = 0;
    void* handler = NULL;

    while (printf("Enter type of funtion (0 - Change implementation, 1 - PrimeCount, 2 - Sort): ") && scanf("%d", &choice) > 0) {
        if (choice == 0) {
            if (handler) {
                // dlclose(handle) - уменьшает на единицу счетчик ссылок на указатель динамической библиотеки handle.
                dlclose(handler);
            }
            lib = 1 - lib; // переключатель
            printf("Implementation changed!\n");
        } else {
            // dlopen(filename, flag) - dlopen загружает динамическую библиотеку,
            // имя которой указано в строке filename, и возвращает прямой указатель
            // на начало динамической библиотеки.
            // flag RTLD_LAZY, подразумевает разрешение неопределенных символов в виде кода,
            // содержащегося в исполняемой динамической библиотеке
            handler = dlopen(libs[lib], RTLD_LAZY);
            if (!handler) { // если произошла ошибка
                fprintf(stderr, "dlopen() error: %s\n", dlerror()); // выводим её
                exit(1); // и выходим
            }

            // Функции dlsym(handler, symbol) передаётся handler динамически загруженного объекта,
            // возвращаемого dlopen и имя символа (с null в конце). В результате функция
            // возвращает адрес, по которому символ расположен в памяти.
            PrimeCount = dlsym(handler, "PrimeCount");
            Sort = dlsym(handler, "Sort");
            if (choice == 1) {
                printf("A = ");
                int a;
                scanf("%d", &a);
                printf("B = ");
                int b;
                scanf("%d", &b);
                printf("Prime count = %d\n", PrimeCount(a, b));
            } else if (choice == 2) {
                printf("Size = ");
                int size;
                scanf("%d", &size);
                int* array = malloc(sizeof(int) * (size + 1));
                array[0] = size;
                for (int i = 1; i <= size; i++) {
                    int temp;
                    scanf("%d", &temp);
                    array[i] = temp;
                }
                int* result = Sort(array);
                for (int i = 1; i <= size; i++) {
                    printf("%d ", result[i]);
                }
                printf("\n");
            } else {
                printf("Exit\n");
                dlclose(handler);
                break;
            }
        }

    }

}