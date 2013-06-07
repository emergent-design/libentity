#ifndef EVENTLEVEL_H_
#define EVENTLEVEL_H_

#include <string>

namespace xUnitpp
{
    enum class EventLevel
    {
        Debug,
        Info,
        Warning,
        Check,
        Assert,
        Fatal
    };

    const std::string &to_string(EventLevel level);
}

#endif
