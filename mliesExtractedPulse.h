/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISPEXTRACTEDPULSE_H_
#define MLISPEXTRACTEDPULSE_H_

#include <stdint.h>
#include "mlisp_constants.h"

struct mlispExtractedPulse {
        uint8_t arrival_time_slice;
        int32_t simulation_truth_id;
};

struct mlispExtractedPulse mlispExtractedPulse_init() {
        struct mlispExtractedPulse out;
        out.simulation_truth_id = MLISP_DEFAULT_SIMULATION_TRUTH;
        out.arrival_time_slice = 0u;
        return out;
}

#endif
