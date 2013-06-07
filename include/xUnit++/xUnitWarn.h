#ifndef WARN_H_
#define WARN_H_

#include "xUnitAssert.h"

namespace xUnitpp
{

class TestEventRecorder;

class Warn : public Assert
{
public:
    Warn(const TestEventRecorder &recorder);

private:
    Warn(const Warn &) /* = delete */;
    Warn(Warn &&) /* = delete */;
    Warn &operator =(Warn) /* = delete */;
};

}

#endif
