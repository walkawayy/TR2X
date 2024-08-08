#include "game/benchmark.h"

#include <libtrx/log.h>
#include <libtrx/memory.h>

#include <SDL2/SDL_timer.h>

BENCHMARK *Benchmark_Start(void)
{
    BENCHMARK *const b = Memory_Alloc(sizeof(BENCHMARK));
    b->start = SDL_GetPerformanceCounter();
    return b;
}

void Benchmark_End(BENCHMARK *b, const char *message)
{
    const double elapsed = (double)(SDL_GetPerformanceCounter() - b->start)
        * 1000.0 / (double)SDL_GetPerformanceFrequency();
    LOG_INFO("%s: finished in %.02f ms", message, elapsed);
    Memory_FreePointer(&b);
}
