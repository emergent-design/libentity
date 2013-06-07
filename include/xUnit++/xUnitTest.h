#ifndef XUNITTEST_H_
#define XUNITTEST_H_

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "TestDetails.h"
#include "TestEvent.h"
#include "xUnitTime.h"

namespace xUnitpp
{

class AttributeCollection;
class TestEvent;
class TestEventRecorder;

enum class TestResult
{
    Success,
    Failure
};

class xUnitTest
{
public:
    xUnitTest(std::function<void()> &&test, std::string &&name, int testInstance, std::string &&params,
        const std::string &suite, AttributeCollection &&attributes, Time::Duration timeLimit,
        std::string &&filename, int line, const std::vector<std::shared_ptr<TestEventRecorder>> &testEventRecorders);

    const xUnitpp::TestDetails &TestDetails() const;

    TestResult Run();
    Time::Duration Duration() const;

    void AddEvent(TestEvent &&evt);
    const std::vector<TestEvent> &TestEvents() const;

private:
    xUnitTest(const xUnitTest &other) /* = delete */;
    xUnitTest(xUnitTest &&other) /* = delete */;
    xUnitTest &operator =(xUnitTest other) /* = delete */;

private:
    std::function<void()> test;
    xUnitpp::TestDetails testDetails;

    Time::TimeStamp testStart;
    Time::TimeStamp testStop;

    std::vector<std::shared_ptr<TestEventRecorder>> testEventRecorders;

    std::mutex eventLock;
    std::vector<TestEvent> testEvents;
    bool failureEventLogged;
};

}

#endif
