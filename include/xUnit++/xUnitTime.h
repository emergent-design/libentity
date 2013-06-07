#ifndef XUNITTIME_H_
#define XUNITTIME_H_

#include <chrono>

namespace xUnitpp { namespace Time
{

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds Duration;
typedef std::chrono::duration<float> Seconds;
typedef decltype(Clock::now()) TimeStamp;

inline std::chrono::milliseconds ToMilliseconds(Duration time)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time);
}

inline std::chrono::milliseconds ToMilliseconds(int ms)
{
    return std::chrono::milliseconds(ms);
}

inline Seconds ToSeconds(Duration time)
{
    return std::chrono::duration_cast<Seconds>(time);
}

inline Seconds ToSeconds(int ms)
{
    return Seconds(ms);
}

template<typename TDuration>
inline Duration ToDuration(TDuration time)
{
    return std::chrono::duration_cast<Duration>(time);
}

inline std::string to_string(Duration time)
{
    using std::to_string;

    auto ms = ToMilliseconds(time);

    if (ms.count() < 1)
    {
        return to_string(time.count()) + " nanoseconds";
    }

    if (ms.count() > 500)
    {
        return to_string(ToSeconds(time).count()) + " seconds";
    }

    return to_string(ms.count()) + " milliseconds";
}

}}

#endif
