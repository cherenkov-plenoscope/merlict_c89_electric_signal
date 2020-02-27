/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPEXTRACTEDPULSEVECTOR_H_
#define MLISPEXTRACTEDPULSEVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mlispExtractedPulse.h"

struct mlispExtractedPulseVector {
        struct mliVector vector;
};

struct mlispExtractedPulseVector mlispExtractedPulseVector_init()
{
        struct mlispExtractedPulseVector pulses;
        pulses.vector = mliVector_init();
        return pulses;
}

void mlispExtractedPulseVector_free(struct mlispExtractedPulseVector *pulses)
{
        mliVector_free(&pulses->vector);
}

int mlispExtractedPulseVector_malloc(
        struct mlispExtractedPulseVector *pulses,
        const uint64_t capcity)
{
        mli_c(mliVector_malloc(
                &pulses->vector,
                capcity,
                sizeof(struct mlispExtractedPulse)));
        return 1;
  error:
        return 0;
}

struct mlispExtractedPulse mlispExtractedPulseVector_at(
        const struct mlispExtractedPulseVector *pulses,
        const uint64_t idx)
{
        struct mlispExtractedPulse spph = *(struct mlispExtractedPulse*)
                mliVector_at(
                    &pulses->vector,
                    idx);
        return spph;
}

int mlispExtractedPulseVector_push_back(
        struct mlispExtractedPulseVector *pulses,
        const struct mlispExtractedPulse pulse)
{
        mli_c(mliVector_push_back(&pulses->vector, &pulse));
        return 1;
  error:
        return 0;
}

void mlispExtractedPulseVector_assign(
    struct mlispExtractedPulseVector *pulses,
    const uint64_t index,
    const struct mlispExtractedPulse pulse)
{
        mliVector_assign(&pulses->vector, index, &pulse);
}



#endif
