/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPPHOTONSTREAM_TEST_H_
#define MLISPPHOTONSTREAM_TEST_H_

#include "mlispPhotonStream.h"
#include "mlispPhotonStream_io.h"

int mlisp_test_PhotonStream_is_equal(
        const struct mlispPhotonStream *a,
        const struct mlispPhotonStream *b,
        const int check_simulation_truth)
{
        uint64_t ch, pu;
        if (a->time_slice_duration != b->time_slice_duration)
                return -1;
        if (a->num_time_slices != b->num_time_slices)
                return -1;
        if (a->num_channels != b->num_channels)
                return -1;
        for (ch = 0; ch < a->num_channels; ch++) {
                if (a->channels[ch].vector.size != b->channels[ch].vector.size)
                        return -1;
                for (pu = 0; pu < a->channels[ch].vector.size; pu++) {
                        struct mlispExtractedPulse exa, exb;
                        exa = mlispExtractedPulseVector_at(
                                &a->channels[ch],
                                pu);
                        exb = mlispExtractedPulseVector_at(
                                &b->channels[ch],
                                pu);
                        if (exa.arrival_time_slice != exb.arrival_time_slice)
                                return -1;
                        if (check_simulation_truth) {
                                if (
                                        exa.simulation_truth_id !=
                                        exb.simulation_truth_id)
                                {
                                        return -1;
                                }
                        }
                }
        }
        return 1;
}

int mlisp_test_PhotonStream_expose_poisson(
        struct mlispPhotonStream *phs,
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
                                struct mlispExtractedPulse expulse;
                                t += t_next;
                                expulse.arrival_time_slice = (int32_t)
                                        floor(t/phs->time_slice_duration);
                                expulse.simulation_truth_id = (int32_t)
                                         1000*mliMT19937_uniform(prng);
                                mli_c(mlispExtractedPulseVector_push_back(
                                        &phs->channels[ch],
                                        expulse));
                        }
                }
        }
        return 1;
    error:
        return 0;
}

int mlisp_test_PhotonStream_expose_write_and_read_back(
        const uint64_t num_channels,
        const uint64_t num_time_slices,
        const double time_slice_duration,
        const double single_pulse_rate,
        const double exposure_time,
        struct mliMT19937 *prng)
{
        struct mlispPhotonStream phs = mlispPhotonStream_init();
        struct mlispPhotonStream phs_back = mlispPhotonStream_init();
        phs.num_channels = num_channels;
        phs.time_slice_duration = time_slice_duration;
        phs.num_time_slices = num_time_slices;
        mli_c(mlispPhotonStream_malloc(&phs));
        mli_c(mlisp_test_PhotonStream_expose_poisson(
                &phs,
                single_pulse_rate,
                exposure_time,
                prng));

        mli_c(mlispPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "photon_stream.phs.tmp",
                "tests/"
                "resources/"
                "photon_stream.phs.truth.tmp"));

        mli_c(mlispPhotonStream_malloc_from_path(
                &phs_back,
                "tests/"
                "resources/"
                "photon_stream.phs.tmp",
                "tests/"
                "resources/"
                "photon_stream.phs.truth.tmp"));

        mli_c(mlisp_test_PhotonStream_is_equal(&phs, &phs_back, 1));

        return 1;
    error:
        return 0;
}

#endif
