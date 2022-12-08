#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    int array_size = 10;
    int* array = malloc(sizeof(int) * (array_size + 1));
    array[0] = array_size;
    for (int i = 1; i <= array_size; i++) {
        array[i] = 11 - i;
    }
    int* new_array = Sort(array);
    for (int i = 1; i <= array_size; i++) {
        printf("%d ", new_array[i]);
    }
    printf("\n");
    return 0;
}
