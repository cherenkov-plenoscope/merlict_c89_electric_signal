/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPHOTONVECTOR_H_
#define MLIESPHOTONVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mliesPhoton.h"

struct mliesPhotonVector {
        struct mliVector vector;
};

struct mliesPhotonVector mliesPhotonVector_init()
{
        struct mliesPhotonVector photons;
        photons.vector = mliVector_init();
        return photons;
}

void mliesPhotonVector_free(struct mliesPhotonVector *photons)
{
        mliVector_free(&photons->vector);
}

int mliesPhotonVector_malloc(
        struct mliesPhotonVector *photons,
        const uint64_t capcity)
{
        mli_c(mliVector_malloc(
                &photons->vector,
                capcity,
                sizeof(struct mliesPhoton)));
        return 1;
  error:
        return 0;
}

struct mliesPhoton mliesPhotonVector_at(
        const struct mliesPhotonVector *photons,
        const uint64_t idx)
{
        struct mliesPhoton spph = *(struct mliesPhoton*)mliVector_at(
                &photons->vector,
                idx);
        return spph;
}

int mliesPhotonVector_push_back(
        struct mliesPhotonVector *photons,
        const struct mliesPhoton photon)
{
        mli_c(mliVector_push_back(&photons->vector, &photon));
        return 1;
  error:
        return 0;
}

#endif
