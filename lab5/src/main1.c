#include <stdio.h>
#include <stdlib.h>
#include "functions.h"


int main() {
    int choice;
    printf("Enter type of function (1 - PrimeCount, 2 - Sort): ");
    scanf("%d", &choice);
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
        return 0;
    }
    return 0;
}
