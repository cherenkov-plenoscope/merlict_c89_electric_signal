/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPHOTONSTREAM_TEST_H_
#define MLIESPHOTONSTREAM_TEST_H_

#include "mliesPhotonStream.h"
#include "mliesPhotonStream_io.h"


int mlies_test_PhotonStream_is_equal_verbose(
        const struct mliesPhotonStream *a,
        const struct mliesPhotonStream *b)
{
        uint64_t ch, pu;
        mli_check(a->time_slice_duration == b->time_slice_duration,
                "time_slice_duration not equal");
        mli_check(a->num_time_slices == b->num_time_slices,
                "num_time_slices not equal");
        mli_check(a->num_channels == b->num_channels,
                "num_channels not equal");
        for (ch = 0; ch < a->num_channels; ch++) {
                mli_check(
                        a->channels[ch].vector.size ==
                        b->channels[ch].vector.size,
                        "num_channels not equal");
                for (pu = 0; pu < a->channels[ch].vector.size; pu++) {
                        struct mliesExtractedPulse exa, exb;
                        exa = mliesExtractedPulseVector_at(
                                &a->channels[ch],
                                pu);
                        exb = mliesExtractedPulseVector_at(
                                &b->channels[ch],
                                pu);
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

int mlies_test_PhotonStream_expose_poisson(
        struct mliesPhotonStream *phs,
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
                                struct mliesExtractedPulse expulse;
                                t += t_next;
                                expulse.arrival_time_slice = (int32_t)
                                        floor(t/phs->time_slice_duration);
                                expulse.simulation_truth_id = (int32_t)
                                         1000*mliMT19937_uniform(prng);
                                mli_c(mliesExtractedPulseVector_push_back(
                                        &phs->channels[ch],
                                        expulse));
                        }
                }
        }
        return 1;
    error:
        return 0;
}

int mlies_test_PhotonStream_expose_write_and_read_back(
        const uint64_t num_channels,
        const uint64_t num_time_slices,
        const double time_slice_duration,
        const double single_pulse_rate,
        const double exposure_time,
        struct mliMT19937 *prng)
{
        struct mliesPhotonStream phs = mliesPhotonStream_init();
        struct mliesPhotonStream phs_back = mliesPhotonStream_init();
        phs.num_channels = num_channels;
        phs.time_slice_duration = time_slice_duration;
        phs.num_time_slices = num_time_slices;
        mli_c(mliesPhotonStream_malloc(&phs));
        mli_c(mlies_test_PhotonStream_expose_poisson(
                &phs,
                single_pulse_rate,
                exposure_time,
                prng));

        mli_c(mliesPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "photon_stream.phs.tmp",
                "tests/"
                "resources/"
                "photon_stream.phs.truth.tmp"));

        mli_c(mliesPhotonStream_malloc_from_path(
                &phs_back,
                "tests/"
                "resources/"
                "photon_stream.phs.tmp",
                "tests/"
                "resources/"
                "photon_stream.phs.truth.tmp"));

        mli_c(mlies_test_PhotonStream_is_equal_verbose(&phs, &phs_back));

        mliesPhotonStream_free(&phs);
        mliesPhotonStream_free(&phs_back);

        return 1;
    error:
        return 0;
}

#endif
