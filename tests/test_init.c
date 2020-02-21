/* Copyright 2019-2020 Sebastian Achim Mueller                                */

CASE("init PipelinePhoton") {
        struct mlispPhoton piph = mlispPhoton_init();
        CHECK(piph.simulation_truth_id == MLISP_DEFAULT_SIMULATION_TRUTH);
        CHECK(piph.arrival_time == 0.);
        CHECK(piph.wavelength == 0.);
}

CASE("init ElectricPulse") {
        struct mlispPulse elpu = mlispPulse_init();
        CHECK(elpu.simulation_truth_id == MLISP_DEFAULT_SIMULATION_TRUTH);
        CHECK(elpu.arrival_time == 0.);
}

CASE("init ExtractedPulse") {
        struct mlispExtractedPulse expu = mlispsExtractedPulse_init();
        CHECK(expu.simulation_truth_id == MLISP_DEFAULT_SIMULATION_TRUTH);
        CHECK(expu.arrival_time_slice == 0u);
}
