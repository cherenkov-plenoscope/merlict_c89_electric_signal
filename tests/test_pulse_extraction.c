
CASE("truncate_invalid_arrival_times") {
    struct mliMT19937 prng = mliMT19937_init(0);
    const double time_slice_duration = 500e-12;
    const double arrival_time_std = 0.0;
    uint64_t num_invalid_photons = 0u;
    uint64_t num_passing_photons = 0u;
    int64_t i, p, ph;

    struct mlispPulseVector pulse_pipeline = mlispPulseVector_init();
    struct mlispExtractedPulseVector extraction_pipeline;

    CHECK(mlispPulseVector_malloc(&pulse_pipeline, 0u));
    for (i = -1000; i < 1000; i++) {
        struct mlispPulse pulse;
        pulse.arrival_time = time_slice_duration*(double)i;
        pulse.simulation_truth_id = 0;
        CHECK(mlispPulseVector_push_back(&pulse_pipeline, pulse));
    }

    num_invalid_photons = 0u;
    for (p = 0; p < pulse_pipeline.vector.size; p++) {
        struct mlispPulse pulse = mlispPulseVector_at(&pulse_pipeline, p);
        int32_t slice = mli_round(pulse.arrival_time/time_slice_duration);

        if (slice < 0 || slice >= MLISP_NUM_TIME_SLICES) {
            num_invalid_photons++;
        }
    }

    CHECK(2000 - MLISP_NUM_TIME_SLICES == num_invalid_photons);

    extraction_pipeline = mlispExtractedPulseVector_init();
    CHECK(mlispExtractedPulseVector_malloc(&extraction_pipeline, 0u));
    CHECK(mlisp_extract_pulses(
        &pulse_pipeline,
        &extraction_pipeline,
        time_slice_duration,
        MLISP_NUM_TIME_SLICES,
        arrival_time_std,
        &prng));

    CHECK(pulse_pipeline.vector.size != extraction_pipeline.vector.size);
    num_passing_photons = 0;
    for (ph = 0; ph < extraction_pipeline.vector.size; ph++) {
            struct mlispExtractedPulse expulse = mlispExtractedPulseVector_at(
            &extraction_pipeline,
            ph);
        CHECK(255 > expulse.arrival_time_slice);
        num_passing_photons++;
    }

    CHECK(MLISP_NUM_TIME_SLICES == num_passing_photons);
    mlispExtractedPulseVector_free(&extraction_pipeline);
    mlispPulseVector_free(&pulse_pipeline);
}

/*
CASE("arrival_time_std") {
    struct mliMT19937 prng = mliMT19937_init(0);
    const double time_slice_duration = 500e-12;
    const double arrival_time_std = 5e-9;
    const double true_arrival_time = 25e-9;

    std::vector<std::vector<signal_processing::ElectricPulse>> response;
    std::vector<signal_processing::ElectricPulse> pulse_pipeline;
    for (int i = 0; i < 10*1000; i++) {
        signal_processing::ElectricPulse pulse;
        pulse.arrival_time = true_arrival_time;
        pulse.simulation_truth_id = i;
        pulse_pipeline.push_back(pulse);
    }
    response.push_back(pulse_pipeline);

    std::vector<double> true_arrival_times;
    for (signal_processing::ElectricPulse &pulse : response.at(0))
        true_arrival_times.push_back(pulse.arrival_time);

    CHECK(0.0 == Approx(ml::numeric::stddev(true_arrival_times)).margin(1e-1));
    CHECK(true_arrival_time == Approx(ml::numeric::mean(true_arrival_times)).margin(1e-1));

    std::vector<std::vector<signal_processing::ExtractedPulse>> raw =
        signal_processing::extract_pulses(
            response,
            time_slice_duration,
            arrival_time_std,
            &prng);

    std::vector<double> reconstructed_arrival_times;
    for (signal_processing::ExtractedPulse &pulse : raw.at(0))
        reconstructed_arrival_times.push_back(
            pulse.arrival_time_slice * time_slice_duration);

    CHECK(arrival_time_std == Approx(ml::numeric::stddev(reconstructed_arrival_times)).margin(1e-10));
    CHECK(true_arrival_time == Approx(ml::numeric::mean(reconstructed_arrival_times)).margin(1e-10));
}
*/