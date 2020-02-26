/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLISP_PHOTONSTREAM_IO_H_
#define MLISP_PHOTONSTREAM_IO_H_

#include <stdint.h>
#include "mlispPhotonStream.h"
#include "mlisp_constants.h"

#define MLISP_PHOTONSTREAM_FORMAT_VERSION 20200226

int mlispPhotonStream_append_to_file(
        const struct mlispPhotonStream *phs,
        FILE *file)
{
        const uint64_t num_symbols =
                mlispPhotonStream_num_pulses(phs) + phs->num_channels;
        uint64_t ch, pu;
        /* Header */
        mli_write_type(uint8_t, 'P', file);
        mli_write_type(uint8_t, 'H', file);
        mli_write_type(uint8_t, 'S', file);
        mli_write_type(uint8_t, '\n', file);
        mli_write_type(uint64_t, MLISP_PHOTONSTREAM_FORMAT_VERSION, file);

        mli_write_type(double, phs->time_slice_duration, file);
        mli_write_type(uint64_t, phs->num_channels, file);
        mli_write_type(uint64_t, MLISP_NUM_TIME_SLICES, file);
        mli_write_type(uint64_t, num_symbols, file);

        for (ch = 0; ch < phs->num_channels; ch++) {
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mlispExtractedPulse expulse =
                                mlispExtractedPulseVector_at(
                                        &phs->channels[ch],
                                        pu);
                        mli_check(
                                expulse.arrival_time_slice !=
                                MLISP_NEXT_CHANNEL_MARKER,
                                "Expected arrival_time_slice != "
                                "MLISP_NEXT_CHANNEL_MARKER");
                        mli_check(
                                expulse.arrival_time_slice <
                                phs->num_time_slices,
                                "Expected arrival_time_slice < "
                                "phs->num_time_slices")
                        mli_write_type(
                                uint8_t,expulse.arrival_time_slice, file);
                }
                mli_write_type(uint8_t, MLISP_NEXT_CHANNEL_MARKER, file);
        }
        return 1;
   error:
        return 0;
}

int mlispPhotonStream_malloc_from_file(
    struct mlispPhotonStream *phs,
    FILE *file)
{
        uint8_t temp_char = 0;
        uint64_t format_version = 0;
        uint64_t num_symbols = 0;
        uint64_t channel = 0;
        uint64_t symbol = 0;
        uint64_t i = 0;

        mlispPhotonStream_free(phs);

        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'P', "Expected first char to be 'P'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'H', "Expected first char to be 'H'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'S', "Expected first char to be 'S'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == '\n', "Expected first char to be newline.");

        mli_fread(&format_version, sizeof(uint64_t), 1, file);
        mli_c(format_version == MLISP_PHOTONSTREAM_FORMAT_VERSION);

        mli_fread(&phs->time_slice_duration, sizeof(double), 1, file);
        mli_c(phs->time_slice_duration >= 0.);

        mli_fread(&phs->num_channels, sizeof(uint64_t), 1, file);

        mli_fread(&phs->num_time_slices, sizeof(uint64_t), 1, file);
        mli_c(phs->num_time_slices == MLISP_NUM_TIME_SLICES);

        mli_fread(&num_symbols, sizeof(uint64_t), 1, file);

        mli_c(mlispPhotonStream_malloc(phs));

        channel = 0;
        for (i = 0; i < num_symbols; i++) {
                mli_fread(&symbol, sizeof(uint8_t), 1, file);
                if (symbol == MLISP_NEXT_CHANNEL_MARKER) {
                        channel++;
                } else {
                        struct mlispExtractedPulse expulse;
                        expulse.arrival_time_slice = symbol;
                        mli_c(mlispExtractedPulseVector_push_back(
                                &phs->channels[channel],
                                expulse));
                }
        }
        return 1;
   error:
        return 0;
}

int mlispPhotonStream_read_simulation_truth_from_file(
        struct mlispPhotonStream *phs,
        FILE *file)
{
        uint64_t ch, pu;
        for (ch = 0; ch < phs->num_channels; ch++) {
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mlispExtractedPulse expulse =
                                mlispExtractedPulseVector_at(
                                        &phs->channels[ch],
                                        pu);
                        mli_fread(
                                &expulse.simulation_truth_id,
                                sizeof(int32_t),
                                1,
                                file);
                        mlispExtractedPulseVector_assign(
                                &phs->channels[ch],
                                pu,
                                expulse);
                }
        }

        return 1;
   error:
        return 0;
}

int mlispPhotonStream_append_simulation_truth_to_file(
        const struct mlispPhotonStream *phs,
        FILE *file)
{
        uint64_t ch, pu;
        for (ch = 0; ch < phs->num_channels; ch++) {
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mlispExtractedPulse expulse =
                                mlispExtractedPulseVector_at(
                                        &phs->channels[ch],
                                        pu);
                        mli_write_type(
                                int32_t,
                                expulse.simulation_truth_id,
                                file);
                }
        }
        return 1;
   error:
        return 0;
}

int mlispPhotonStream_write_to_pulsepath_and_truthpath(
        struct mlispPhotonStream *phs,
        const char *pulsepath,
        const char *truthpath)
{
        FILE *f;
        f = fopen(pulsepath, "w");
        mli_check(f != NULL, "Can not open PhotonStream for writing.");
        mli_check(mlispPhotonStream_append_to_file(phs, f),
                "Failed to write PhotonStream to file.");
        fclose(f);

        f = fopen(truthpath, "w");
        mli_check(f != NULL, "Can not open PhotonStream-truth for writing.");
        mli_check(mlispPhotonStream_append_simulation_truth_to_file(phs, f),
                "Failed to write PhotonStream-truth to file.");
        fclose(f);

        return 1;
    error:
        if (f != NULL) {
                fclose(f);
        }
        return 0;
}

int mlispPhotonStream_malloc_from_path(
        struct mlispPhotonStream *phs,
        const char *pulsepath,
        const char *truthpath)
{
        FILE *f;
        f = fopen(pulsepath, "r");
        mli_check(f != NULL, "Can not open PhotonStream for reading.");
        mli_check(
                mlispPhotonStream_malloc_from_file(phs, f),
                "Failed to read PhotonStream from file.");
        fclose(f);

        f = fopen(truthpath, "r");
        mli_check(f != NULL, "Can not open PhotonStream-truth for reading.");
        mli_check(
                mlispPhotonStream_read_simulation_truth_from_file(phs, f),
                "Failed to read PhotonStream-truth from file.");
        fclose(f);

        return 1;
    error:
        if (f != NULL) {
                fclose(f);
        }
        mlispPhotonStream_free(phs);
        return 0;
}

#endif
