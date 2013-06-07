#ifndef TESTEVENTRECORDER_H_
#define TESTEVENTRECORDER_H_

#include <functional>
#include <map>
#include <mutex>
#include <thread>

namespace xUnitpp
{

class TestEvent;

class TestEventRecorder
{
public:
    void Tie(std::function<void(TestEvent &&)> sink);
    void operator()(TestEvent &&evt) const;

private:
    mutable std::mutex lock;
    mutable std::map<std::thread::id, std::function<void(TestEvent &&)>> sinks;
};

}

#endif
