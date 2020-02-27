/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIESEXTRACTEDPULSE_H_
#define MLIESEXTRACTEDPULSE_H_

#include <stdint.h>
#include "mlies_constants.h"

struct mliesExtractedPulse {
        uint8_t arrival_time_slice;
        int32_t simulation_truth_id;
};

struct mliesExtractedPulse mliesExtractedPulse_init() {
        struct mliesExtractedPulse out;
        out.simulation_truth_id = MLIES_DEFAULT_SIMULATION_TRUTH;
        out.arrival_time_slice = 0u;
        return out;
}

#endif
