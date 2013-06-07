#ifndef TESTEVENT_H_
#define TESTEVENT_H_

#include <exception>
#include <string>
#include "ITestEvent.h"
#include "LineInfo.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

enum class EventLevel;

class TestEvent : public ITestEvent, public ITestAssert
{
public:
    TestEvent(EventLevel level, const std::string &message, const LineInfo &lineInfo = xUnitpp::LineInfo());
    TestEvent(EventLevel level, const xUnitAssert &assert);
    TestEvent(const std::exception &e);

    // ITestEvent implementation
    virtual bool __stdcall GetIsAssertType() const override;
    virtual bool __stdcall GetIsFailure() const override;
    virtual EventLevel __stdcall GetLevel() const override;
    virtual const char * __stdcall GetMessage() const override;
    virtual const char * __stdcall GetToString() const override;
    virtual const char * __stdcall GetFile() const override;
    virtual int __stdcall GetLine() const override;
    virtual const ITestAssert & __stdcall GetAssertInterface() const override;

    // ITestAssert implementation
    virtual const char * __stdcall GetCall() const override;
    virtual const char * __stdcall GetUserMessage() const override;
    virtual const char * __stdcall GetCustomMessage() const override;
    virtual const char * __stdcall GetExpected() const override;
    virtual const char * __stdcall GetActual() const override;

    const xUnitAssert &Assert() const;
    const std::string &Message() const;

    const xUnitpp::LineInfo &LineInfo() const;

    friend std::string to_string(const TestEvent &event);

private:
    EventLevel level;
    xUnitAssert assert;
    std::string message;
    xUnitpp::LineInfo lineInfo;

    mutable std::string toString;
};

}

#endif
