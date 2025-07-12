#include "fft.h"
#include <math.h>

// Função auxiliar para a permutação bit-reversal
static void bit_reverse_reorder(Complex* data, size_t n) {
    for (size_t i = 1, j = 0; i < n; i++) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            Complex temp = data[i];
            data[i] = data[j];
            data[j] = temp;
        }
    }
}

// Função principal que implementa o algoritmo Cooley-Tukey Radix-2
static void transform(Complex* data, size_t n, int inverse) {
    if (n == 0) return;

    bit_reverse_reorder(data, n);

    for (size_t len = 2; len <= n; len <<= 1) {
        double angle = 2 * M_PI / len * (inverse? -1 : 1);
        Complex wlen = {cos(angle), sin(angle)};
        for (size_t i = 0; i < n; i += len) {
            Complex w = {1.0, 0.0};
            for (size_t j = 0; j < len / 2; j++) {
                Complex u = data[i + j];
                Complex v = {
                    data[i + j + len / 2].real * w.real - data[i + j + len / 2].imag * w.imag,
                    data[i + j + len / 2].real * w.imag + data[i + j + len / 2].imag * w.real
                };
                data[i + j].real = u.real + v.real;
                data[i + j].imag = u.imag + v.imag;
                data[i + j + len / 2].real = u.real - v.real;
                data[i + j + len / 2].imag = u.imag - v.imag;

                Complex next_w = {
                    w.real * wlen.real - w.imag * wlen.imag,
                    w.real * wlen.imag + w.imag * wlen.real
                };
                w = next_w;
            }
        }
    }

    if (inverse) {
        for (size_t i = 0; i < n; i++) {
            data[i].real /= n;
            data[i].imag /= n;
        }
    }
}

void fft(Complex* data, size_t n) {
    transform(data, n, 0);
}

void ifft(Complex* data, size_t n) {
    transform(data, n, 1);
}