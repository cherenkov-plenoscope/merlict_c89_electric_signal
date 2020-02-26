/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPPULSE_H_
#define MLISPPULSE_H_

#include <stdint.h>
#include "mlisp_constants.h"

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

#endif
