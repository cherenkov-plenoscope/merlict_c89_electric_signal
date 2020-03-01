/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPULSEVECTOR_H_
#define MLIESPULSEVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliDynArray_test.h"
#include "mliesPulse.h"


MLIDYNARRAY_TEMPLATE(mlies, Pulse, struct mliesPulse)


MLIDYNARRAY_2D_TEMPLATE(mlies, Pulse, mliesDynPulse)


#endif
