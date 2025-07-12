#ifndef WAV_HANDLER_H
#define WAV_HANDLER_H

#include <stdint.h>
#include <stdio.h>

// Estrutura para conter todos os dados e metadados de um arquivo WAV.
// Suporta apenas PCM de 16 bits, mono ou estéreo.
typedef struct {
    // --- Metadados do Cabeçalho ---
    uint32_t sample_rate;     // Taxa de amostragem (ex: 44100)
    uint16_t num_channels;    // Número de canais (1 para mono, 2 para estéreo)
    uint16_t bits_per_sample; // Bits por amostra (ex: 16)
    uint32_t data_size;       // Tamanho total do chunk de dados em bytes

    // --- Dados de Áudio ---
    uint32_t num_samples;     // Número de amostras POR CANAL
    double* data_double;      // Ponteiro para os dados de áudio normalizados para [-1.0, 1.0]
} WavData;

WavData* read_wav_file(const char* filename);

// A declaração aqui deve ser IGUAL à definição no .c, incluindo o 'const'.
void write_wav_file(const char* filename, const WavData* data);

void free_wav_data(WavData* data);

#endif //WAV_HANDLER_H