/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPHOTONVECTOR_H_
#define MLIESPHOTONVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliDynArray_test.h"
#include "mliesPhoton.h"


MLIDYNARRAY_TEMPLATE(mlies, Photon, struct mliesPhoton)


MLIDYNARRAY_2D_TEMPLATE(mlies, Photon, mliesDynPhoton)


#endif
