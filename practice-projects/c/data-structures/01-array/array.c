#include "array.h"
#include <stdio.h>
#include <stdlib.h>

// Helper: parses index so it is in range
int parse_index(int idx, size_t len) {
    if (idx < 0 && idx + len >= 0) idx = len + idx;
    return idx;
}

// Helper: throws error for empty array
void empty_array_error(char* name, char* func) {
    printf("Error: Provided %s doesn't exist for %s (try initializing it)\n", name, func);
    exit(DSA_EmptyArrayError);
}

void DSA_putAt_impl(DSA_Array* self, int idx, int val) {
    if (!self) empty_array_error("self", "indexing");
    idx = parse_index(idx, self->length);
    if (idx < 0 || idx >= self->length) {
        printf("Error: array index out of bounds (%d)\n", idx);
        self->free(self);
        exit(DSA_IndexOutOfBoundsError);
    }

    int* valAddress = self->_data + idx;
    *valAddress = val;
}

int DSA_getAt_impl(DSA_Array* self, int idx) {
    if (!self) empty_array_error("self", "indexing");
    idx = parse_index(idx, self->length);
    if (idx < 0 || idx >= self->length) {
        printf("Error: array index out of bounds (%d)\n", idx);
        self->free(self);
        exit(DSA_IndexOutOfBoundsError);
    }

    int* valAddress = self->_data + idx;
    return *valAddress;
}

void DSA_free_impl(DSA_Array* self) {
    if (self && self->_data) free(self->_data);
    if (self) free(self);
}

void DSA_resize_impl(DSA_Array* self, size_t newLen) {
    if (!self) empty_array_error("self", "resizing");
    if (newLen == self->length) return;
    int* tmp = (int*)realloc(self->_data, sizeof(int)*newLen);
    if (!tmp) {
        printf("Error: Failed to allocate memory for array while resizing to %ld\n", newLen);
        exit(DSA_AllocFailError);
    }

    self->_data = tmp;
    self->length = newLen;
}

void DSA_fill_impl(DSA_Array* self, int val) {
    if (!self) empty_array_error("self", "filling");
    for (size_t i=0; i<self->length; i++) self->putAt(self, i, val);
}

DSA_Array* DSA_copy_impl(DSA_Array* self) {
    if (!self) empty_array_error("self", "copying");
    DSA_Array* arr_cp = new_array(self->length);
    for (size_t i=0; i<self->length; i++) arr_cp->putAt(arr_cp, i, self->getAt(self, i));
    return arr_cp;
}

DSA_Array* DSA_slice_impl(DSA_Array* self, int start, int end) {
    if (!self) empty_array_error("self", "slicing");
    start = parse_index(start, self->length);
    if (start < 0 || start >= self->length) {
        printf("Error: start index out of bounds for array slicing (%d)\n", start);
        self->free(self);
        exit(DSA_IndexOutOfBoundsError);
    }

    end = parse_index(end, self->length);
    if (end < 0 || end >= self->length) {
        printf("Error: end index out of bounds for array slicing (%d)\n", end);
        self->free(self);
        exit(DSA_IndexOutOfBoundsError);
    }

    DSA_Array* arr_slice = new_array(end - start);
    for (size_t i=start; i<end; i++)
      arr_slice->putAt(arr_slice, i-start, self->getAt(self, i));
    return arr_slice;
}

void DSA_print_impl(DSA_Array* self) {
    if (!self) empty_array_error("self", "printing");
    printf("[");
    for (size_t i=0; i<self->length; i++) {
        printf("%d", self->getAt(self, i));
        if (i != self->length-1) printf(", ");
    }
    printf("]\n");
}

void DSA_merge_impl(DSA_Array* self, DSA_Array* other) {
    if (!self) {
        other->free(other);
        empty_array_error("self array", "merging");
    }
    if (!other) {
        self->free(self);
        empty_array_error("other array", "merging");
    }
    size_t len1 = self->length;
    size_t len2 = other->length;
    size_t newLen = len1 + len2;
    self->resize(self, newLen);
    for (int i=0; i<len2; i++) self->putAt(self, i+len1, other->getAt(other, i));
}

DSA_Array* new_array(size_t len) {
    DSA_Array* arr = (DSA_Array*)malloc(sizeof(DSA_Array));
    if (!arr) goto alloc_fail;

    arr->_data = (int*)calloc(len, sizeof(int));
    if (!arr->_data) goto alloc_fail;

    arr->length = len;
    arr->putAt = DSA_putAt_impl;
    arr->getAt = DSA_getAt_impl;
    arr->free = DSA_free_impl;
    arr->resize = DSA_resize_impl;
    arr->fill = DSA_fill_impl;
    arr->copy = DSA_copy_impl;
    arr->slice = DSA_slice_impl;
    arr->print = DSA_print_impl;
    arr->merge = DSA_merge_impl;

    return arr;
alloc_fail:
    printf("Error: Failed to allocate memory for array(%ld)\n", len);
    exit(DSA_AllocFailError);
}
