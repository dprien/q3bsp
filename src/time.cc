#include <unistd.h>

#include <SDL2/SDL.h>

#include "time.h"

#if _POSIX_VERSION >= 200101
#include <time.h>

std::uint64_t get_ticks()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * TICKS_PER_SECOND + t.tv_nsec;
}

void sleep_ticks(std::uint64_t ticks)
{
    static std::uint64_t overhead = 0;
    if (overhead > ticks) {
        return;
    }
    std::uint64_t cticks = ticks - overhead;

    struct timespec t;
    t.tv_sec = cticks / TICKS_PER_SECOND;
    t.tv_nsec = cticks % TICKS_PER_SECOND;

    std::uint64_t tstart = get_ticks();
    while (clock_nanosleep(CLOCK_MONOTONIC, 0, &t, &t)) {
    }
    std::uint64_t tdiff = get_ticks() - tstart;
    if (tdiff > cticks) {
        overhead = tdiff - cticks;
    }
    else {
        overhead = 0;
    }
}
#else
std::uint64_t get_ticks()
{
    return SDL_GetTicks() * 1000000ULL;
}

void sleep_ticks(std::uint64_t ticks)
{
    SDL_Delay(ticks / 1000000ULL);
}
#endif

float TickQueue::new_frame(const int fps)
{
    std::uint64_t last = m_queue.back();

    if (fps) {
        std::uint64_t ticks_per_frame = TICKS_PER_SECOND / fps;
        std::uint64_t delta = get_ticks() - last;
        if (delta < ticks_per_frame) {
            sleep_ticks(ticks_per_frame - delta);
        }
    }

    m_queue.push_back(get_ticks());
    ++m_frames;
    while (m_queue.size() > 2 && get_cum_ticks() > TICKS_PER_SECOND) {
        m_queue.pop_front();
        --m_frames;
    }

    return (get_ticks() - last) / float(TICKS_PER_SECOND);
}
