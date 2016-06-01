#ifndef TURNCHANGE_H
#define TURNCHANGE_H

class TurnChange
{
public:
    virtual void SwitchTurn() = 0;
    virtual bool IsThereLiveTownshipWaiting() = 0;
};


#endif // TURNCHANGE_H
