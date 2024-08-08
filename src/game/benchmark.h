#pragma once

#include <SDL2/SDL_stdinc.h>

typedef struct {
    Uint64 start;
} BENCHMARK;

BENCHMARK *Benchmark_Start(void);

void Benchmark_End(BENCHMARK *b, const char *message);
