/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPULSEVECTOR_H_
#define MLIESPULSEVECTOR_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mliesPulse.h"

struct mliesPulseChannels {
        uint64_t num_channels;
        struct mliVector *channels;
};

struct mliesPulseChannels mliesPulseChannels_init()
{
        struct mliesPulseChannels phch;
        phch.num_channels = 0u;
        phch.channels = NULL;
        return phch;
}

void mliesPulseChannels_free(struct mliesPulseChannels *phch)
{
        uint64_t ch;
        for (ch = 0u; ch < phch->num_channels; ch++) {
                mliVector_free(&phch->channels[ch]);
        }
        free(phch->channels);
        phch->num_channels = 0u;
        phch->channels = NULL;
}

int mliesPulseChannels_malloc(
    struct mliesPulseChannels *phch,
    const uint64_t num_channels)
{
        uint64_t ch = 0u;
        mliesPulseChannels_free(phch);
        phch->num_channels = num_channels;
        mli_malloc(phch->channels, struct mliVector, phch->num_channels);
        for (ch = 0; ch < phch->num_channels; ch++) {
                phch->channels[ch] = mliVector_init();
                mliVector_malloc(
                        &phch->channels[ch],
                        0,
                        sizeof(struct mliesPulse));
        }
        return 1;
    error:
        return 0;
}

struct mliesPulse mliesPulseChannels_at(
        const struct mliesPulseChannels *phch,
        const uint64_t channel,
        const uint64_t idx)
{
        struct mliesPulse spph = *(struct mliesPulse*)mliVector_at(
                &phch->channels[channel],
                idx);
        return spph;
}

int mliesPulseChannels_push_back(
        const struct mliesPulseChannels *phch,
        const uint64_t channel,
        const struct mliesPulse photon)
{
        mli_c(mliVector_push_back(&phch->channels[channel], &photon));
        return 1;
  error:
        return 0;
}

#endif
