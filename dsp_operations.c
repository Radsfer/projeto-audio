#include "dsp_operations.h"
#include "fft.h" // ADICIONADO: Para conhecer 'Complex', 'fft' e 'ifft'
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Encontra a próxima potência de 2, necessário para a FFT Radix-2.
size_t find_next_power_of_2(size_t n) {
    size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

WavData* mix_audio(const WavData* wav1, const WavData* wav2) {
    uint32_t max_samples = (wav1->num_samples > wav2->num_samples) ? wav1->num_samples : wav2->num_samples;

    WavData* mixed_wav = (WavData*)malloc(sizeof(WavData));

    mixed_wav->sample_rate = wav1->sample_rate;
    mixed_wav->num_channels = wav1->num_channels;
    mixed_wav->bits_per_sample = wav1->bits_per_sample;
    mixed_wav->num_samples = max_samples;
    mixed_wav->data_size = max_samples * mixed_wav->num_channels * (mixed_wav->bits_per_sample / 8);

    mixed_wav->data_double = (double*)calloc(max_samples, sizeof(double));

    for (uint32_t i = 0; i < max_samples; i++) {
        double s1 = (i < wav1->num_samples) ? wav1->data_double[i] : 0.0;
        double s2 = (i < wav2->num_samples) ? wav2->data_double[i] : 0.0;
        mixed_wav->data_double[i] = s1 + s2;
    }

    double max_abs_val = 0.0;
    for (uint32_t i = 0; i < max_samples; i++) {
        if (fabs(mixed_wav->data_double[i]) > max_abs_val) {
            max_abs_val = fabs(mixed_wav->data_double[i]);
        }
    }

    if (max_abs_val > 1.0) {
        double norm_factor = 1.0 / max_abs_val;
        for (uint32_t i = 0; i < max_samples; i++) {
            mixed_wav->data_double[i] *= norm_factor;
        }
    }
    return mixed_wav;
}

void apply_fft_filter(WavData* wav_data, double cutoff_freq, int is_high_pass) {
    size_t original_size = wav_data->num_samples;
    size_t fft_size = find_next_power_of_2(original_size);

    Complex* spectrum = (Complex*)calloc(fft_size, sizeof(Complex));
    for(size_t i = 0; i < original_size; i++) {
        spectrum[i].real = wav_data->data_double[i];
        spectrum[i].imag = 0.0;
    }

    fft(spectrum, fft_size);

    for (size_t k = 0; k < fft_size / 2 + 1; k++) {
        double freq = (double)k * wav_data->sample_rate / fft_size;
        int should_zero = 0;
        if (is_high_pass) {
            if (freq < cutoff_freq) should_zero = 1;
        } else {
            if (freq > cutoff_freq) should_zero = 1;
        }

        if (should_zero) {
            spectrum[k].real = 0;
            spectrum[k].imag = 0;
            if (k > 0 && k < fft_size / 2) {
                spectrum[fft_size - k].real = 0;
                spectrum[fft_size - k].imag = 0;
            }
        }
    }

    ifft(spectrum, fft_size);

    for (size_t i = 0; i < original_size; i++) {
        wav_data->data_double[i] = spectrum[i].real;
    }

    free(spectrum);
}

void apply_sma_filter(WavData* wav_data, int window_size) {
    if (window_size <= 1) return;

    double* temp_buffer = (double*)malloc(wav_data->num_samples * sizeof(double));
    double sum = 0.0;

    for (int i = 0; i < window_size; i++) {
        if (i < wav_data->num_samples) {
             sum += wav_data->data_double[i];
        }
    }

    for (uint32_t i = 0; i < wav_data->num_samples; i++) {
        if (i >= window_size) {
            sum += wav_data->data_double[i] - wav_data->data_double[i - window_size];
        }
        temp_buffer[i] = sum / window_size;
    }

    memcpy(wav_data->data_double, temp_buffer, wav_data->num_samples * sizeof(double));
    free(temp_buffer);
}

Complex* get_spectrum(const WavData* wav_data, size_t* fft_size_out) {
    size_t original_size = wav_data->num_samples;
    size_t fft_size = find_next_power_of_2(original_size);
    *fft_size_out = fft_size;

    Complex* spectrum = (Complex*)calloc(fft_size, sizeof(Complex));
    for(size_t i = 0; i < original_size; i++) {
        spectrum[i].real = wav_data->data_double[i];
        spectrum[i].imag = 0.0;
    }

    fft(spectrum, fft_size);
    return spectrum;
}

