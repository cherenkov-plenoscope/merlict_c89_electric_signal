/* Copyright 2020 Sebastian Achim Mueller */
#ifndef mliesExtract_H_
#define mliesExtract_H_

#include <stdint.h>
#include "mlies_constants.h"

struct mliesExtract {
        uint8_t arrival_time_slice;
        int32_t simulation_truth_id;
};

struct mliesExtract mliesExtract_init() {
        struct mliesExtract out;
        out.simulation_truth_id = MLIES_DEFAULT_SIMULATION_TRUTH;
        out.arrival_time_slice = 0u;
        return out;
}

#endif
