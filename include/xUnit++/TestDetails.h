#ifndef TESTDETAILS_H_
#define TESTDETAILS_H_

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include "Attributes.h"
#include "LineInfo.h"
#include "ITestDetails.h"
#include "xUnitTime.h"

namespace xUnitpp
{

struct TestDetails : public ITestDetails
{
    TestDetails();
    TestDetails(std::string &&name, int testInstance, std::string &&params, const std::string &suite,
        AttributeCollection &&attributes, Time::Duration timeLimit,
        std::string &&filename, int line);

    // ITestDetails implementation
    virtual int __stdcall GetId() const override;
    virtual const char * __stdcall GetName() const override;
    virtual const char * __stdcall GetFullName() const override;
    virtual const char * __stdcall GetSuite() const override;
    virtual const char * __stdcall GetParams() const override;
    virtual int __stdcall GetTestInstance() const override;
    virtual size_t __stdcall GetAttributeCount() const override;
    virtual const char * __stdcall GetAttributeKey(size_t index) const override;
    virtual const char * __stdcall GetAttributeValue(size_t index) const override;
    virtual void __stdcall FindAttributeKey(const char *key, size_t &begin, size_t &end) const override;
    virtual const char * __stdcall GetFile() const override;
    virtual int __stdcall GetLine() const override;

    int Id;
    int TestInstance;
    std::string Name;
    std::string Params;
    std::string FullName;   // name + params
    std::string Suite;
    AttributeCollection Attributes;
    Time::Duration TimeLimit;
    xUnitpp::LineInfo LineInfo;
};

}

#endif
