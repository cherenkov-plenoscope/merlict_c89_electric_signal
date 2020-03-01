/* Copyright 2020 Sebastian Achim Mueller */
#ifndef MLIES_EXTRACTCHANNELS_IO_H_
#define MLIES_EXTRACTCHANNELS_IO_H_

#include <stdint.h>
#include "mliesExtractChannels.h"
#include "mlies_constants.h"

#define MLIES_PHOTON_STREAM_FORMAT_VERSION 20200226

int mliesPhotonStream_append_to_file(
        const struct mliesPhotonStream *phs,
        FILE *file)
{
        const uint64_t num_symbols =
                mliesExtractChannels_total_num(&phs->stream) +
                phs->stream.num_channels;
        uint64_t ch, pu;
        /* Header */
        mli_write_type(uint8_t, 'P', file);
        mli_write_type(uint8_t, 'H', file);
        mli_write_type(uint8_t, 'S', file);
        mli_write_type(uint8_t, '\n', file);
        mli_write_type(uint64_t, MLIES_PHOTON_STREAM_FORMAT_VERSION, file);

        mli_write_type(double, phs->time_slice_duration, file);
        mli_write_type(uint64_t, phs->stream.num_channels, file);
        mli_write_type(uint64_t, phs->num_time_slices, file);
        mli_write_type(uint64_t, num_symbols, file);

        for (ch = 0; ch < phs->stream.num_channels; ch++) {
                for (pu = 0; pu < phs->stream.channels[ch].dyn.size; pu++) {
                        struct mliesExtract expulse =
                                phs->stream.channels[ch].arr[pu];
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
        uint64_t num_channels = 0;
        uint64_t channel = 0;
        uint64_t symbol = 0;
        uint64_t i = 0;

        mliesExtractChannels_free(&phs->stream);

        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'P', "Expected char[0] to be 'P'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'H', "Expected char[1] to be 'H'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == 'S', "Expected char[2] to be 'S'.");
        mli_fread(&temp_char, sizeof(uint8_t), 1, file);
        mli_check(temp_char == '\n', "Expected char[3] to be newline.");

        mli_fread(&format_version, sizeof(uint64_t), 1, file);
        mli_c(format_version == MLIES_PHOTON_STREAM_FORMAT_VERSION);

        mli_fread(&phs->time_slice_duration, sizeof(double), 1, file);
        mli_c(phs->time_slice_duration >= 0.);

        mli_fread(&num_channels, sizeof(uint64_t), 1, file);

        mli_fread(&phs->num_time_slices, sizeof(uint64_t), 1, file);

        mli_fread(&num_symbols, sizeof(uint64_t), 1, file);

        mli_c(mliesExtractChannels_malloc(&phs->stream, num_channels));

        channel = 0;
        for (i = 0; i < num_symbols; i++) {
                mli_fread(&symbol, sizeof(uint8_t), 1, file);
                if (symbol == MLIES_NEXT_CHANNEL_MARKER) {
                        channel++;
                } else {
                        struct mliesExtract expulse;
                        expulse.arrival_time_slice = symbol;
                        mli_c(mliesDynExtract_push_back(
                            &phs->stream.channels[channel],
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
        for (ch = 0; ch < phs->stream.num_channels; ch++) {
                for (pu = 0; pu < phs->stream.channels[ch].dyn.size; pu++) {
                        mli_fread(
                                &phs->stream.channels[ch].arr[pu].
                                    simulation_truth_id,
                                sizeof(int32_t),
                                1,
                                file);
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
        for (ch = 0; ch < phs->stream.num_channels; ch++) {
                for (pu = 0; pu < phs->stream.channels[ch].dyn.size; pu++) {
                        struct mliesExtract expulse =
                            phs->stream.channels[ch].arr[pu];
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
        mli_check(f != NULL, "Can not open ExtractChannels for writing.");
        mli_check(mliesPhotonStream_append_to_file(phs, f),
                "Failed to write ExtractChannels to file.");
        fclose(f);

        f = fopen(truthpath, "w");
        mli_check(f != NULL, "Can not open ExtractChannels-truth for writing.");
        mli_check(mliesPhotonStream_append_simulation_truth_to_file(phs, f),
                "Failed to write ExtractChannels-truth to file.");
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
        mli_check(f != NULL, "Can not open ExtractChannels for reading.");
        mli_check(
                mliesPhotonStream_malloc_from_file(phs, f),
                "Failed to read ExtractChannels from file.");
        fclose(f);

        f = fopen(truthpath, "r");
        mli_check(f != NULL, "Can not open ExtractChannels-truth for reading.");
        mli_check(
                mliesPhotonStream_read_simulation_truth_from_file(phs, f),
                "Failed to read ExtractChannels-truth from file.");
        fclose(f);

        return 1;
    error:
        if (f != NULL) {
                fclose(f);
        }
        mliesExtractChannels_free(&phs->stream);
        return 0;
}

#endif
