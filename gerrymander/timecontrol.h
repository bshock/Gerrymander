#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <functional>

class TimeControl
{
public:
    virtual void AddTimeExternal(int dist) = 0;
    virtual void SetTimeCallback(std::function<void(int)> timeCallback) = 0;
};

#endif // TIMECONTROL_H
