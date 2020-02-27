/* Copyright 2019-2020 Sebastian Achim Mueller                                */

CASE("init PhotonStream") {
        struct mlispPhotonStream phs = mlispPhotonStream_init();
        CHECK(phs.time_slice_duration == 0.);
        CHECK(phs.num_channels == 0u);
        CHECK(phs.channels == NULL);
}

CASE("malloc PhotonStream") {
        struct mlispPhotonStream phs = mlispPhotonStream_init();
        phs.num_channels = 12u;
        CHECK(mlispPhotonStream_malloc(&phs));
        CHECK(phs.num_channels == 12u);
        mlispPhotonStream_free(&phs);
        CHECK(phs.num_channels == 0u);
}

CASE("num_pulses PhotonStream") {
        uint64_t num_channel_scenarios = 3;
        uint64_t channel_scenarios[3] = {0, 1, 10};
        uint64_t chsc = 0;

        uint64_t num_pulse_scenarios = 4;
        uint64_t pulse_scenarios[4] = {0, 1, 10, 50};
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

                        struct mlispExtractedPulse ex;
                        struct mlispPhotonStream phs = mlispPhotonStream_init();
                        phs.num_channels = channel_scenarios[chsc];
                        CHECK(mlispPhotonStream_malloc(&phs));

                        for (c = 0; c < channel_scenarios[chsc]; c++) {
                                for (p = 0; p < pulse_scenarios[pusc]; p++) {
                                        ex.simulation_truth_id = p + 1 + c;
                                        ex.arrival_time_slice = p + c;
                                        mlispExtractedPulseVector_push_back(
                                                &phs.channels[0],
                                                ex);
                                }
                        }

                        CHECK(
                                mlispPhotonStream_num_pulses(&phs) ==
                                expected_num_pulses);
                        mlispPhotonStream_free(&phs);
                }
        }
}

CASE("arrival_time_slices_below_next_channel_marker") {
        struct mlispPhotonStream phs = mlispPhotonStream_init();
        struct mlispPhotonStream phs_back = mlispPhotonStream_init();
        struct mlispExtractedPulse expulse = mlispExtractedPulse_init();
        struct mlispExtractedPulse expulse_back = mlispExtractedPulse_init();

        phs.num_channels = 1u;
        phs.num_time_slices = MLISP_NEXT_CHANNEL_MARKER;
        phs.time_slice_duration = 500e-12;
        CHECK(mlispPhotonStream_malloc(&phs));

        expulse.arrival_time_slice = 254u;
        expulse.simulation_truth_id = 0;

        CHECK(mlispExtractedPulseVector_push_back(
                &phs.channels[0],
                expulse));

        CHECK(mlispPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.tmp",
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.truth.tmp"));

        CHECK(mlispPhotonStream_malloc_from_path(
                &phs_back,
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.tmp",
                "tests/"
                "resources/"
                "arrival_time_slices_below_next_channel_marker.phs.truth.tmp"));

        CHECK(phs_back.num_channels == phs.num_channels);
        expulse_back = mlispExtractedPulseVector_at(
                &phs_back.channels[0],
                0);

        CHECK(expulse_back.arrival_time_slice == expulse.arrival_time_slice);
        CHECK(expulse_back.simulation_truth_id == expulse.simulation_truth_id);

        CHECK(mlisp_test_PhotonStream_is_equal_verbose(
                &phs,
                &phs_back));

        mlispPhotonStream_free(&phs);
        mlispPhotonStream_free(&phs_back);
}

CASE("arrival_slices_must_not_be_NEXT_CHANNEL_MARKER") {
        struct mlispPhotonStream phs = mlispPhotonStream_init();
        struct mlispExtractedPulse expulse = mlispExtractedPulse_init();
        const int32_t simulation_truth_id = 1337;
        int64_t rc;
        phs.num_channels = 1u;
        phs.num_time_slices = MLISP_NEXT_CHANNEL_MARKER;
        phs.time_slice_duration = 500e-12;
        CHECK(mlispPhotonStream_malloc(&phs));

        expulse.simulation_truth_id = simulation_truth_id;
        expulse.arrival_time_slice = MLISP_NEXT_CHANNEL_MARKER;
        CHECK(mlispExtractedPulseVector_push_back(
                &phs.channels[0],
                expulse));

        rc = mlispPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "must_fail.phs.tmp",
                "tests/"
                "resources/"
                "must_fail.phs.truth.tmp");
        CHECK(rc < 1);

        expulse.arrival_time_slice = MLISP_NEXT_CHANNEL_MARKER - 1;
        mlispExtractedPulseVector_assign(
                &phs.channels[0],
                0,
                expulse);

        rc = mlispPhotonStream_write_to_pulsepath_and_truthpath(
                &phs,
                "tests/"
                "resources/"
                "fine.phs.tmp",
                "tests/"
                "resources/"
                "fine.phs.truth.tmp");
        CHECK(rc);

        mlispPhotonStream_free(&phs);
}

CASE("write_and_read_back_full_single_pulse_event") {
        struct mliMT19937 prng = mliMT19937_init(0u);
        const uint64_t num_channels = 1337;
        const uint64_t num_time_slices = MLISP_NEXT_CHANNEL_MARKER;
        const double time_slice_duration = 0.5e-9;
        const double single_pulse_rate = 50e6;
        const double exposure_time = 127.0e-9;

        CHECK(mlisp_test_PhotonStream_expose_write_and_read_back(
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
        const uint64_t num_time_slices = MLISP_NEXT_CHANNEL_MARKER;
        const double time_slice_duration = 0.5e-9;
        const double single_pulse_rate = 50e6;
        const double exposure_time = 127.0e-9;

        CHECK(mlisp_test_PhotonStream_expose_write_and_read_back(
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
        const uint64_t num_time_slices = MLISP_NEXT_CHANNEL_MARKER;
        const double time_slice_duration = 0.5e-9;
        const double single_pulse_rate = 0.0;
        const double exposure_time = 127.0e-9;

        CHECK(mlisp_test_PhotonStream_expose_write_and_read_back(
                num_channels,
                num_time_slices,
                time_slice_duration,
                single_pulse_rate,
                exposure_time,
                &prng));
}
