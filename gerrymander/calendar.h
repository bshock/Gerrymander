#ifndef CALENDAR_H
#define CALENDAR_H
#include <QLabel>
#include <QPoint>
#include "timecontrol.h"
#include "district.h"
#include <vector>
#include <stack>

class Calendar : public QLabel
{
public:
    static const int TRACK_BOARD_SIZE = 8;
    static const int TRACK_SQUARE_DRAW_SIZE = 50;
    static const int COIN_SIZE = 37;
private:
    QPoint progress[65];
    int board1Progress;
    int board2Progress;

    TimeControl *board1;
    TimeControl *board2;

    int turn;

    std::vector<District*> *patchList;
    std::stack<int> patchStack;
    int patchCount;
public:
    Calendar(QWidget *parent);

    void SetBoard1(TimeControl* b1);
    void SetBoard2(TimeControl* b2);

    void SetBoard1Progress(int time)
    {
        board1Progress = time;
        enablePatches();
        DrawBoard();
    }
    void SetBoard2Progress(int time)
    {
        board2Progress = time;
        enablePatches();
        DrawBoard();
    }
    void SetTurn(int t) { turn = t; }

    void DrawBoard();

    bool IsCurrentBoardAhead(int current);

    void SetPatches(std::vector<District*> *plist) {patchList = plist;}

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void setupProgressArray();
    void enablePatches();
};

#endif // CALENDAR_H
