/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIES_EXTRACTCHANNELS_H_
#define MLIES_EXTRACTCHANNELS_H_

#include <stdint.h>
#include "../merlict_c89/mliVector.h"
#include "mliesExtract.h"
#include "mlies_constants.h"

struct mliesExtractChannels {
        double time_slice_duration;
        uint64_t num_time_slices;

        uint64_t num_channels;
        struct mliVector *channels;
};

struct mliesExtractChannels mliesExtractChannels_init()
{
        struct mliesExtractChannels phs;
        phs.time_slice_duration = .0;
        phs.num_time_slices = MLIES_NUM_TIME_SLICES;
        phs.num_channels = 0u;
        phs.channels = NULL;
        return phs;
}

void mliesExtractChannels_free(struct mliesExtractChannels *phs)
{
        uint64_t ch;
        for (ch = 0u; ch < phs->num_channels; ch++) {
                mliVector_free(&phs->channels[ch]);
        }
        free(phs->channels);
        phs->num_channels = 0u;
        phs->channels = NULL;
}

int mliesExtractChannels_malloc(
        struct mliesExtractChannels *phs,
        const uint64_t num_channels)
{
        uint64_t ch = 0u;
        mliesExtractChannels_free(phs);
        phs->num_channels = num_channels;
        mli_malloc(phs->channels, struct mliVector, phs->num_channels);
        for (ch = 0; ch < phs->num_channels; ch++) {
                phs->channels[ch] = mliVector_init();
                mliVector_malloc(
                        &phs->channels[ch],
                        0u,
                        sizeof(struct mliesExtract));
        }
        return 1;
    error:
        return 0;
}

struct mliesExtract mliesExtractChannels_at(
        const struct mliesExtractChannels *phs,
        const uint64_t channel,
        const uint64_t idx)
{
        struct mliesExtract expu = *(struct mliesExtract*)
                mliVector_at(
                    &phs->channels[channel],
                    idx);
        return expu;
}

int mliesExtractChannels_push_back(
        struct mliesExtractChannels *phs,
        const uint64_t channel,
        const struct mliesExtract expulse)
{
        mli_c(mliVector_push_back(&phs->channels[channel], &expulse));
        return 1;
  error:
        return 0;
}

void mliesExtractChannels_assign(
        struct mliesExtractChannels *phs,
        const uint64_t channel,
        const uint64_t index,
        const struct mliesExtract expulse)
{
        mliVector_assign(&phs->channels[channel], index, &expulse);
}

uint64_t mliesExtractChannels_num_pulses(const struct mliesExtractChannels *phs)
{
        uint64_t num_pulses = 0u;
        uint64_t ch = 0u;
        for (ch = 0; ch < phs->num_channels; ch++) {
                num_pulses += phs->channels[ch].size;
        }
        return num_pulses;
}

void mliesExtractChannels_print(const struct mliesExtractChannels *phs)
{
        uint64_t ch, pu;
        printf("Start photon-stream\n");
        printf("    time_slice_duration: %Es\n", phs->time_slice_duration);
        printf("    num_time_slices: %ld\n", phs->num_time_slices);
        printf("    num_channels: %ld\n", phs->num_channels);
        printf("    num_pulses: %ld\n", mliesExtractChannels_num_pulses(phs));
        printf("    Channels:\n");
        for (ch = 0; ch < phs->num_channels; ch++) {
                printf("    % 6ld\n", ch);
                printf("        [");
                for (pu = 0; pu < phs->channels[ch].size; pu++) {
                        struct mliesExtract exa;
                        exa = mliesExtractChannels_at(phs, ch, pu);
                        printf(
                                "(%d, %d), ",
                                exa.arrival_time_slice,
                                exa.simulation_truth_id);
                }
                printf("]\n");
        }

        printf("END photon-stream\n");
}


#endif
