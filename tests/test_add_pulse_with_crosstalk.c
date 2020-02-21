/* Copyright 2019-2020 Sebastian Achim Mueller                                */

CASE("add pulse to electric pipeline with crosstalk") {
        const uint64_t num_initial_pulses = 100;
        const uint64_t num_additional_pulses = 1000u*1000u;
        const uint64_t N = num_additional_pulses;
        const double probability_for_second_puls = 0.1;
        const double p = probability_for_second_puls;

        uint64_t i;
        uint64_t num_crosstalk_expected = 0;
        uint64_t num_crosstalk = 0;
        struct mliMT19937 prng;
        struct mliVector electric_pipeline = mliVector_init();

        mliMT19937_init(&prng, 0u);
        CHECK(mliVector_malloc(
                &electric_pipeline,
                0u,
                sizeof(struct mlispPulse)));

        /* Add initial pulses, so the pipeline is not empty on start */
        for (i = 0; i < num_initial_pulses; i++) {
                struct mlispPulse initial_pulse;
                initial_pulse.arrival_time = 1.;
                initial_pulse.simulation_truth_id = -1;
                CHECK(mliVector_push_back(&electric_pipeline, &initial_pulse));
        }
        CHECK(electric_pipeline.size == num_initial_pulses);

        /* Add pulses which might trigger cross-talk */
        for (i = 0; i < N; i++) {
                struct mlispPulse epulse;
                epulse.arrival_time = 1.;
                epulse.simulation_truth_id = (int32_t)i;
                CHECK(mlisp_converter_add_incoming_pulse(
                        &electric_pipeline,
                        epulse,
                        probability_for_second_puls,
                        &prng));
        }

        num_crosstalk_expected = N*p + ((N*p)*p) + (((N*p)*p)*p);
        /* And so on ... */

        num_crosstalk = 0;
        for (i = 0; i < electric_pipeline.size; i++) {
                struct mlispPulse
                ep = *(struct mlispPulse*)mliVector_at(
                        &electric_pipeline,
                        i);
                if (ep.simulation_truth_id == MLISP_CONVERTER_CROSSTALK) {
                        num_crosstalk++;
                }
        }

        CHECK(
                abs(num_crosstalk - num_crosstalk_expected) <
                0.01*num_crosstalk_expected);
        CHECK(
                electric_pipeline.size - num_crosstalk ==
                num_additional_pulses + num_initial_pulses);

        mliVector_free(&electric_pipeline);
}
