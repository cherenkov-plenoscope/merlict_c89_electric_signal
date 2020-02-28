/* Copyright 2020 Sebastian A. Mueller */


CASE("empty_input_yields_empty_output") {
        struct mliesPhotonChannels photons = mliesPhotonChannels_init();
        struct mliesPulseChannels pulses = mliesPulseChannels_init();
        struct mliMT19937 prng = mliMT19937_init(0);
        struct mliFunc converter_efficiency_vs_wavelength = mliFunc_init();
        const double exposure_time = 500e-9;
        const double converter_dark_rate = 10.0e6;
        const double converter_crosstalk_probability = 0.1;
        const double wavelength = 433e-9;
        const int64_t num_photons = 100;

        mliesPhotonChannels_malloc(&photons, 1);
        mliesPulseChannels_malloc(&pulses, 1);

        converter_efficiency_vs_wavelength.num_points = 2;
        mliFunc_malloc(&converter_efficiency_vs_wavelength);
        converter_efficiency_vs_wavelength.x[0] = 400e-9;
        converter_efficiency_vs_wavelength.x[1] = 500e-9;
        converter_efficiency_vs_wavelength.y[0] = 0.5;
        converter_efficiency_vs_wavelength.y[1] = 0.5;

        CHECK(mlies_append_equi_distant_photons(
                &photons.channels[0],
                wavelength,
                0.0,
                exposure_time,
                num_photons));

        CHECK(pulses.channels[0].size == 0);
        CHECK(mlies_photo_electric_convert(
                &photons.channels[0],
                &pulses.channels[0],
                exposure_time,
                &converter_efficiency_vs_wavelength,
                converter_dark_rate,
                converter_crosstalk_probability,
                &prng));
        CHECK(pulses.channels[0].size > 0.5*num_photons);

        mliFunc_free(&converter_efficiency_vs_wavelength);
        mliesPulseChannels_free(&pulses);
        mliesPhotonChannels_free(&photons);
}

/*

CASE("input_pulses_absorbed_zero_qunatum_eff") {
    signal_processing::PhotoElectricConverter::Config config;
    signal_processing::PhotoElectricConverter::Converter conv(&config);

    std::vector<signal_processing::PipelinePhoton> photons =
        equi_distant_photons(1337u);

    double exposure_time = 1337e-9;
    ml::random::Mt19937 prng(0);
    std::vector<signal_processing::ElectricPulse> result = conv.
        get_pulses_for_photons(
            photons,
            exposure_time,
            &prng);

    CHECK(result.size() == 0u);
}

CASE("input_pulses_pass_qunatum_eff_is_one") {
    signal_processing::PhotoElectricConverter::Config config;
    ml::function::Func1 qeff({{200e-9, 1.0}, {1200e-9, 1.0}});
    config.quantum_efficiency_vs_wavelength = &qeff;
    signal_processing::PhotoElectricConverter::Converter conv(&config);

    std::vector<signal_processing::PipelinePhoton> photons =
        equi_distant_photons(1337u);

    double exposure_time = 1337e-9;
    ml::random::Mt19937 prng(0);
    std::vector<signal_processing::ElectricPulse> result = conv.
        get_pulses_for_photons(
            photons,
            exposure_time,
            &prng);

    REQUIRE(result.size() == 1337u);
    for (unsigned int i = 0; i < result.size(); i++) {
        CHECK(result.at(i).arrival_time == photons.at(i).arrival_time);
    }
}

CASE("dark_rate_on_empty_photon_pipe") {
    signal_processing::PhotoElectricConverter::Config config;
    config.dark_rate = 100e6;
    signal_processing::PhotoElectricConverter::Converter conv(&config);

    std::vector<signal_processing::PipelinePhoton> photons;

    double exposure_time = 1e-6;
    ml::random::Mt19937 prng(0);
    std::vector<signal_processing::ElectricPulse> result = conv.
        get_pulses_for_photons(
            photons,
            exposure_time,
            &prng);

    CHECK(100u == Approx(result.size()).margin(3));
}

CASE("triangle_qeff") {
    // Qeff [1]                                            //
    //                                                     //
    // _1.0_|                                              //
    //      |            /\                                //
    //      |           /  \                               //
    //      |          /    \                              //
    //      |         /      \                             //
    //      |        /        \                            //
    //      |       /          \                           //
    //      |      /            \                          //
    //      |     /              \                         //
    //      |    /                \                        //
    //      |   /                  \                       //
    //      |  /                    \                      //
    // _0.0_|--|----------|----------|----                 //
    //        200e-9    700e-9     1200e-9 [nm] wavelength //

    signal_processing::PhotoElectricConverter::Config config;
    std::vector<std::vector<double>> raw_qeff {{  200e-9, 0.0},
                                     {  700e-9, 1.0},
                                     { 1200e-9, 0.0}};
    ml::function::Func1 qeff(raw_qeff);
    config.quantum_efficiency_vs_wavelength = &qeff;
    signal_processing::PhotoElectricConverter::Converter conv(&config);

    std::vector<signal_processing::PipelinePhoton> photons;
    const unsigned int n = 1000*1000;
    for (unsigned int i = 0; i < n; i++) {
        const double arrival_time = static_cast<double>(i);
        const double wavelength = 200e-9 + static_cast<double>(i)/
            static_cast<double>(n)*1000e-9;
        const int simulation_truth_id = i;
        photons.push_back(
            signal_processing::PipelinePhoton(
                arrival_time,
                wavelength,
                simulation_truth_id));
    }

    double exposure_time = 1.0;
    ml::random::Mt19937 prng(0);
    std::vector<signal_processing::ElectricPulse> result = conv.
        get_pulses_for_photons(
            photons,
            exposure_time,
            &prng);

    CHECK(n/2 == Approx(result.size()).margin(static_cast<double>(n)));

    std::vector<double> survived_arrival_times;
    // We created a correlation between wavelength and arrival
    // time in this case on purpose.
    for (const signal_processing::ElectricPulse pulse : result)
        survived_arrival_times.push_back(pulse.arrival_time);

    ml::Histogram1 hist(
        survived_arrival_times,
        {
            0,
            100*1000,
            200*1000,
            300*1000,
            400*1000,
            500*1000,
            600*1000,
            700*1000,
            800*1000,
            900*1000,
            1000*1000
        });

    CHECK(10*1000u == Approx(hist.bins.at(0)).margin(58));
    CHECK(30*1000u == Approx(hist.bins.at(1)).margin(253));
    CHECK(50*1000u == Approx(hist.bins.at(2)).margin(119));
    CHECK(70*1000u == Approx(hist.bins.at(3)).margin(39));
    CHECK(90*1000u == Approx(hist.bins.at(4)).margin(54));
    CHECK(90*1000u == Approx(hist.bins.at(5)).margin(228));
    CHECK(70*1000u == Approx(hist.bins.at(6)).margin(71));
    CHECK(50*1000u == Approx(hist.bins.at(7)).margin(194));
    CHECK(30*1000u == Approx(hist.bins.at(8)).margin(37));
    CHECK(10*1000u == Approx(hist.bins.at(9)).margin(25));
}
*/