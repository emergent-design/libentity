#ifndef XUNITCHECK_H_
#define XUNITCHECK_H_

#include <functional>
#include <map>
#include <memory>
#include <thread>
#include "xUnitAssert.h"

namespace xUnitpp
{

class TestEventRecorder;

class Check : public Assert
{
public:
    Check(const TestEventRecorder &recorder);

private:
    Check(const Check &) /* = delete */;
    Check(Check &&) /* = delete */;
    Check &operator =(Check) /* = delete */;
};

}

#endif
