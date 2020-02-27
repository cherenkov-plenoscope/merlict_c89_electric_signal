
CASE("truncate_invalid_arrival_times") {
        struct mliMT19937 prng = mliMT19937_init(0);
        const double time_slice_duration = 500e-12;
        const double arrival_time_std = 0.0;
        uint64_t num_invalid_photons = 0u;
        uint64_t num_passing_photons = 0u;
        int64_t i;
        uint64_t p;

        struct mliesPulseVector pulse_pipeline = mliesPulseVector_init();
        struct mliesExtractedPulseVector extraction_pipeline;

        CHECK(mliesPulseVector_malloc(&pulse_pipeline, 0u));
        for (i = -1000; i < 1000; i++) {
                struct mliesPulse pulse;
                pulse.arrival_time = time_slice_duration*(double)i;
                pulse.simulation_truth_id = 0;
                CHECK(mliesPulseVector_push_back(&pulse_pipeline, pulse));
        }

        num_invalid_photons = 0u;
        for (p = 0; p < pulse_pipeline.vector.size; p++) {
                struct mliesPulse pulse = mliesPulseVector_at(
                        &pulse_pipeline,
                        p);
                int32_t slice = mli_near_int(
                    pulse.arrival_time/time_slice_duration);

                if (slice < 0 || slice >= MLIES_NUM_TIME_SLICES) {
                        num_invalid_photons++;
                }
        }

        CHECK(2000 - MLIES_NUM_TIME_SLICES == num_invalid_photons);

        extraction_pipeline = mliesExtractedPulseVector_init();
        CHECK(mliesExtractedPulseVector_malloc(&extraction_pipeline, 0u));
        CHECK(mlies_extract_pulses(
                &pulse_pipeline,
                &extraction_pipeline,
                time_slice_duration,
                MLIES_NUM_TIME_SLICES,
                arrival_time_std,
                &prng));

        CHECK(pulse_pipeline.vector.size != extraction_pipeline.vector.size);
        num_passing_photons = 0;
        for (p = 0; p < extraction_pipeline.vector.size; p++) {
                struct mliesExtractedPulse expulse;
                expulse = mliesExtractedPulseVector_at(
                    &extraction_pipeline,
                    p);
                CHECK(255 > expulse.arrival_time_slice);
                num_passing_photons++;
        }

        CHECK(MLIES_NUM_TIME_SLICES == num_passing_photons);
        mliesExtractedPulseVector_free(&extraction_pipeline);
        mliesPulseVector_free(&pulse_pipeline);
}

CASE("arrival_time_std") {
        struct mliMT19937 prng = mliMT19937_init(0);
        const double time_slice_duration = 500e-12;
        const double arrival_time_std = 5e-9;
        const double true_arrival_time = 25e-9;
        uint64_t i;
        uint64_t max_num_time_slices = 100;
        struct mliesPulseVector pulse_pipeline = mliesPulseVector_init();
        struct mliesExtractedPulseVector extraction_pipeline =
                mliesExtractedPulseVector_init();
        double *reconstructed_arrival_times = NULL;
        double reco_mean;
        double reco_std;
        uint64_t num_reco;
        CHECK(mliesPulseVector_malloc(&pulse_pipeline, 0u));

        for (i = 0; i < 10*1000; i++) {
            struct mliesPulse pulse;
            pulse.arrival_time = true_arrival_time;
            pulse.simulation_truth_id = i;
            CHECK(mliesPulseVector_push_back(&pulse_pipeline, pulse));
        }

        CHECK(mliesExtractedPulseVector_malloc(&extraction_pipeline, 0u));

        CHECK(mlies_extract_pulses(
                &pulse_pipeline,
                &extraction_pipeline,
                time_slice_duration,
                max_num_time_slices,
                arrival_time_std,
                &prng));

        num_reco = extraction_pipeline.vector.size;
        reconstructed_arrival_times = (double *)malloc(sizeof(double)*num_reco);
        CHECK(reconstructed_arrival_times);
        for (i = 0; i < num_reco; i++) {
                struct mliesExtractedPulse expulse;
                expulse = mliesExtractedPulseVector_at(&extraction_pipeline, i);
                reconstructed_arrival_times[i] = (
                    (double)expulse.arrival_time_slice*
                    time_slice_duration);
        }

        reco_mean = mli_mean(reconstructed_arrival_times, num_reco);
        reco_std = mli_std(reconstructed_arrival_times, num_reco, reco_mean);

        CHECK_MARGIN(arrival_time_std, reco_std, 1e-10);
        CHECK_MARGIN(true_arrival_time, reco_mean, 1e-10);

        mliesPulseVector_free(&pulse_pipeline);
        mliesExtractedPulseVector_free(&extraction_pipeline);
        free(reconstructed_arrival_times);
}
