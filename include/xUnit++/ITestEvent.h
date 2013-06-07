#ifndef ITESTEVENT_H_
#define ITESTEVENT_H_

// !!!VS remove the #if/#endif when VS can compile this code
#if defined(_MSC_VER)
# define DEFAULT {}
#else
# define DEFAULT = default;
#endif

namespace xUnitpp
{

enum class EventLevel;

struct ITestAssert
{
protected:
    virtual ~ITestAssert() DEFAULT

public:
    virtual const char * __stdcall GetCall() const = 0;
    virtual const char * __stdcall GetUserMessage() const = 0;
    virtual const char * __stdcall GetCustomMessage() const = 0;
    virtual const char * __stdcall GetExpected() const = 0;
    virtual const char * __stdcall GetActual() const = 0;
};

struct ITestEvent
{
protected:
    virtual ~ITestEvent() DEFAULT

public:
    virtual bool __stdcall GetIsAssertType() const = 0;
    virtual bool __stdcall GetIsFailure() const = 0;
    virtual EventLevel __stdcall GetLevel() const = 0;
    virtual const char * __stdcall GetMessage() const = 0;
    virtual const char * __stdcall GetToString() const = 0;
    virtual const char * __stdcall GetFile() const = 0;
    virtual int __stdcall GetLine() const = 0;

    virtual const ITestAssert & __stdcall GetAssertInterface() const = 0;
};

}

#endif
