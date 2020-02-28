/* Copyright 2020 Sebastian Achim Mueller                                     */
/* Compile with:                                                              */
/* gcc test_main.c -o test -std=c89 -lm -Wall -pedantic                       */
/* g++ test_main.c -o test -lm -Wall -pedantic                                */
#include "../merlict_c89/mli_testing.h"
#include "merlict_c89_electric_signal.h"


int main(int argc, char *argv[]) {
        #include "tests/test_init.c"
        #include "tests/test_add_pulse_with_crosstalk.c"
        #include "tests/test_pulse_extraction.c"
        #include "tests/test_photonstream.c"
        #include "tests/test_photo_electric_converter.c"

        printf("__SUCCESS__\n");
        return EXIT_SUCCESS;
test_failure:
        printf("__FAILURE__\n");
        return EXIT_FAILURE;
}
