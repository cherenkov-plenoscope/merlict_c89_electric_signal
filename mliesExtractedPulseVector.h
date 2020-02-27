/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESEXTRACTEDPULSEVECTOR_H_
#define MLIESEXTRACTEDPULSEVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mliesExtractedPulse.h"

struct mliesExtractedPulseVector {
        struct mliVector vector;
};

struct mliesExtractedPulseVector mliesExtractedPulseVector_init()
{
        struct mliesExtractedPulseVector pulses;
        pulses.vector = mliVector_init();
        return pulses;
}

void mliesExtractedPulseVector_free(struct mliesExtractedPulseVector *pulses)
{
        mliVector_free(&pulses->vector);
}

int mliesExtractedPulseVector_malloc(
        struct mliesExtractedPulseVector *pulses,
        const uint64_t capcity)
{
        mli_c(mliVector_malloc(
                &pulses->vector,
                capcity,
                sizeof(struct mliesExtractedPulse)));
        return 1;
  error:
        return 0;
}

struct mliesExtractedPulse mliesExtractedPulseVector_at(
        const struct mliesExtractedPulseVector *pulses,
        const uint64_t idx)
{
        struct mliesExtractedPulse spph = *(struct mliesExtractedPulse*)
                mliVector_at(
                    &pulses->vector,
                    idx);
        return spph;
}

int mliesExtractedPulseVector_push_back(
        struct mliesExtractedPulseVector *pulses,
        const struct mliesExtractedPulse pulse)
{
        mli_c(mliVector_push_back(&pulses->vector, &pulse));
        return 1;
  error:
        return 0;
}

void mliesExtractedPulseVector_assign(
    struct mliesExtractedPulseVector *pulses,
    const uint64_t index,
    const struct mliesExtractedPulse pulse)
{
        mliVector_assign(&pulses->vector, index, &pulse);
}



#endif
