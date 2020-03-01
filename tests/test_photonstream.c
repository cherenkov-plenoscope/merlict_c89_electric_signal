/* Copyright 2019-2020 Sebastian Achim Mueller                                */

CASE("init ExtractChannels") {
        struct mliesPhotonStream phs = mliesPhotonStream_init();
        CHECK(phs.num_time_slices == 0);
        CHECK(phs.time_slice_duration == 0.);
        CHECK(phs.stream.num_channels == 0u);
        CHECK(phs.stream.channels == NULL);
}

CASE("malloc ExtractChannels") {
        struct mliesPhotonStream phs = mliesPhotonStream_init();
        CHECK(mliesPhotonStream_malloc(&phs, 12u));
        CHECK(phs.stream.num_channels == 12u);
        mliesPhotonStream_free(&phs);
        CHECK(phs.stream.num_channels == 0u);
}

CASE("num_pulses ExtractChannels") {
        uint64_t num_channel_scenarios = 7;
        uint64_t channel_scenarios[7] = {0, 1, 10, 100, 3, 4, 5};
        uint64_t chsc = 0;

        uint64_t num_pulse_scenarios = 9;
        uint64_t pulse_scenarios[9] = {0, 1, 10, 50, 0, 3, 64, 12, 1337};
        uint64_t pusc = 0;

        for (
                chsc = 0;
                chsc < num_channel_scenarios;
                chsc++)
        {
                for (
                        pusc = 0;
                        pusc < num_pulse_scenarios;
                        pusc++)
                {
                        uint64_t p = 0;
                        uint64_t c = 0;
                        uint64_t expected_num_pulses =
                                channel_scenarios[chsc]*
                                pulse_scenarios[pusc];

                        struct mliesExtract ex;
                        struct mliesPhotonStream phs = mliesPhotonStream_init();

                        CHECK(mliesPhotonStream_malloc(
                                &phs,
                                channel_scenarios[chsc]));
                        CHECK(
                                phs.stream.num_channels ==
                                channel_scenarios[chsc]);

                        for (c = 0; c < phs.stream.num_channels; c++) {
                                for (p = 0; p < pulse_scenarios[pusc]; p++) {
                                        ex.simulation_truth_id = p + 1 + c;
                                        ex.arrival_time_slice = p + c;
                                        CHECK(mliesDynExtract_push_back(
                                                &phs.stream.channels[c],
                                                ex));
                                }
                        }

                        CHECK(
                                mliesExtractChannels_total_num(&phs.stream) ==
                                expected_num_pulses);
                        mliesPhotonStream_free(&phs);
                }
        }
}

CASE("arrival_time_slices_below_next_channel_marker") {
        struct mliesPhotonStream phs = mliesPhotonStream_init();
        struct mliesPhotonStream phs_back = mliesPhotonStream_init();
        struct mliesExtract expulse = mliesExtract_init();
        struct mliesExtract expulse_back = mliesExtract_init();
        const uint64_t ch = 0;

        phs.num_time_slices = MLIES_NEXT_CHANNEL_MARKER;
        phs.time_slice_duration = 500e-12;
        CHECK(mliesPhotonStream_malloc(&phs, 1u));

        expulse.arrival_time_slice = 254u;
        expulse.simulation_truth_id = 0;

        CHECK(mliesDynExtract_push_back(&phs.stream.channels[ch], expulse));

        CHECK(mliesPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.tmp",
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.truth.tmp"));

        CHECK(mliesPhotonStream_malloc_from_path(
                &phs_back,
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.tmp",
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.truth.tmp"));

        CHECK(phs_back.stream.num_channels == phs.stream.num_channels);
        expulse_back = phs_back.stream.channels[ch].arr[0];

        CHECK(expulse_back.arrival_time_slice == expulse.arrival_time_slice);
        CHECK(expulse_back.simulation_truth_id == expulse.simulation_truth_id);

        CHECK(mliesPhotonStream_is_equal_verbose(&phs, &phs_back));

        mliesPhotonStream_free(&phs);
        mliesPhotonStream_free(&phs_back);
}

CASE("arrival_slices_must_not_be_NEXT_CHANNEL_MARKER") {
        struct mliesPhotonStream phs = mliesPhotonStream_init();
        struct mliesExtract expulse = mliesExtract_init();
        const int32_t simulation_truth_id = 1337;
        const uint64_t ch = 0;
        int64_t rc;
        phs.num_time_slices = MLIES_NEXT_CHANNEL_MARKER;
        phs.time_slice_duration = 500e-12;
        CHECK(mliesPhotonStream_malloc(&phs, 1u));

        expulse.simulation_truth_id = simulation_truth_id;
        expulse.arrival_time_slice = MLIES_NEXT_CHANNEL_MARKER;
        CHECK(mliesDynExtract_push_back(&phs.stream.channels[ch], expulse));

        rc = mliesPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "must_fail.phs.tmp",
                "tests/"
                "resources/"
                "must_fail.phs.truth.tmp");
        CHECK(rc < 1);

        expulse.arrival_time_slice = MLIES_NEXT_CHANNEL_MARKER - 1;
        phs.stream.channels[ch].arr[0] = expulse;

        rc = mliesPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "fine.phs.tmp",
                "tests/"
                "resources/"
                "fine.phs.truth.tmp");
        CHECK(rc);

        mliesPhotonStream_free(&phs);
}

CASE("write_and_read_back_full_single_pulse_event") {
        struct mliMT19937 prng = mliMT19937_init(0u);
        const uint64_t num_channels = 1337;
        const uint64_t num_time_slices = MLIES_NEXT_CHANNEL_MARKER;
        const double time_slice_duration = 0.5e-9;
        const double single_pulse_rate = 50e6;
        const double exposure_time = 127.0e-9;

        CHECK(mlies_test_ExtractChannels_expose_write_and_read_back(
                num_channels,
                num_time_slices,
                time_slice_duration,
                single_pulse_rate,
                exposure_time,
                &prng));
}

CASE("zero_channels") {
        struct mliMT19937 prng = mliMT19937_init(0u);
        const uint64_t num_channels = 0;
        const uint64_t num_time_slices = MLIES_NEXT_CHANNEL_MARKER;
        const double time_slice_duration = 0.5e-9;
        const double single_pulse_rate = 50e6;
        const double exposure_time = 127.0e-9;

        CHECK(mlies_test_ExtractChannels_expose_write_and_read_back(
                num_channels,
                num_time_slices,
                time_slice_duration,
                single_pulse_rate,
                exposure_time,
                &prng));
}

CASE("empty_channels") {
        struct mliMT19937 prng = mliMT19937_init(0u);
        const uint64_t num_channels = 1337;
        const uint64_t num_time_slices = MLIES_NEXT_CHANNEL_MARKER;
        const double time_slice_duration = 0.5e-9;
        const double single_pulse_rate = 0.0;
        const double exposure_time = 127.0e-9;

        CHECK(mlies_test_ExtractChannels_expose_write_and_read_back(
                num_channels,
                num_time_slices,
                time_slice_duration,
                single_pulse_rate,
                exposure_time,
                &prng));
}
