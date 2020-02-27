/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIES_PHOTONSTREAM_H_
#define MLIES_PHOTONSTREAM_H_

#include <stdint.h>
#include "mliesExtractedPulseVector.h"
#include "mlies_constants.h"

struct mliesPhotonStream {
        double time_slice_duration;
        uint64_t num_channels;
        uint64_t num_time_slices;
        struct mliesExtractedPulseVector *channels;
};

struct mliesPhotonStream mliesPhotonStream_init() {
        struct mliesPhotonStream phs;
        phs.time_slice_duration = .0;
        phs.num_time_slices = MLIES_NUM_TIME_SLICES;
        phs.num_channels = 0u;
        phs.channels = NULL;
        return phs;
}

void _mliesPhotonStream_free(struct mliesPhotonStream *phs) {
        if (phs->channels != NULL) {
                uint64_t ch;
                for (ch = 0u; ch < phs->num_channels; ch++) {
                        mliesExtractedPulseVector_free(&phs->channels[ch]);
                }
        }
        free(phs->channels);
}

void mliesPhotonStream_free(struct mliesPhotonStream *phs) {
        _mliesPhotonStream_free(phs);
        phs->num_channels = 0u;
}

int mliesPhotonStream_malloc(struct mliesPhotonStream *phs) {
        uint64_t ch = 0u;
        _mliesPhotonStream_free(phs);
        mli_malloc(
                phs->channels,
                struct mliesExtractedPulseVector,
                phs->num_channels);
        for (ch = 0; ch < phs->num_channels; ch++) {
                mliesExtractedPulseVector_malloc(&phs->channels[ch], 0u);
        }
        return 1;
    error:
        return 0;
}

uint64_t mliesPhotonStream_num_pulses(const struct mliesPhotonStream *phs) {
        uint64_t num_pulses = 0u;
        uint64_t ch = 0u;
        for (ch = 0; ch < phs->num_channels; ch++) {
                num_pulses += phs->channels[ch].vector.size;
        }
        return num_pulses;
}

void mliesPhotonStream_print(const struct mliesPhotonStream *phs)
{
        uint64_t ch, pu;
        printf("Start photon-stream\n");
        printf("    time_slice_duration: %Es\n", phs->time_slice_duration);
        printf("    num_time_slices: %ld\n", phs->num_time_slices);
        printf("    num_channels: %ld\n", phs->num_channels);
        printf("    num_pulses: %ld\n", mliesPhotonStream_num_pulses(phs));
        printf("    Channels:\n");
        for (ch = 0; ch < phs->num_channels; ch++) {
                printf("    % 6ld\n", ch);
                printf("        [");
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mliesExtractedPulse exa;
                        exa = mliesExtractedPulseVector_at(
                                &phs->channels[ch],
                                pu);
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
