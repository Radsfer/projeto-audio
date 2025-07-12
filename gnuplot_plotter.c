#include "gnuplot_plotter.h"
#include "fft.h"
#include <stdio.h>
#include <math.h>

void plot_signal_to_file(const char* filename, const WavData* data) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Erro ao criar arquivo de dados para plotagem");
        return;
    }
    for (uint32_t i = 0; i < data->num_samples; i++) {
        fprintf(fp, "%f %f\n", (double)i / data->sample_rate, data->data_double[i]);
    }
    fclose(fp);
}

void plot_spectrum_to_file(const char* filename, const Complex* spectrum, size_t fft_size, uint32_t sample_rate) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Erro ao criar arquivo de dados do espectro");
        return;
    }
    for (size_t i = 0; i <= fft_size / 2; i++) {
        double mag = sqrt(spectrum[i].real * spectrum[i].real + spectrum[i].imag * spectrum[i].imag);
        double freq = (double)i * sample_rate / fft_size;
        fprintf(fp, "%f %f\n", freq, mag);
    }
    fclose(fp);
}

// ATUALIZADO: Função modificada para aceitar e usar o parâmetro de zoom.
void invoke_gnuplot(const char* data_filename, const char* title, const char* xlabel, const char* ylabel, int is_log_scale, double zoom_duration_ms) {
    FILE* gnuplot_pipe = popen("gnuplot -persistent", "w");
    if (gnuplot_pipe) {
        // Estilos para deixar o gráfico mais bonito
        fprintf(gnuplot_pipe, "set title '%s' font ',14'\n", title);
        fprintf(gnuplot_pipe, "set xlabel '%s'\n", xlabel);
        fprintf(gnuplot_pipe, "set ylabel '%s'\n", ylabel);
        fprintf(gnuplot_pipe, "set grid\n"); // Adiciona um grid de fundo
        fprintf(gnuplot_pipe, "set zeroaxis\n"); // Desenha uma linha no eixo zero

        // LÓGICA DO ZOOM: Se um tempo de zoom foi dado...
        if (zoom_duration_ms > 0) {
            // ...converte de milissegundos para segundos e define o range do eixo X.
            double zoom_seconds = zoom_duration_ms / 1000.0;
            fprintf(gnuplot_pipe, "set xrange [0:%f]\n", zoom_seconds);
        }

        if (is_log_scale) {
            fprintf(gnuplot_pipe, "set logscale y\n");
        }

        // PLOT MELHORADO: Linha mais grossa (lw 2) e cor azul (lc 'blue')
        fprintf(gnuplot_pipe, "plot '%s' with lines lw 2 lc 'blue' title 'Sinal'\n", data_filename);
        fflush(gnuplot_pipe);
        pclose(gnuplot_pipe);
        printf("Gráfico gerado. Feche a janela do gráfico para continuar...\n");
    } else {
        fprintf(stderr, "Gnuplot não encontrado. Verifique se ele está instalado e no seu PATH.\n");
    }
}