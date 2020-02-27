/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESPULSE_H_
#define MLIESPULSE_H_

#include <stdint.h>
#include "mlies_constants.h"

struct mliesPulse {
        double arrival_time;
        int32_t simulation_truth_id;
};

struct mliesPulse mliesPulse_init() {
        struct mliesPulse out;
        out.simulation_truth_id = MLIES_DEFAULT_SIMULATION_TRUTH;
        out.arrival_time = 0.;
        return out;
}

struct mliesPulse mliesPulse_from_photon(const struct mliesPhoton ph) {
        struct mliesPulse out;
        out.simulation_truth_id = ph.simulation_truth_id;
        out.arrival_time = ph.arrival_time;
        return out;
}

#endif
