/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPPHOTONVECTOR_H_
#define MLISPPHOTONVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mlispPhoton.h"

struct mlispPhotonVector {
        struct mliVector vector;
};

struct mlispPhotonVector mlispPhotonVector_init()
{
        struct mlispPhotonVector photons;
        photons.vector = mliVector_init();
        return photons;
}

void mlispPhotonVector_free(struct mlispPhotonVector *photons)
{
        mliVector_free(&photons->vector);
}

int mlispPhotonVector_malloc(
        struct mlispPhotonVector *photons,
        const uint64_t capcity)
{
        mli_c(mliVector_malloc(
                &photons->vector,
                capcity,
                sizeof(struct mlispPhoton)));
        return 1;
  error:
        return 0;
}

struct mlispPhoton mlispPhotonVector_at(
        const struct mlispPhotonVector *photons,
        const uint64_t idx)
{
        struct mlispPhoton spph = *(struct mlispPhoton*)mliVector_at(
                &photons->vector,
                idx);
        return spph;
}

int mlispPhotonVector_push_back(
        struct mlispPhotonVector *photons,
        const struct mlispPhoton photon)
{
        mli_c(mliVector_push_back(&photons->vector, &photon));
        return 1;
  error:
        return 0;
}

#endif
