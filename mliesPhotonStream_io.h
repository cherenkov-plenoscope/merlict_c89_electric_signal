/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIES_PHOTONSTREAM_IO_H_
#define MLIES_PHOTONSTREAM_IO_H_

#include <stdint.h>
#include "mliesPhotonStream.h"
#include "mlies_constants.h"

#define MLIES_PHOTONSTREAM_FORMAT_VERSION 20200226

int mliesPhotonStream_append_to_file(
        const struct mliesPhotonStream *phs,
        FILE *file)
{
        const uint64_t num_symbols =
                mliesPhotonStream_num_pulses(phs) + phs->num_channels;
        uint64_t ch, pu;
        /* Header */
        mli_write_type(uint8_t, 'P', file);
        mli_write_type(uint8_t, 'H', file);
        mli_write_type(uint8_t, 'S', file);
        mli_write_type(uint8_t, '\n', file);
        mli_write_type(uint64_t, MLIES_PHOTONSTREAM_FORMAT_VERSION, file);

        mli_write_type(double, phs->time_slice_duration, file);
        mli_write_type(uint64_t, phs->num_channels, file);
        mli_write_type(uint64_t, phs->num_time_slices, file);
        mli_write_type(uint64_t, num_symbols, file);

        for (ch = 0; ch < phs->num_channels; ch++) {
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mliesExtractedPulse expulse =
                                mliesExtractedPulseVector_at(
                                        &phs->channels[ch],
                                        pu);
                        mli_check(
                                expulse.arrival_time_slice !=
                                MLIES_NEXT_CHANNEL_MARKER,
                                "Expected arrival_time_slice != "
                                "MLIES_NEXT_CHANNEL_MARKER");
                        mli_check(
                                expulse.arrival_time_slice <
                                phs->num_time_slices,
                                "Expected arrival_time_slice < "
                                "phs->num_time_slices")
                        mli_write_type(
                                uint8_t,expulse.arrival_time_slice, file);
                }
                mli_write_type(uint8_t, MLIES_NEXT_CHANNEL_MARKER, file);
        }
        return 1;
   error:
        return 0;
}

int mliesPhotonStream_malloc_from_file(
    struct mliesPhotonStream *phs,
    FILE *file)
{
        uint8_t temp_char = 0;
        uint64_t format_version = 0;
        uint64_t num_symbols = 0;
        uint64_t channel = 0;
        uint64_t symbol = 0;
        uint64_t i = 0;

        mliesPhotonStream_free(phs);

        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'P', "Expected char[0] to be 'P'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'H', "Expected char[1] to be 'H'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'S', "Expected char[2] to be 'S'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == '\n', "Expected char[3] to be newline.");

        mli_fread(&format_version, sizeof(uint64_t), 1, file);
        mli_c(format_version == MLIES_PHOTONSTREAM_FORMAT_VERSION);

        mli_fread(&phs->time_slice_duration, sizeof(double), 1, file);
        mli_c(phs->time_slice_duration >= 0.);

        mli_fread(&phs->num_channels, sizeof(uint64_t), 1, file);

        mli_fread(&phs->num_time_slices, sizeof(uint64_t), 1, file);

        mli_fread(&num_symbols, sizeof(uint64_t), 1, file);

        mli_c(mliesPhotonStream_malloc(phs));

        channel = 0;
        for (i = 0; i < num_symbols; i++) {
                mli_fread(&symbol, sizeof(uint8_t), 1, file);
                if (symbol == MLIES_NEXT_CHANNEL_MARKER) {
                        channel++;
                } else {
                        struct mliesExtractedPulse expulse;
                        expulse.arrival_time_slice = symbol;
                        mli_c(mliesExtractedPulseVector_push_back(
                                &phs->channels[channel],
                                expulse));
                }
        }
        return 1;
   error:
        return 0;
}

int mliesPhotonStream_read_simulation_truth_from_file(
        struct mliesPhotonStream *phs,
        FILE *file)
{
        uint64_t ch, pu;
        for (ch = 0; ch < phs->num_channels; ch++) {
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mliesExtractedPulse expulse =
                                mliesExtractedPulseVector_at(
                                        &phs->channels[ch],
                                        pu);
                        mli_fread(
                                &expulse.simulation_truth_id,
                                sizeof(int32_t),
                                1,
                                file);
                        mliesExtractedPulseVector_assign(
                                &phs->channels[ch],
                                pu,
                                expulse);
                }
        }

        return 1;
   error:
        return 0;
}

int mliesPhotonStream_append_simulation_truth_to_file(
        const struct mliesPhotonStream *phs,
        FILE *file)
{
        uint64_t ch, pu;
        for (ch = 0; ch < phs->num_channels; ch++) {
                for (pu = 0; pu < phs->channels[ch].vector.size; pu++) {
                        struct mliesExtractedPulse expulse =
                                mliesExtractedPulseVector_at(
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

int mliesPhotonStream_write_to_pulsepath_and_truthpath(
        struct mliesPhotonStream *phs,
        const char *pulsepath,
        const char *truthpath)
{
        FILE *f;
        f = fopen(pulsepath, "w");
        mli_check(f != NULL, "Can not open PhotonStream for writing.");
        mli_check(mliesPhotonStream_append_to_file(phs, f),
                "Failed to write PhotonStream to file.");
        fclose(f);

        f = fopen(truthpath, "w");
        mli_check(f != NULL, "Can not open PhotonStream-truth for writing.");
        mli_check(mliesPhotonStream_append_simulation_truth_to_file(phs, f),
                "Failed to write PhotonStream-truth to file.");
        fclose(f);

        return 1;
    error:
        if (f != NULL) {
                fclose(f);
        }
        return 0;
}

int mliesPhotonStream_malloc_from_path(
        struct mliesPhotonStream *phs,
        const char *pulsepath,
        const char *truthpath)
{
        FILE *f;
        f = fopen(pulsepath, "r");
        mli_check(f != NULL, "Can not open PhotonStream for reading.");
        mli_check(
                mliesPhotonStream_malloc_from_file(phs, f),
                "Failed to read PhotonStream from file.");
        fclose(f);

        f = fopen(truthpath, "r");
        mli_check(f != NULL, "Can not open PhotonStream-truth for reading.");
        mli_check(
                mliesPhotonStream_read_simulation_truth_from_file(phs, f),
                "Failed to read PhotonStream-truth from file.");
        fclose(f);

        return 1;
    error:
        if (f != NULL) {
                fclose(f);
        }
        mliesPhotonStream_free(phs);
        return 0;
}

#endif
