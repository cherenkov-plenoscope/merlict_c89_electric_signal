/* Copyright 2019-2020 Sebastian Achim Mueller                                */

CASE("DynExtract simple") {
        struct mliesExtract c;
        struct mliesDynExtract channel = mliesDynExtract_init();
        CHECK(_mliesDynExtract_test_after_init(&channel));

        CHECK(mliesDynExtract_malloc(&channel, 10));
        CHECK(_mliesDynExtract_test_after_malloc(&channel, 10));

        channel.arr[0].simulation_truth_id = 42;
        channel.arr[0].arrival_time_slice = 137;
        c = channel.arr[0];
        CHECK(c.simulation_truth_id == 42);
        CHECK(c.arrival_time_slice == 137);

        mliesDynExtract_free(&channel);
        CHECK(_mliesDynExtract_test_after_free(&channel));
}

CASE("DynExtract malloc 0") {
        struct mliesDynExtract channel = mliesDynExtract_init();
        CHECK(_mliesDynExtract_test_after_init(&channel));

        CHECK(mliesDynExtract_malloc(&channel, 0));
        CHECK(_mliesDynExtract_test_after_malloc(&channel, 0));

        mliesDynExtract_free(&channel);
        CHECK(_mliesDynExtract_test_after_free(&channel));
}

CASE("DynExtract push") {
        uint64_t i;
        struct mliesDynExtract channel = mliesDynExtract_init();
        CHECK(_mliesDynExtract_test_after_init(&channel));

        CHECK(mliesDynExtract_malloc(&channel, 0));
        CHECK(_mliesDynExtract_test_after_malloc(&channel, 0));

        for (i = 0; i < 1337; i++) {
                struct mliesExtract extract;
                extract.arrival_time_slice = 1;
                extract.simulation_truth_id = i;
                CHECK(mliesDynExtract_push_back(&channel, extract));
        }
        CHECK(channel.dyn.size == 1337);
        CHECK(channel.arr != NULL);
        CHECK(channel.dyn.capacity >= channel.dyn.size);

        mliesDynExtract_free(&channel);
        CHECK(_mliesDynExtract_test_after_free(&channel));
}

/* */

CASE("DynPulse simple") {
        struct mliesPulse c;
        struct mliesDynPulse channel = mliesDynPulse_init();
        CHECK(_mliesDynPulse_test_after_init(&channel));

        CHECK(mliesDynPulse_malloc(&channel, 10));
        CHECK(_mliesDynPulse_test_after_malloc(&channel, 10));

        channel.arr[0].simulation_truth_id = 42;
        channel.arr[0].arrival_time = 137.0;
        c = channel.arr[0];
        CHECK(c.simulation_truth_id == 42);
        CHECK(c.arrival_time == 137.0);

        mliesDynPulse_free(&channel);
        CHECK(_mliesDynPulse_test_after_free(&channel));
}

CASE("DynPulse malloc 0") {
        struct mliesDynPulse channel = mliesDynPulse_init();
        CHECK(_mliesDynPulse_test_after_init(&channel));

        CHECK(mliesDynPulse_malloc(&channel, 0));
        CHECK(_mliesDynPulse_test_after_malloc(&channel, 0));

        mliesDynPulse_free(&channel);
        CHECK(_mliesDynPulse_test_after_free(&channel));
}

CASE("DynPulse push") {
        uint64_t i;
        struct mliesDynPulse channel = mliesDynPulse_init();
        CHECK(_mliesDynPulse_test_after_init(&channel));

        CHECK(mliesDynPulse_malloc(&channel, 0));
        CHECK(_mliesDynPulse_test_after_malloc(&channel, 0));

        for (i = 0; i < 1337; i++) {
                struct mliesPulse pu;
                pu.arrival_time = 1.0;
                pu.simulation_truth_id = i;
                CHECK(mliesDynPulse_push_back(&channel, pu));
        }
        CHECK(channel.dyn.size == 1337);
        CHECK(channel.arr != NULL);
        CHECK(channel.dyn.capacity >= channel.dyn.size);

        mliesDynPulse_free(&channel);
        CHECK(_mliesDynPulse_test_after_free(&channel));
}

/* */

CASE("DynPhoton simple") {
        struct mliesPhoton c;
        struct mliesDynPhoton channel = mliesDynPhoton_init();
        CHECK(_mliesDynPhoton_test_after_init(&channel));

        CHECK(mliesDynPhoton_malloc(&channel, 10));
        CHECK(_mliesDynPhoton_test_after_malloc(&channel, 10));

        channel.arr[0].simulation_truth_id = 42;
        channel.arr[0].arrival_time = 137.0;
        channel.arr[0].wavelength = 433e-9;
        c = channel.arr[0];
        CHECK(c.simulation_truth_id == 42);
        CHECK(c.arrival_time == 137.0);
        CHECK(c.wavelength == 433e-9);

        mliesDynPhoton_free(&channel);
        CHECK(_mliesDynPhoton_test_after_free(&channel));
}

CASE("DynPhoton malloc 0") {
        struct mliesDynPhoton channel = mliesDynPhoton_init();
        CHECK(_mliesDynPhoton_test_after_init(&channel));

        CHECK(mliesDynPhoton_malloc(&channel, 0));
        CHECK(_mliesDynPhoton_test_after_malloc(&channel, 0));

        mliesDynPhoton_free(&channel);
        CHECK(_mliesDynPhoton_test_after_free(&channel));
}

CASE("DynPhoton push") {
        uint64_t i;
        struct mliesDynPhoton channel = mliesDynPhoton_init();
        CHECK(_mliesDynPhoton_test_after_init(&channel));

        CHECK(mliesDynPhoton_malloc(&channel, 0));
        CHECK(_mliesDynPhoton_test_after_malloc(&channel, 0));

        for (i = 0; i < 1337; i++) {
                struct mliesPhoton pu;
                pu.arrival_time = 1.0;
                pu.simulation_truth_id = i;
                CHECK(mliesDynPhoton_push_back(&channel, pu));
        }
        CHECK(channel.dyn.size == 1337);
        CHECK(channel.arr != NULL);
        CHECK(channel.dyn.capacity >= channel.dyn.size);

        mliesDynPhoton_free(&channel);
        CHECK(_mliesDynPhoton_test_after_free(&channel));
}

/* */
