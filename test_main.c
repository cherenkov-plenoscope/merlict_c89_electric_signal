/* Copyright 2020 Sebastian Achim Mueller                                     */
/* Compile with:                                                              */
/* gcc test_main.c -o test -std=c89 -lm -Wall -pedantic    */
#include "../merlict_c89/mli_testing.h"
#include "mli_single_photon.h"


int main(int argc, char *argv[]) {
        #include "tests/test_init.c"
        #include "tests/test_add_pulse_with_crosstalk.c"

        printf("__SUCCESS__\n");
        return EXIT_SUCCESS;
test_failure:
        printf("__FAILURE__\n");
        return EXIT_FAILURE;
}
