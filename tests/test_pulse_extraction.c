
CASE("truncate_invalid_arrival_times") {
        struct mliMT19937 prng = mliMT19937_init(0);
        const double time_slice_duration = 500e-12;
        const double arrival_time_std = 0.0;
        uint64_t num_invalid_photons = 0u;
        uint64_t num_passing_photons = 0u;
        int64_t i;
        uint64_t p;

        struct mliesPulseChannels pulses = mliesPulseChannels_init();
        struct mliesExtractChannels extracts = mliesExtractChannels_init();

        CHECK(mliesPulseChannels_malloc(&pulses, 1u));
        for (i = -1000; i < 1000; i++) {
                struct mliesPulse pulse;
                pulse.arrival_time = time_slice_duration*(double)i;
                pulse.simulation_truth_id = 0;
                CHECK(mliesDynPulse_push_back(&pulses.channels[0], pulse));
        }

        num_invalid_photons = 0u;
        for (p = 0; p < pulses.channels[0].dyn.size; p++) {
                int32_t slice = mli_near_int(
                    pulses.channels[0].arr[p].arrival_time/
                    time_slice_duration);
                if (slice < 0 || slice >= MLIES_NUM_TIME_SLICES) {
                        num_invalid_photons++;
                }
        }

        CHECK(2000 - MLIES_NUM_TIME_SLICES == num_invalid_photons);

        CHECK(mliesExtractChannels_malloc(&extracts, 1u));
        CHECK(mlies_extract_pulses(
                &pulses.channels[0],
                &extracts.channels[0],
                time_slice_duration,
                MLIES_NUM_TIME_SLICES,
                arrival_time_std,
                &prng));

        CHECK(pulses.channels[0].dyn.size != extracts.channels[0].dyn.size);
        num_passing_photons = 0;
        for (p = 0; p < extracts.channels[0].dyn.size; p++) {
                struct mliesExtract expulse = extracts.channels[0].arr[p];
                CHECK(255 > expulse.arrival_time_slice);
                num_passing_photons++;
        }

        CHECK(MLIES_NUM_TIME_SLICES == num_passing_photons);
        mliesExtractChannels_free(&extracts);
        mliesPulseChannels_free(&pulses);
}

CASE("arrival_time_std") {
        struct mliMT19937 prng = mliMT19937_init(0);
        const double time_slice_duration = 500e-12;
        const double arrival_time_std = 5e-9;
        const double true_arrival_time = 25e-9;
        uint64_t i;
        uint64_t max_num_time_slices = 100;
        struct mliesPulseChannels pulses = mliesPulseChannels_init();
        struct mliesExtractChannels extracts = mliesExtractChannels_init();
        double *reconstructed_arrival_times = NULL;
        double reco_mean;
        double reco_std;
        uint64_t num_reco;
        CHECK(mliesPulseChannels_malloc(&pulses, 1u));

        for (i = 0; i < 10*1000; i++) {
            struct mliesPulse pulse;
            pulse.arrival_time = true_arrival_time;
            pulse.simulation_truth_id = i;
            CHECK(mliesDynPulse_push_back(&pulses.channels[0], pulse));
        }

        CHECK(mliesExtractChannels_malloc(&extracts, 1u));

        CHECK(mlies_extract_pulses(
                &pulses.channels[0],
                &extracts.channels[0],
                time_slice_duration,
                max_num_time_slices,
                arrival_time_std,
                &prng));

        num_reco = extracts.channels[0].dyn.size;
        reconstructed_arrival_times = (double *)malloc(sizeof(double)*num_reco);
        CHECK(reconstructed_arrival_times);
        for (i = 0; i < num_reco; i++) {
                struct mliesExtract expulse = extracts.channels[0].arr[i];
                reconstructed_arrival_times[i] = (
                    (double)expulse.arrival_time_slice*
                    time_slice_duration);
        }

        reco_mean = mli_mean(reconstructed_arrival_times, num_reco);
        reco_std = mli_std(reconstructed_arrival_times, num_reco, reco_mean);

        CHECK_MARGIN(arrival_time_std, reco_std, 1e-10);
        CHECK_MARGIN(true_arrival_time, reco_mean, 1e-10);

        mliesPulseChannels_free(&pulses);
        mliesExtractChannels_free(&extracts);
        free(reconstructed_arrival_times);
}
