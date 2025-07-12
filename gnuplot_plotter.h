#ifndef PROJETO_AUDIO_GNUPLOT_PLOTTER_H
#define PROJETO_AUDIO_GNUPLOT_PLOTTER_H

#include "wav_handler.h"
#include "fft.h"

void plot_signal_to_file(const char* filename, const WavData* data);
void plot_spectrum_to_file(const char* filename, const Complex* spectrum, size_t fft_size, uint32_t sample_rate);

// ATUALIZADO: Adicionado parâmetro 'zoom_duration_ms' para controlar o zoom.
// Se for 0, mostra o sinal completo. Se for > 0, dá zoom nesse tempo em milissegundos.
void invoke_gnuplot(const char* data_filename, const char* title, const char* xlabel, const char* ylabel, int is_log_scale, double zoom_duration_ms);

#endif //PROJETO_AUDIO_GNUPLOT_PLOTTER_H