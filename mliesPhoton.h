/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPPHOTON_H_
#define MLISPPHOTON_H_

#include <stdint.h>
#include "mlisp_constants.h"

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

#endif
