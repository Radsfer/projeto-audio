#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wav_handler.h"
#include "fft.h"
#include "dsp_operations.h"
#include "gnuplot_plotter.h"

void print_usage(const char* prog_name) {
    fprintf(stderr, "Uso:\n");
    fprintf(stderr, "  %s mix <in1.wav> <in2.wav> <out.wav>\n", prog_name);
    fprintf(stderr, "  %s filter-fft <low|high> <freq_corte_hz> <in.wav> <out.wav>\n", prog_name);
    fprintf(stderr, "  %s filter-sma <tamanho_janela> <in.wav> <out.wav>\n", prog_name);
    fprintf(stderr, "  %s plot-spectrum <in.wav>\n", prog_name);
    fprintf(stderr, "  %s plot-signal <in.wav>\n", prog_name);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char* command = argv[1];

    if (strcmp(command, "mix") == 0) {
        if (argc != 5) { print_usage(argv[0]); return 1; }
        WavData* wav1 = read_wav_file(argv[2]);
        WavData* wav2 = read_wav_file(argv[3]);
        if (!wav1 || !wav2) return 1;

        printf("Mixando '%s' e '%s'...\n", argv[2], argv[3]);
        WavData* mixed_wav = mix_audio(wav1, wav2);
        write_wav_file(argv[4], mixed_wav);
        printf("Arquivo mixado salvo em '%s'.\n", argv[4]);

        plot_signal_to_file("plot_data.dat", mixed_wav);
        // ATUALIZADO: Adicionado '20.0' para dar zoom de 20ms
        invoke_gnuplot("plot_data.dat", "Sinal Mixado", "Tempo (s)", "Amplitude", 0, 20.0);

        free_wav_data(wav1);
        free_wav_data(wav2);
        free_wav_data(mixed_wav);

    } else if (strcmp(command, "filter-fft") == 0) {
        if (argc != 6) { print_usage(argv[0]); return 1; }
        int is_high_pass = (strcmp(argv[2], "high") == 0);
        double cutoff = atof(argv[3]);
        WavData* wav = read_wav_file(argv[4]);
        if (!wav) return 1;

        printf("Aplicando filtro FFT %s-pass com corte em %.2f Hz...\n", is_high_pass ? "high" : "low", cutoff);
        apply_fft_filter(wav, cutoff, is_high_pass);
        write_wav_file(argv[5], wav);
        printf("Arquivo filtrado salvo em '%s'.\n", argv[5]);

        plot_signal_to_file("plot_data.dat", wav);
        // ATUALIZADO: Adicionado '20.0' para dar zoom de 20ms
        invoke_gnuplot("plot_data.dat", "Sinal Filtrado (FFT)", "Tempo (s)", "Amplitude", 0, 20.0);

        free_wav_data(wav);

    } else if (strcmp(command, "filter-sma") == 0) {
        if (argc != 5) { print_usage(argv[0]); return 1; }
        int window_size = atoi(argv[2]);
        WavData* wav = read_wav_file(argv[3]);
        if (!wav) return 1;

        printf("Aplicando filtro de Média Móvel com janela de %d amostras...\n", window_size);
        apply_sma_filter(wav, window_size);
        write_wav_file(argv[4], wav);
        printf("Arquivo filtrado salvo em '%s'.\n", argv[4]);

        plot_signal_to_file("plot_data.dat", wav);
        // ATUALIZADO: Adicionado '20.0' para dar zoom de 20ms
        invoke_gnuplot("plot_data.dat", "Sinal Filtrado (Média Móvel)", "Tempo (s)", "Amplitude", 0, 20.0);

        free_wav_data(wav);

    } else if (strcmp(command, "plot-spectrum") == 0) {
        if (argc != 3) { print_usage(argv[0]); return 1; }
        WavData* wav = read_wav_file(argv[2]);
        if (!wav) return 1;

        printf("Calculando e plotando espectro de '%s'...\n", argv[2]);
        size_t fft_size;
        Complex* spectrum = get_spectrum(wav, &fft_size);
        plot_spectrum_to_file("spectrum_data.dat", spectrum, fft_size, wav->sample_rate);
        // ATUALIZADO: Adicionado '0' para NÃO dar zoom no espectro
        invoke_gnuplot("spectrum_data.dat", "Espectro de Frequência", "Frequência (Hz)", "Magnitude", 1, 0);

        free(spectrum);
        free_wav_data(wav);

    } else if (strcmp(command, "plot-signal") == 0) {
        if (argc != 3) { print_usage(argv[0]); return 1; }
        WavData* wav = read_wav_file(argv[2]);
        if (!wav) return 1;

        printf("Plotando o sinal de '%s' no domínio do tempo...\n", argv[2]);
        plot_signal_to_file("plot_data.dat", wav);

        char plot_title[256];
        snprintf(plot_title, sizeof(plot_title), "Sinal de %s", argv[2]);
        // ATUALIZADO: Adicionado '20.0' para dar zoom de 20ms
        invoke_gnuplot("plot_data.dat", plot_title, "Tempo (s)", "Amplitude", 0, 20.0);

        free_wav_data(wav);
    }
    else {
        fprintf(stderr, "Comando desconhecido: %s\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}