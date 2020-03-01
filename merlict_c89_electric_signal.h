/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLI_SINGLE_PHOTON_SIGNAL_PROCESSING_H_
#define MLI_SINGLE_PHOTON_SIGNAL_PROCESSING_H_

#include <stdint.h>
#include <math.h>
#include "../merlict_c89/mliFunc.h"
#include "../merlict_c89/mliMT19937.h"

#include "mlies_constants.h"

#include "mliesPhoton.h"
#include "mliesPhotonChannels.h"

#include "mliesPulse.h"
#include "mliesPulseChannels.h"

#include "mliesExtract.h"
#include "mliesExtractChannels.h"
#include "mliesExtractChannels_io.h"
#include "mliesExtractChannels_test.h"

/*
 *  CORSIKA-Cherenkov-photon
 *
 *     |
 *     |        adopt geometry, arrival-time, and wavelength
 *     V
 *
 *  mliPhoton
 *
 *     |
 *     |        Propagate photon in scenery. Bounce around, and get absorbed.
 *     V
 *
 *  mliPhotonHistory
 *
 *     |
 *     |        strip away support-vector, and direction-vector
 *     V
 *
 *  mliesPhoton
 *
 *     |        photo-electric-converter
 *     |        strip away wavelength
       |                 quantum-detection-efficiency vs. wavelength
 *     V
 *
 *  mliesPulse
 *
 *     |
 *     |        assign to time-slice
 *     V
 *
 *  mliesExtract
 *
 *     |
 *     |        write photon-stream-format
 *     V
 *
 *   write FILE
 *
 */


int mlies_converter_add_incoming_pulse(
        struct mliesDynPulse *out_electric_pipeline,
        const struct mliesPulse incoming_pulse,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        mli_c(mliesDynPulse_push_back(out_electric_pipeline, incoming_pulse));
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
        struct mliesDynPulse *out_electric_pipeline,
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

struct mliesConverter {
        double dark_rate;
        double crosstalk_probability;
        const struct mliFunc *quantum_efficiency_vs_wavelength;
};

int mlies_photo_electric_convert(
        const struct mliesDynPhoton *photon_pipeline,
        struct mliesDynPulse *out_electric_pipeline,
        const double exposure_time,
        const struct mliFunc *converter_quantum_efficiency_vs_wavelength,
        const double converter_dark_rate,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        uint64_t i;
        for (i = 0; i < photon_pipeline->dyn.size; i++) {
                struct mliesPhoton ph = photon_pipeline->arr[i];
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
        const struct mliesDynPulse *pulse_channel,
        struct mliesDynExtract *out_extracted_pulse_channel,
        const double time_slice_duration,
        const uint64_t max_num_time_slices,
        const double extractor_arrival_time_std,
        struct mliMT19937 *prng)
{
        const double zero_arrival_time_shift = 0.0;
        uint64_t p;
        for (p = 0; p < pulse_channel->dyn.size; p++) {
                struct mliesPulse pulse = pulse_channel->arr[p];

                const double reconstructed_arrival_time = (
                        pulse.arrival_time +
                        mliMT19937_normal_Irwin_Hall_approximation(
                                prng,
                                zero_arrival_time_shift,
                                extractor_arrival_time_std));
                const int64_t slice = mli_near_int(
                        reconstructed_arrival_time/
                        time_slice_duration);

                if (slice >= 0 && slice < (int64_t)max_num_time_slices) {
                        struct mliesExtract extracted_pulse;
                        extracted_pulse.simulation_truth_id = pulse.
                                simulation_truth_id;
                        extracted_pulse.arrival_time_slice = (uint8_t)slice;
                        mli_c(mliesDynExtract_push_back(
                                out_extracted_pulse_channel,
                                extracted_pulse));
                }
        }
        return 1;
error:
        return 0;
}

int mlies_append_equi_distant_photons(
        struct mliesDynPhoton *photon_channel,
        const double wavelength,
        const double start_time,
        const double stop_time,
        const uint64_t num_photons)
{
        uint64_t i;
        const double range = stop_time - start_time;
        const double step = range/(double)(num_photons - 1u);
        mli_check(range > 0., "Expect start before stop");
        for (i = 0; i < num_photons; i++) {
                struct mliesPhoton ph;
                ph.arrival_time = (double)i*step;
                ph.wavelength = 433e-9;
                ph.simulation_truth_id = i;
                mli_c(mliesDynPhoton_push_back(photon_channel, ph));
        }
        return 1;
error:
        return 0;
}

#endif
