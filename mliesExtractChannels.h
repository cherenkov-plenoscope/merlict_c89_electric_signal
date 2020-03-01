/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIES_EXTRACTCHANNELS_H_
#define MLIES_EXTRACTCHANNELS_H_

#include <stdint.h>
#include "../merlict_c89/mliDynArray_test.h"
#include "mliesExtract.h"
#include "mlies_constants.h"


MLIDYNARRAY_TEMPLATE(mlies, Extract, struct mliesExtract)


MLIDYNARRAY_2D_TEMPLATE(mlies, Extract, mliesDynExtract)


struct mliesPhotonStream {
        struct mliesExtractChannels stream;
        double time_slice_duration;
        uint64_t num_time_slices;
};

struct mliesPhotonStream mliesPhotonStream_init()
{
        struct mliesPhotonStream phs;
        phs.time_slice_duration = 0.0;
        phs.num_time_slices = 0;
        phs.stream = mliesExtractChannels_init();
        return phs;
}

void mliesPhotonStream_free(struct mliesPhotonStream *phs)
{
        mliesExtractChannels_free(&phs->stream);
}

int mliesPhotonStream_malloc(
        struct mliesPhotonStream *phs,
        const uint64_t num_channels)
{
        return mliesExtractChannels_malloc(&phs->stream, num_channels);
}

void mliesPhotonStream_print(const struct mliesPhotonStream *phs)
{
        uint64_t ch, pu;
        printf("Start photon-stream\n");
        printf("    time_slice_duration: %Es\n", phs->time_slice_duration);
        printf("    num_time_slices: %ld\n", phs->num_time_slices);
        printf("    num_channels: %ld\n", phs->stream.num_channels);
        printf("    num_pulses: %ld\n", mliesExtractChannels_total_num(
            &phs->stream));
        printf("    Channels:\n");
        for (ch = 0; ch < phs->stream.num_channels; ch++) {
                printf("    % 6ld\n", ch);
                printf("        [");
                for (pu = 0; pu < phs->stream.channels[ch].dyn.size; pu++) {
                        struct mliesExtract exa =
                            phs->stream.channels[ch].arr[pu];
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
