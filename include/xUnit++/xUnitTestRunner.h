#ifndef XUNITTESTRUNNER_H_
#define XUNITTESTRUNNER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ExportApi.h"
#include "xUnitTime.h"

namespace xUnitpp
{

struct IOutput;
struct TestDetails;
class xUnitTest;

int RunTests(IOutput &output, xUnitpp::TestFilterCallback filter, const std::vector<std::shared_ptr<xUnitTest>> &tests,
             Time::Duration maxTestRunTime, size_t maxConcurrent);

}

#endif
