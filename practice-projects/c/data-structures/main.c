#include <stdio.h>
#include "./01-array/array.h"

int main() {
    DSA_Array* arr1 = new_array(3);
    arr1->fill(arr1, 1);
    arr1->resize(arr1, 5);
    arr1->putAt(arr1, -2, 2);
    arr1->putAt(arr1, -1, 3);
    printf("Array 1: ");
    arr1->print(arr1);

    DSA_Array* arr2 = new_array(5);
    arr2->fill(arr2, 4);
    arr2->putAt(arr2, 3, 5);
    arr2->putAt(arr2, 4, 6);
    printf("Array 2: ");
    arr2->print(arr2);

    arr1->merge(arr1, arr2);
    printf("Array 1 + 2: ");
    arr1->print(arr1);

    printf("Array 1 [4..8]: ");
    arr1->print(arr1->slice(arr1, 4, 8));

    DSA_Array* arr3 = arr2->copy(arr2);
    printf("Array 3: ");
    arr3->print(arr3);
    
    arr1->free(arr1);
    arr2->free(arr2);
    arr3->free(arr3);
    return 0;
}
