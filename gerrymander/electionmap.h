#ifndef ELECTIONMAP_H
#define ELECTIONMAP_H
#include <QLabel>
#include <QPoint>
#include "piecepasser.h"
#include "timecontrol.h"
#include <stack>
#include <functional>
#include "district.h"
#include "turnchange.h"

class ElectionMap : public QLabel, public PiecePasser, public TimeControl
{
    static const int MAX_BOARD_SIZE = 9;
    int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

    int tempBackers;
    int tempCost;
    int tempTime;

    int totalBackers;
    int totalMoney;
    int totalTime;
    int tmpShape[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE];
    QPoint position;
    std::stack<int> coinMultipliers;
    std::function<void(int)> TimeCallback;
    TurnChange *TurnChangeObject;
    int boardNumber;
    bool turnOn;
public:
    ElectionMap(QWidget *parent);
    void DrawBoard();
    bool Intersect(const QPoint& point);
    bool PlacePiece(District& piece, const QPoint &offset);
    //always use this function to add time, so it can be registered as profit

    virtual void AddTimeExternal(int time);
    virtual void SetTimeCallback(std::function<void(int)> timeCallback) { TimeCallback = timeCallback; }

    void SetTurnChanger(TurnChange* turnChangeObject) {TurnChangeObject = turnChangeObject; }
    void SetTurn(bool t);
    void SetBoardNumber(int bn) { boardNumber = bn; }
    bool IsBoardFinished() { return (totalTime > 59); }

    bool IsSolid7x7();

    virtual void Pass(const int shape[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE], int bkrs, int cst, int tm);

private:
    bool DoesTmpShapeFit(const QPoint &offset);
    void ZeroTmpShape();
    void zeroBoard();
    void AddTimeInternal(int time);
    int GetEmptySpaces() const;
    bool FoundSolid7x7(int offsetX, int offsetY);
};

#endif // ELECTIONMAP_H
