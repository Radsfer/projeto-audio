#include "wav_handler.h"
#include <stdlib.h>
#include <string.h>

// Estruturas para ler o cabeçalho do arquivo.
// O atributo 'packed' garante que o compilador não adicione preenchimento,
// permitindo a leitura direta do arquivo para a estrutura.
typedef struct __attribute__((packed)) {
    char     riff[4];
    uint32_t overall_size;
    char     wave[4];
} RiffHeader;

typedef struct __attribute__((packed)) {
    char     id[4];
    uint32_t size;
} ChunkHeader;

typedef struct __attribute__((packed)) {
    uint16_t format_type;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} FmtChunk;

// Converte amostra de short 16-bit para double normalizado.
static double short_to_double(int16_t s) {
    return (double)s / 32768.0;
}

// Converte double normalizado para short 16-bit, com clipping.
static int16_t double_to_short(double d) {
    d *= 32767.0;
    if (d > 32767.0) d = 32767.0;
    if (d < -32768.0) d = -32768.0;
    return (int16_t)d;
}

WavData* read_wav_file(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("Erro ao abrir arquivo de entrada");
        return NULL;
    }

    RiffHeader riff_header;
    fread(&riff_header, sizeof(RiffHeader), 1, fp);
    if (strncmp(riff_header.riff, "RIFF", 4) != 0 || strncmp(riff_header.wave, "WAVE", 4) != 0) {
        fprintf(stderr, "Arquivo de entrada não é um WAV válido.\n");
        fclose(fp);
        return NULL;
    }

    WavData* wav_data = (WavData*)calloc(1, sizeof(WavData));
    ChunkHeader chunk_header;
    FmtChunk fmt_chunk;

    // Procura pelo chunk "fmt "
    while (fread(&chunk_header, sizeof(ChunkHeader), 1, fp) == 1) {
        if (strncmp(chunk_header.id, "fmt ", 4) == 0) {
            fread(&fmt_chunk, sizeof(FmtChunk), 1, fp);
            wav_data->sample_rate = fmt_chunk.sample_rate;
            wav_data->num_channels = fmt_chunk.num_channels;
            wav_data->bits_per_sample = fmt_chunk.bits_per_sample;
            if (chunk_header.size > sizeof(FmtChunk)) {
                fseek(fp, chunk_header.size - sizeof(FmtChunk), SEEK_CUR);
            }
        } else if (strncmp(chunk_header.id, "data", 4) == 0) {
            wav_data->data_size = chunk_header.size;
            break; // Encontrou o chunk de dados, para de procurar.
        } else {
            // Pula chunks desconhecidos
            fseek(fp, chunk_header.size, SEEK_CUR);
        }
    }

    if (wav_data->data_size == 0) {
        fprintf(stderr, "Chunk 'data' não encontrado ou vazio.\n");
        fclose(fp);
        free(wav_data);
        return NULL;
    }

    if (wav_data->bits_per_sample != 16) {
        fprintf(stderr, "Este programa suporta apenas arquivos WAV PCM de 16 bits.\n");
        fclose(fp);
        free(wav_data);
        return NULL;
    }

    uint32_t num_total_samples = wav_data->data_size / (wav_data->bits_per_sample / 8);
    wav_data->num_samples = num_total_samples / wav_data->num_channels;
    int16_t* raw_data = (int16_t*)malloc(wav_data->data_size);
    fread(raw_data, wav_data->data_size, 1, fp);

    // Aloca e preenche o buffer de doubles normalizados
    // Este projeto simplifica para MONO, pegando apenas o primeiro canal se for estéreo.
    wav_data->data_double = (double*)malloc(wav_data->num_samples * sizeof(double));
    for (uint32_t i = 0; i < wav_data->num_samples; i++) {
        wav_data->data_double[i] = short_to_double(raw_data[i * wav_data->num_channels]);
    }
    // Se for estéreo, o segundo canal (raw_data[i * num_channels + 1]) é ignorado.

    free(raw_data);
    fclose(fp);
    return wav_data;
}

void write_wav_file(const char* filename, const WavData* data) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Erro ao criar arquivo de saída");
        return;
    }

    // Prepara os dados brutos a partir dos doubles
    uint32_t num_total_samples = data->num_samples * data->num_channels;
    uint32_t data_size = num_total_samples * (data->bits_per_sample / 8);
    int16_t* raw_data = (int16_t*)malloc(data_size);
    for (uint32_t i = 0; i < data->num_samples; i++) {
        int16_t sample = double_to_short(data->data_double[i]);
        for (uint16_t j = 0; j < data->num_channels; j++) {
            raw_data[i * data->num_channels + j] = sample; // Escreve o mesmo dado em todos os canais
        }
    }

    // Escreve o cabeçalho
    RiffHeader riff_header = { {'R', 'I', 'F', 'F'}, 36 + data_size, {'W', 'A', 'V', 'E'} };
    fwrite(&riff_header, sizeof(RiffHeader), 1, fp);

    ChunkHeader fmt_header = { {'f', 'm', 't', ' '}, 16 };
    FmtChunk fmt_chunk = {
        1, // PCM
        data->num_channels,
        data->sample_rate,
        data->sample_rate * data->num_channels * (data->bits_per_sample / 8),
        data->num_channels * (data->bits_per_sample / 8),
        data->bits_per_sample
    };
    fwrite(&fmt_header, sizeof(ChunkHeader), 1, fp);
    fwrite(&fmt_chunk, sizeof(FmtChunk), 1, fp);

    ChunkHeader data_header = { {'d', 'a', 't', 'a'}, data_size };
    fwrite(&data_header, sizeof(ChunkHeader), 1, fp);

    // Escreve os dados
    fwrite(raw_data, data_size, 1, fp);

    free(raw_data);
    fclose(fp);
}

void free_wav_data(WavData* data) {
    if (data) {
        free(data->data_double);
        free(data);
    }
}