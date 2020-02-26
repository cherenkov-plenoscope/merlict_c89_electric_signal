/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISP_PHOTONSTREAM_H_
#define MLISP_PHOTONSTREAM_H_

#include <stdint.h>
#include "mlispExtractedPulseVector.h"
#include "mlisp_constants.h"

struct mlispPhotonStream {
        double time_slice_duration;
        uint64_t num_channels;
        uint64_t num_time_slices;
        struct mlispExtractedPulseVector *channels;
};

struct mlispPhotonStream mlispPhotonStream_init() {
        struct mlispPhotonStream phs;
        phs.time_slice_duration = .0;
        phs.num_time_slices = MLISP_NUM_TIME_SLICES;
        phs.num_channels = 0u;
        phs.channels = NULL;
        return phs;
}

void _mlispPhotonStream_free(struct mlispPhotonStream *phs) {
        if (phs->channels != NULL) {
                uint64_t ch;
                for (ch = 0u; ch < phs->num_channels; ch++) {
                        mlispExtractedPulseVector_free(&phs->channels[ch]);
                }
        }
        free(phs->channels);
}

void mlispPhotonStream_free(struct mlispPhotonStream *phs) {
        _mlispPhotonStream_free(phs);
        phs->num_channels = 0u;
}

int mlispPhotonStream_malloc(struct mlispPhotonStream *phs) {
        uint64_t ch = 0u;
        _mlispPhotonStream_free(phs);
        mli_malloc(
                phs->channels,
                struct mlispExtractedPulseVector,
                phs->num_channels);
        for (ch = 0; ch < phs->num_channels; ch++) {
                mlispExtractedPulseVector_malloc(&phs->channels[ch], 0u);
        }
        return 1;
    error:
        return 0;
}

uint64_t mlispPhotonStream_num_pulses(const struct mlispPhotonStream *phs) {
        uint64_t num_pulses = 0u;
        uint64_t ch = 0u;
        for (ch = 0; ch < phs->num_channels; ch++) {
                num_pulses += phs->channels[ch].vector.size;
        }
        return num_pulses;
}

#endif
