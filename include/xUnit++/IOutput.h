#ifndef IOUTPUT_H_
#define IOUTPUT_H_

// !!!VS remove the #if/#endif when VS can compile this code
#if defined(_MSC_VER)
# define DEFAULT {}
#else
# define DEFAULT = default;
#endif

#include <string>
#include "xUnitTime.h"

namespace xUnitpp
{

struct ITestDetails;
struct ITestEvent;

struct IOutput
{
    virtual ~IOutput() DEFAULT

    virtual void __stdcall ReportStart(const ITestDetails &testDetails) = 0;
    virtual void __stdcall ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt) = 0;
    virtual void __stdcall ReportSkip(const ITestDetails &testDetails, const char *reason) = 0;
    virtual void __stdcall ReportFinish(const ITestDetails &testDetails, long long ns) = 0;
    virtual void __stdcall ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, long long nsTotal) = 0;
};

}

#endif
