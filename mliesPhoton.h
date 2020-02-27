/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPHOTON_H_
#define MLIESPHOTON_H_

#include <stdint.h>
#include "mlies_constants.h"

struct mliesPhoton {
        double arrival_time;
        double wavelength;
        int32_t simulation_truth_id;
};

struct mliesPhoton mliesPhoton_init() {
        struct mliesPhoton pph;
        pph.simulation_truth_id = MLIES_DEFAULT_SIMULATION_TRUTH;
        pph.arrival_time = 0.;
        pph.wavelength = 0.;
        return pph;
}

#endif
