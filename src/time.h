#ifndef Q3BSP__TIME_H
#define Q3BSP__TIME_H

#include <deque>
#include <cstdint>

#define TICKS_PER_SECOND 1000000000

extern std::uint64_t get_ticks();
extern void sleep_ticks(std::uint64_t);

class TickQueue
{
    public:
        TickQueue()
            : m_frames(0)
        {
            m_queue.push_back(get_ticks());
        }

        std::uint64_t get_cum_ticks() const
        {
            return m_queue.back() - m_queue.front();
        }

        float get_seconds() const
        {
            return get_cum_ticks() / float(TICKS_PER_SECOND);
        };

        std::uint64_t get_frames() const
        {
            return m_frames;
        }

        std::uint64_t get_ticks_per_frame() const
        {
            return get_cum_ticks() / m_frames;
        }

        float get_seconds_per_frame() const
        {
            return get_seconds() / m_frames;
        }

        float get_frames_per_second() const
        {
            return m_frames / get_seconds();
        }

        float new_frame(const int = 0);

    private:
        std::deque<std::uint64_t>   m_queue;
        std::uint64_t               m_frames;
};

#endif
