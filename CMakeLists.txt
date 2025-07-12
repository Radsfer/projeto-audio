cmake_minimum_required(VERSION 3.10)
project(projeto_audio C)

set(CMAKE_C_STANDARD 99)

add_executable(projeto_audio
        main.c
        wav_handler.c
        fft.c
        dsp_operations.c
        gnuplot_plotter.c
        wav_handler.h
        wav_handler.c
        fft.h
        fft.c
        dsp_operations.h
        dsp_operations.c
        gnuplot_plotter.h
        gnuplot_plotter.c
)

# Linka a biblioteca matemática (libm) para funções como sin, cos, sqrt, etc.
target_link_libraries(projeto_audio m)