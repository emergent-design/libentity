#ifndef EXPORTAPI_H_
#define EXPORTAPI_H_

#include <functional>
#include <memory>
#include <vector>

#if !defined(WIN32)
# define __declspec(x)
#endif

namespace xUnitpp
{
    struct IOutput;
    struct ITestDetails;

    typedef std::function<void(const ITestDetails &)> EnumerateTestDetailsCallback;
    typedef void(*EnumerateTestDetails)(EnumerateTestDetailsCallback callback);

    typedef std::function<bool(const ITestDetails &)> TestFilterCallback;
    typedef int(*FilteredTestsRunner)(int, int, IOutput &, TestFilterCallback);
}

#endif
