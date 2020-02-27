/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLI_SINGLE_PHOTON_SIGNAL_PROCESSING_H_
#define MLI_SINGLE_PHOTON_SIGNAL_PROCESSING_H_

#include <stdint.h>
#include <math.h>
#include "../merlict_c89/mliFunc.h"
#include "../merlict_c89/mliVector.h"
#include "../merlict_c89/mliMT19937.h"

#include "mlies_constants.h"

#include "mliesPhoton.h"
#include "mliesPhotonVector.h"

#include "mliesPulse.h"
#include "mliesPulseVector.h"

#include "mliesExtractedPulse.h"
#include "mliesExtractedPulseVector.h"

#include "mliesPhotonStream.h"
#include "mliesPhotonStream_io.h"
#include "mliesPhotonStream_test.h"

/*
 *  mliPhoton
 *
 *     |
 *     |        strip away support-vector, and direction-vector
 *     V
 *
 *  mliesPhoton
 *
 *     |        photo-electric-converter
 *     |        strip away wavelength
 *     V
 *
 *  mliesPulse
 *
 *     |
 *     |        assign to time-slice
 *     V
 *
 *  mliesExtractedPulse
 *
 */


int mlies_converter_add_incoming_pulse(
        struct mliesPulseVector *out_electric_pipeline,
        const struct mliesPulse incoming_pulse,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        mli_c(mliesPulseVector_push_back(
                out_electric_pipeline,
                incoming_pulse));
        if (converter_crosstalk_probability >= mliMT19937_uniform(prng)) {
                struct mliesPulse crosstalk_pulse;
                crosstalk_pulse.arrival_time = incoming_pulse.arrival_time;
                crosstalk_pulse.simulation_truth_id = MLIES_CONVERTER_CROSSTALK;
                mli_c(mlies_converter_add_incoming_pulse(
                        out_electric_pipeline,
                        crosstalk_pulse,
                        converter_crosstalk_probability,
                        prng));
        }
        return 1;
error:
        return 0;
}


int mlies_converter_add_internal_poisson_accidentals(
        struct mliesPulseVector *out_electric_pipeline,
        const double rate,
        const double exposure_time,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        double time_sum = mliMT19937_expovariate(prng, rate);
        while (time_sum < exposure_time) {
                const double time_to_next = mliMT19937_expovariate(prng, rate);
                struct mliesPulse pulse;
                pulse.arrival_time = time_sum;
                pulse.simulation_truth_id = MLIES_CONVERTER_ACCIDENTAL;
                mli_c(mlies_converter_add_incoming_pulse(
                        out_electric_pipeline,
                        pulse,
                        converter_crosstalk_probability,
                        prng));
                time_sum += time_to_next;
        }
        return 1;
error:
        return 0;
}


int mlies_photo_electric_convert(
        const struct mliesPhotonVector *photon_pipeline,
        struct mliesPulseVector *out_electric_pipeline,
        const double exposure_time,
        const struct mliFunc *converter_quantum_efficiency_vs_wavelength,
        const double converter_dark_rate,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        uint64_t i;
        for (i = 0; i < photon_pipeline->vector.size; i++) {

                struct mliesPhoton ph = mliesPhotonVector_at(
                        photon_pipeline,
                        i);

                double quantum_efficiency = MLI_NAN;
                mli_c(mliFunc_evaluate(
                        converter_quantum_efficiency_vs_wavelength,
                        ph.wavelength,
                        &quantum_efficiency));

                if (quantum_efficiency >= mliMT19937_uniform(prng)) {
                        struct mliesPulse pu = mliesPulse_from_photon(ph);
                        mli_c(mlies_converter_add_incoming_pulse(
                                out_electric_pipeline,
                                pu,
                                converter_crosstalk_probability,
                                prng));
                }
        }
        mli_c(mlies_converter_add_internal_poisson_accidentals(
                out_electric_pipeline,
                converter_dark_rate,
                exposure_time,
                converter_crosstalk_probability,
                prng));

        return 1;
error:
        return 0;
}


int mlies_extract_pulses(
        const struct mliesPulseVector *pulses,
        struct mliesExtractedPulseVector *out_extracted_pulses,
        const double time_slice_duration,
        const uint64_t max_num_time_slices,
        const double extractor_arrival_time_std,
        struct mliMT19937 *prng)
{
        uint64_t p;
        for (p = 0; p < pulses->vector.size; p++) {
                struct mliesPulse pulse = mliesPulseVector_at(pulses, p);

                const double reconstructed_arrival_time = (
                        pulse.arrival_time +
                        mliMT19937_normal_Irwin_Hall_approximation(
                                prng,
                                0.0f,
                                extractor_arrival_time_std));
                const int64_t slice = mli_near_int(
                        reconstructed_arrival_time/
                        time_slice_duration);

                if (slice >= 0 && slice < (int64_t)max_num_time_slices) {
                        struct mliesExtractedPulse extracted_pulse;
                        extracted_pulse.simulation_truth_id = pulse.
                                simulation_truth_id;
                        extracted_pulse.arrival_time_slice = (uint8_t)slice;
                        mli_c(mliesExtractedPulseVector_push_back(
                                out_extracted_pulses,
                                extracted_pulse));
                }
        }
        return 1;
error:
        return 0;
}

#endif
