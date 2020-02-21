/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLI_SINGLE_PHOTON_SIGNAL_PROCESSING_H_
#define MLI_SINGLE_PHOTON_SIGNAL_PROCESSING_H_

#include <stdint.h>
#include "../merlict_c89/mliFunc.h"
#include "../merlict_c89/mliVector.h"
#include "../merlict_c89/mliMT19937.h"


#define MLISP_DEFAULT_SIMULATION_TRUTH -1
#define MLISP_NIGHT_SKY_BACKGROUND -100
#define MLISP_CONVERTER_ACCIDENTAL -201
#define MLISP_CONVERTER_CROSSTALK -202


struct mlispPhoton {
        double arrival_time;
        double wavelength;
        int32_t simulation_truth_id;
};

struct mlispPhoton mlispPhoton_init() {
        struct mlispPhoton pph;
        pph.simulation_truth_id = MLISP_DEFAULT_SIMULATION_TRUTH;
        pph.arrival_time = 0.;
        pph.wavelength = 0.;
        return pph;
}

struct mlispPulse {
        double arrival_time;
        int32_t simulation_truth_id;
};

struct mlispPulse mlispPulse_init() {
        struct mlispPulse out;
        out.simulation_truth_id = MLISP_DEFAULT_SIMULATION_TRUTH;
        out.arrival_time = 0.;
        return out;
}

struct mlispPulse mlispPulse_from_photon(const struct mlispPhoton ph) {
        struct mlispPulse out;
        out.simulation_truth_id = ph.simulation_truth_id;
        out.arrival_time = ph.arrival_time;
        return out;
}

struct mlispExtractedPulse {
        uint8_t arrival_time_slice;
        int32_t simulation_truth_id;
};

struct mlispExtractedPulse mlispsExtractedPulse_init() {
        struct mlispExtractedPulse out;
        out.simulation_truth_id = MLISP_DEFAULT_SIMULATION_TRUTH;
        out.arrival_time_slice = 0u;
        return out;
}

struct mlispPhotElecConvConf {
        struct mliFunc quantum_efficiency_vs_wavelength;
        double dark_rate;
        double probability_for_second_pulse;
};

struct mlispPhotElecConvConf mlispPhotElecConvConf_init() {
        struct mlispPhotElecConvConf out;
        out.dark_rate = 0.;
        out.probability_for_second_pulse = 0.;
        out.quantum_efficiency_vs_wavelength = mliFunc_init();
        return out;
}

int mlisp_photon_pipeline_to_pulse_pipeline(
        const struct mliVector *photon_pipeline,
        const double exposure_time,
        struct mliMT19937 *prng,
        struct mliVector *out_electric_pipeline)
{
        return 1;
}


int mlisp_converter_add_incoming_pulse(
        struct mliVector *out_electric_pipeline,
        const struct mlispPulse incoming_pulse,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        mli_c(mliVector_push_back(out_electric_pipeline, &incoming_pulse));
        if (converter_crosstalk_probability >= mliMT19937_uniform(prng)) {
                struct mlispPulse crosstalk_pulse;
                crosstalk_pulse.arrival_time = incoming_pulse.arrival_time;
                crosstalk_pulse.simulation_truth_id = MLISP_CONVERTER_CROSSTALK;
                mli_c(mlisp_converter_add_incoming_pulse(
                        out_electric_pipeline,
                        crosstalk_pulse,
                        converter_crosstalk_probability,
                        prng));
        }
        return 1;
error:
        return 0;
}


int mlisp_converter_add_internal_poisson_accidentals(
        struct mliVector *out_electric_pipeline,
        const double rate,
        const double exposure_time,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        double time_sum = mliMT19937_expovariate(prng, rate);
        while (time_sum < exposure_time) {
                const double time_to_next = mliMT19937_expovariate(prng, rate);
                struct mlispPulse pulse;
                pulse.arrival_time = time_sum;
                pulse.simulation_truth_id = MLISP_CONVERTER_ACCIDENTAL;
                mli_c(mlisp_converter_add_incoming_pulse(
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


int mlisp_photo_electric_convert(
        const struct mliVector *photon_pipeline,
        struct mliVector *out_electric_pipeline,
        const double exposure_time,
        const struct mliFunc *converter_quantum_efficiency_vs_wavelength,
        const double converter_dark_rate,
        const double converter_crosstalk_probability,
        struct mliMT19937 *prng)
{
        uint64_t i;
        for (i = 0; i < photon_pipeline->size; i++) {

                struct mlispPhoton ph =
                *(struct mlispPhoton *)mliVector_at(photon_pipeline, i);

                double quantum_efficiency = MLI_NAN;
                mli_c(mliFunc_evaluate(
                        converter_quantum_efficiency_vs_wavelength,
                        ph.wavelength,
                        &quantum_efficiency));

                if (quantum_efficiency >= mliMT19937_uniform(prng)) {
                        struct mlispPulse pu = mlispPulse_from_photon(ph);
                        mli_c(mlisp_converter_add_incoming_pulse(
                                out_electric_pipeline,
                                pu,
                                converter_crosstalk_probability,
                                prng));
                }
        }
        mli_c(mlisp_converter_add_internal_poisson_accidentals(
                out_electric_pipeline,
                converter_dark_rate,
                exposure_time,
                converter_crosstalk_probability,
                prng));

        return 1;
error:
        return 0;
}

#endif
