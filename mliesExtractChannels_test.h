/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESEXTRACTCHANNELS_TEST_H_
#define MLIESEXTRACTCHANNELS_TEST_H_

#include "mliesExtractChannels.h"
#include "mliesExtractChannels_io.h"


int mlies_test_ExtractChannels_is_equal_verbose(
        const struct mliesExtractChannels *a,
        const struct mliesExtractChannels *b)
{
        uint64_t ch, pu;
        mli_check(a->time_slice_duration == b->time_slice_duration,
                "time_slice_duration not equal");
        mli_check(a->num_time_slices == b->num_time_slices,
                "num_time_slices not equal");
        mli_check(a->num_channels == b->num_channels,
                "num_channels not equal");
        for (ch = 0; ch < a->num_channels; ch++) {
                mli_check(a->channels[ch].size == b->channels[ch].size,
                        "num_channels not equal");
                for (pu = 0; pu < a->channels[ch].size; pu++) {
                        struct mliesExtract exa, exb;
                        exa = mliesExtractChannels_at(a, ch, pu);
                        exb = mliesExtractChannels_at(b, ch, pu);
                        mli_check(
                                exa.arrival_time_slice ==
                                exb.arrival_time_slice,
                                "arrival_time_slice not equal");
                        mli_check(
                                exa.simulation_truth_id ==
                                exb.simulation_truth_id,
                                "simulation_truth_id not equal");
                }
        }
        return 1;
    error:
        return 0;
}

int mlies_test_ExtractChannels_expose_poisson(
        struct mliesExtractChannels *phs,
        const double rate,
        const double exposure_time,
        struct mliMT19937 *prng)
{
        uint64_t ch;
        for (ch = 0; ch < phs->num_channels; ch++) {
                double t = 0.0;
                while (1) {
                        const double t_next = mliMT19937_expovariate(
                                prng,
                                rate);
                        if (t + t_next > exposure_time) {
                                break;
                        } else {
                                struct mliesExtract expulse;
                                t += t_next;
                                expulse.arrival_time_slice = (int32_t)
                                        floor(t/phs->time_slice_duration);
                                expulse.simulation_truth_id = (int32_t)
                                         1000*mliMT19937_uniform(prng);
                                mli_c(mliesExtractChannels_push_back(
                                        phs, ch, expulse));
                        }
                }
        }
        return 1;
    error:
        return 0;
}

int mlies_test_ExtractChannels_expose_write_and_read_back(
        const uint64_t num_channels,
        const uint64_t num_time_slices,
        const double time_slice_duration,
        const double single_pulse_rate,
        const double exposure_time,
        struct mliMT19937 *prng)
{
        struct mliesExtractChannels phs = mliesExtractChannels_init();
        struct mliesExtractChannels phs_back = mliesExtractChannels_init();
        phs.time_slice_duration = time_slice_duration;
        phs.num_time_slices = num_time_slices;
        mli_c(mliesExtractChannels_malloc(&phs, num_channels));
        mli_c(mlies_test_ExtractChannels_expose_poisson(
                &phs,
                single_pulse_rate,
                exposure_time,
                prng));

        mli_c(mliesExtractChannels_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "photon_stream.phs.tmp",
                "tests/"
                "resources/"
                "photon_stream.phs.truth.tmp"));

        mli_c(mliesExtractChannels_malloc_from_path(
                &phs_back,
                "tests/"
                "resources/"
                "photon_stream.phs.tmp",
                "tests/"
                "resources/"
                "photon_stream.phs.truth.tmp"));

        mli_c(mlies_test_ExtractChannels_is_equal_verbose(&phs, &phs_back));

        mliesExtractChannels_free(&phs);
        mliesExtractChannels_free(&phs_back);

        return 1;
    error:
        return 0;
}

#endif
