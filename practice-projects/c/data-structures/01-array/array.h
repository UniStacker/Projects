#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

#define DSA_IndexOutOfBoundsError 1
#define DSA_AllocFailError 2
#define DSA_EmptyArrayError 3

typedef struct DSA_Array DSA_Array;
struct DSA_Array {
    int* _data;
    size_t length;
    void (*putAt) (DSA_Array*, int, int);
    int (*getAt) (DSA_Array*, int);
    void (*free) (DSA_Array*);
    void (*resize) (DSA_Array*, size_t);
    void (*fill) (DSA_Array*, int);
    DSA_Array* (*copy) (DSA_Array*);
    DSA_Array* (*slice) (DSA_Array*, int, int);
    void (*print) (DSA_Array*);
    void (*merge) (DSA_Array*, DSA_Array*);
};

DSA_Array* new_array(size_t len);

#endif // !ARRAY_H
