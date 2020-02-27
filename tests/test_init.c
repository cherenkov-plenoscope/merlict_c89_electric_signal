/* Copyright 2019-2020 Sebastian Achim Mueller                                */

CASE("init PipelinePhoton") {
        struct mliesPhoton piph = mliesPhoton_init();
        CHECK(piph.simulation_truth_id == MLIES_DEFAULT_SIMULATION_TRUTH);
        CHECK(piph.arrival_time == 0.);
        CHECK(piph.wavelength == 0.);
}

CASE("init ElectricPulse") {
        struct mliesPulse elpu = mliesPulse_init();
        CHECK(elpu.simulation_truth_id == MLIES_DEFAULT_SIMULATION_TRUTH);
        CHECK(elpu.arrival_time == 0.);
}

CASE("init ExtractedPulse") {
        struct mliesExtractedPulse expu = mliesExtractedPulse_init();
        CHECK(expu.simulation_truth_id == MLIES_DEFAULT_SIMULATION_TRUTH);
        CHECK(expu.arrival_time_slice == 0u);
}
