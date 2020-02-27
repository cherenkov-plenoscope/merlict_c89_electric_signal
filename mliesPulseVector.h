/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPPULSEVECTOR_H_
#define MLISPPULSEVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mlispPulse.h"

struct mlispPulseVector {
        struct mliVector vector;
};

struct mlispPulseVector mlispPulseVector_init()
{
        struct mlispPulseVector pulses;
        pulses.vector = mliVector_init();
        return pulses;
}

void mlispPulseVector_free(struct mlispPulseVector *pulses)
{
        mliVector_free(&pulses->vector);
}

int mlispPulseVector_malloc(
        struct mlispPulseVector *pulses,
        const uint64_t capcity)
{
        mli_c(mliVector_malloc(
                &pulses->vector,
                capcity,
                sizeof(struct mlispPulse)));
        return 1;
  error:
        return 0;
}

struct mlispPulse mlispPulseVector_at(
        const struct mlispPulseVector *pulses,
        const uint64_t idx)
{
        struct mlispPulse spph = *(struct mlispPulse*)mliVector_at(
                &pulses->vector,
                idx);
        return spph;
}

int mlispPulseVector_push_back(
        struct mlispPulseVector *pulses,
        const struct mlispPulse pulse)
{
        mli_c(mliVector_push_back(&pulses->vector, &pulse));
        return 1;
  error:
        return 0;
}

#endif
