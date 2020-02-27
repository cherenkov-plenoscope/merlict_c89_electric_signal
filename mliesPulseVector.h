/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPULSEVECTOR_H_
#define MLIESPULSEVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mliesPulse.h"

struct mliesPulseVector {
        struct mliVector vector;
};

struct mliesPulseVector mliesPulseVector_init()
{
        struct mliesPulseVector pulses;
        pulses.vector = mliVector_init();
        return pulses;
}

void mliesPulseVector_free(struct mliesPulseVector *pulses)
{
        mliVector_free(&pulses->vector);
}

int mliesPulseVector_malloc(
        struct mliesPulseVector *pulses,
        const uint64_t capcity)
{
        mli_c(mliVector_malloc(
                &pulses->vector,
                capcity,
                sizeof(struct mliesPulse)));
        return 1;
  error:
        return 0;
}

struct mliesPulse mliesPulseVector_at(
        const struct mliesPulseVector *pulses,
        const uint64_t idx)
{
        struct mliesPulse spph = *(struct mliesPulse*)mliVector_at(
                &pulses->vector,
                idx);
        return spph;
}

int mliesPulseVector_push_back(
        struct mliesPulseVector *pulses,
        const struct mliesPulse pulse)
{
        mli_c(mliVector_push_back(&pulses->vector, &pulse));
        return 1;
  error:
        return 0;
}

#endif
