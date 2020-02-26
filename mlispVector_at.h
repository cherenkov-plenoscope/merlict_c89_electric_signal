/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISP_VECTOR_AT_H_
#define MLISP_VECTOR_AT_H_

#include "../merlict_c89/mliVector.h"

#define mlispVector_at(ptr_vector, ptr_returnarg, type, idx) { \
        type out = *(type*)mliVector_at(ptr_vector, idx); \
        ptr_returnarg* = out; \
}


#endif
