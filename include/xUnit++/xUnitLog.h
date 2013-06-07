#ifndef XUNITLOG_H_
#define XUNITLOG_H_

#include <functional>
#include <memory>
#include <sstream>
#include "TestEvent.h"

namespace xUnitpp
{

class TestEventRecorder;

class Log
{
    class Logger
    {
        class Message
        {
        public:
            Message(std::function<void(const std::string &, const LineInfo &)> recordMessage, const LineInfo &lineInfo = LineInfo());
            Message(const Message &other);
// !!!g++ why does `= default` cause xUnitLog.cpp to fail to compile?
/*
#if !defined(_MSC_VER) // !!!VS remove the #if/#endif when VS can compile this code
            Message(Message &&) = default;
#endif
*/

            ~Message();

            template<typename T>
            Message &operator <<(const T &value)
            {
                message << value;
                return *this;
            }

        private:
            Message &operator =(Message) /* = delete */;

        private:
            size_t &refCount;
            std::function<void(const std::string &, const LineInfo &)> recordMessage;
            std::stringstream message;
            LineInfo lineInfo;
        };
    public:
        Logger(std::function<void(const std::string &, const LineInfo &)> recordMessage);

        template<typename T>
        Message operator <<(const T &value) const
        {
            return (Message(recordMessage) << value);
        }

        Message operator()(const LineInfo &lineInfo) const;

    private:
        std::function<void(const std::string &, const LineInfo &)> recordMessage;
    };

public:
    Log(const TestEventRecorder &recorder);

    const Logger Debug;
    const Logger Info;
    const Logger Warn;

private:
    Log(const Log &) /* = delete */;
    Log(Log &&) /* = delete */;
    Log &operator =(Log) /* = delete */;
};

}

#endif
