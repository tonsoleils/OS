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

    while (printf("Enter type of funtion: ") && scanf("%d", &choice) > 0) {
        if (choice == 0) {
            if (handler) {
                dlclose(handler);
            }
            lib = 1 - lib;
            printf("Implementation changed!\n");
        } else {
            handler = dlopen(libs[lib], RTLD_LAZY);
            if (!handler) {
                fprintf(stderr, "dlopen() error: %s\n", dlerror());
                exit(1);
            }

            PrimeCount = dlsym(handler, "PrimeCount");
            Sort = dlsym(handler, "Sort");
            if (choice == 2) {
                printf("A = ");
                int a;
                scanf("%d", &a);
                printf("B = ");
                int b;
                scanf("%d", &b);
                printf("Prime count = %d\n", PrimeCount(a, b));
            } else if (choice == 1) {
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