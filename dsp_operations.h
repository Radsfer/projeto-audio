#ifndef DSP_OPERATIONS_H
#define DSP_OPERATIONS_H

#include "wav_handler.h"
#include "fft.h"

// Corrigido: Adicionado 'const' para combinar com o arquivo .c
WavData* mix_audio(const WavData* wav1, const WavData* wav2);

void apply_fft_filter(WavData* wav_data, double cutoff_freq, int is_high_pass);
void apply_sma_filter(WavData* wav_data, int window_size);
size_t find_next_power_of_2(size_t n);
Complex* get_spectrum(const WavData* wav_data, size_t* fft_size);

#endif //DSP_OPERATIONS_H