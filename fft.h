#ifndef PROJETO_AUDIO_FFT_H
#define PROJETO_AUDIO_FFT_H

#include <stddef.h>

// Estrutura para representar um número complexo
typedef struct {
    double real;
    double imag;
} Complex;

void fft(Complex* data, size_t n);
void ifft(Complex* data, size_t n);

#endif //PROJETO_AUDIO_FFT_H