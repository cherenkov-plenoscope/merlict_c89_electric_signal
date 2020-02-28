/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPHOTONVECTOR_H_
#define MLIESPHOTONVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mliesPhoton.h"

struct mliesPhotonChannels {
        uint64_t num_channels;
        struct mliVector *channels;
};

struct mliesPhotonChannels mliesPhotonChannels_init()
{
        struct mliesPhotonChannels phch;
        phch.num_channels = 0u;
        phch.channels = NULL;
        return phch;
}

void mliesPhotonChannels_free(struct mliesPhotonChannels *phch)
{
        uint64_t ch;
        for (ch = 0u; ch < phch->num_channels; ch++) {
                mliVector_free(&phch->channels[ch]);
        }
        free(phch->channels);
        phch->num_channels = 0u;
        phch->channels = NULL;
}

int mliesPhotonChannels_malloc(
    struct mliesPhotonChannels *phch,
    const uint64_t num_channels)
{
        uint64_t ch = 0u;
        mliesPhotonChannels_free(phch);
        phch->num_channels = num_channels;
        mli_malloc(phch->channels, struct mliVector, phch->num_channels);
        for (ch = 0; ch < phch->num_channels; ch++) {
                phch->channels[ch] = mliVector_init();
                mliVector_malloc(
                        &phch->channels[ch],
                        0,
                        sizeof(struct mliesPhoton));
        }
        return 1;
    error:
        return 0;
}

struct mliesPhoton mliesPhotonChannels_at(
        const struct mliesPhotonChannels *phch,
        const uint64_t channel,
        const uint64_t idx)
{
        struct mliesPhoton spph = *(struct mliesPhoton*)mliVector_at(
                &phch->channels[channel],
                idx);
        return spph;
}

int mliesPhotonChannels_push_back(
        const struct mliesPhotonChannels *phch,
        const uint64_t channel,
        const struct mliesPhoton photon)
{
        mli_c(mliVector_push_back(&phch->channels[channel], &photon));
        return 1;
  error:
        return 0;
}

#endif
