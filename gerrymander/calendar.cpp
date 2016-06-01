#include "calendar.h"
#include "district.h"
#include <QPainter>
#include <QMouseEvent>
#include <math.h>

Calendar::Calendar(QWidget *parent):QLabel(parent), board1Progress(0), board2Progress(1), turn(0), patchCount(0)
{
    setupProgressArray();
    DrawBoard();

    patchStack.push(56);
    patchStack.push(49);
    patchStack.push(36);
    patchStack.push(29);
    patchStack.push(22);
}

void Calendar::SetBoard1(TimeControl* b1)
{
    board1 = b1;
    if (board1 != nullptr)
    {
        using namespace std::placeholders;
        board1->SetTimeCallback(std::bind(&Calendar::SetBoard1Progress, this, _1));
    }
}

void Calendar::SetBoard2(TimeControl* b2)
{
    board2 = b2;
    if (board2 != nullptr)
    {
        using namespace std::placeholders;
        board2->SetTimeCallback(std::bind(&Calendar::SetBoard2Progress, this, _1));
    }
}

void Calendar::DrawBoard()
{
    QImage image(TRACK_BOARD_SIZE*TRACK_SQUARE_DRAW_SIZE, TRACK_BOARD_SIZE*TRACK_SQUARE_DRAW_SIZE, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor whiteBrush(0, 0, 0, 0);
    painter.setBrush(whiteBrush);

    for (int x=0; x<TRACK_BOARD_SIZE; ++x)
    {
        for (int y=0; y<TRACK_BOARD_SIZE; ++y)
        {
            painter.drawRect(QRectF(x*TRACK_SQUARE_DRAW_SIZE, y*TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE));
        }
    }

    QColor darkBrush(0, 128, 128, 128);
    painter.setBrush(darkBrush);

    painter.drawRect(QRectF(0, 0, 3*TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE));
    painter.drawRect(QRectF(3*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE));

    QColor coinBrush(0, 0, 0, 128);
    painter.setBrush(coinBrush);
    painter.drawEllipse(6.625*TRACK_SQUARE_DRAW_SIZE, 5, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(7.125*TRACK_SQUARE_DRAW_SIZE, 4.625*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(2.625*TRACK_SQUARE_DRAW_SIZE, 7.125*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(0.125*TRACK_SQUARE_DRAW_SIZE, 2.625*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(5.625*TRACK_SQUARE_DRAW_SIZE, 1.125*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(4.625*TRACK_SQUARE_DRAW_SIZE, 6.125*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(1.125*TRACK_SQUARE_DRAW_SIZE, 3.625*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(5.125*TRACK_SQUARE_DRAW_SIZE, 3.625*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);
    painter.drawEllipse(2.625*TRACK_SQUARE_DRAW_SIZE, 3.125*TRACK_SQUARE_DRAW_SIZE, COIN_SIZE, COIN_SIZE);

    QPen pen;
    pen.setWidth(10);
    pen.setBrush(Qt::black);
    painter.setPen(pen);

    painter.drawLine(0, TRACK_SQUARE_DRAW_SIZE, (TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine((TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE, (TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE, (TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine((TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE, (TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE, (TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(TRACK_SQUARE_DRAW_SIZE, (TRACK_BOARD_SIZE-1)*TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE, 6*TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(6*TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE, 6*TRACK_SQUARE_DRAW_SIZE, 6*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(6*TRACK_SQUARE_DRAW_SIZE, 6*TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE, 6*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(2*TRACK_SQUARE_DRAW_SIZE, 6*TRACK_SQUARE_DRAW_SIZE, 2*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(2*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE, 5*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(5*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE, 5*TRACK_SQUARE_DRAW_SIZE, 5*TRACK_SQUARE_DRAW_SIZE);
    painter.drawLine(5*TRACK_SQUARE_DRAW_SIZE, 5*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE, 5*TRACK_SQUARE_DRAW_SIZE);

    painter.drawLine(3*TRACK_SQUARE_DRAW_SIZE, 5*TRACK_SQUARE_DRAW_SIZE, 3*TRACK_SQUARE_DRAW_SIZE, 4*TRACK_SQUARE_DRAW_SIZE);

    QPen pen2;
    pen.setWidth(1);
    pen.setBrush(Qt::black);
    painter.setPen(pen2);

    int px1 = progress[board1Progress].x()*TRACK_SQUARE_DRAW_SIZE;
    int py1 = progress[board1Progress].y()*TRACK_SQUARE_DRAW_SIZE;

    QColor redBrush(255, 0, 0, 255);
    painter.setBrush(redBrush);
    painter.drawEllipse(px1, py1, TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE);

    int px2 = progress[board2Progress].x()*TRACK_SQUARE_DRAW_SIZE;
    int py2 = progress[board2Progress].y()*TRACK_SQUARE_DRAW_SIZE;

    QColor blueBrush(0, 0, 255, 255);
    painter.setBrush(blueBrush);
    painter.drawEllipse(px2, py2, TRACK_SQUARE_DRAW_SIZE, TRACK_SQUARE_DRAW_SIZE);

    painter.end();

    setPixmap(QPixmap::fromImage(image));
}

void Calendar::setupProgressArray()
{
    progress[0] = QPoint(0, 0);
    //progress array holds the next coordinate on the grid to travel from the current position
    progress[1] = QPoint(1, 0);
    progress[2] = QPoint(2, 0);
    progress[3] = QPoint(3, 0); //game starts here
    progress[4] = QPoint(4, 0);
    progress[5] = QPoint(5, 0);
    progress[6] = QPoint(6, 0); //b1
    progress[7] = QPoint(7, 0);

    progress[8] = QPoint(7, 1);
    progress[9] = QPoint(7, 2);
    progress[10] = QPoint(7, 3);
    progress[11] = QPoint(7, 4);
    progress[12] = QPoint(7, 5); //b2
    progress[13] = QPoint(7, 6);
    progress[14] = QPoint(7, 7);

    progress[15] = QPoint(6, 7);
    progress[16] = QPoint(5, 7);
    progress[17] = QPoint(4, 7);
    progress[18] = QPoint(3, 7); //b3
    progress[19] = QPoint(2, 7);
    progress[20] = QPoint(1, 7);
    progress[21] = QPoint(0, 7);

    progress[22] = QPoint(0, 6);
    progress[23] = QPoint(0, 5);
    progress[24] = QPoint(0, 4);
    progress[25] = QPoint(0, 3); //b4
    progress[26] = QPoint(0, 2);
    progress[27] = QPoint(0, 1);

    progress[28] = QPoint(1, 1);
    progress[29] = QPoint(2, 1);
    progress[30] = QPoint(3, 1);
    progress[31] = QPoint(4, 1);
    progress[32] = QPoint(5, 1); //b5
    progress[33] = QPoint(6, 1);

    progress[34] = QPoint(6, 2);
    progress[35] = QPoint(6, 3);
    progress[36] = QPoint(6, 4);
    progress[37] = QPoint(6, 5);
    progress[38] = QPoint(6, 6);

    progress[39] = QPoint(5, 6); //b6
    progress[40] = QPoint(4, 6);
    progress[41] = QPoint(3, 6);
    progress[42] = QPoint(2, 6);
    progress[43] = QPoint(1, 6);

    progress[44] = QPoint(1, 5);
    progress[45] = QPoint(1, 4); //b7
    progress[46] = QPoint(1, 3);
    progress[47] = QPoint(1, 2);

    progress[48] = QPoint(2, 2);
    progress[49] = QPoint(3, 2);
    progress[50] = QPoint(4, 2);
    progress[51] = QPoint(5, 2);

    progress[52] = QPoint(5, 3); //b8
    progress[53] = QPoint(5, 4);
    progress[54] = QPoint(5, 5);

    progress[55] = QPoint(4, 5);
    progress[56] = QPoint(3, 5);
    progress[57] = QPoint(2, 5);

    progress[58] = QPoint(2, 4);
    progress[59] = QPoint(2, 3); //b9

    progress[60] = QPoint(3, 3);
    progress[61] = QPoint(3, 4);
    progress[62] = QPoint(4, 3);
    progress[63] = QPoint(4, 4);
    progress[64] = QPoint(4, 4);

}

void Calendar::mousePressEvent(QMouseEvent *event)
{
    //don't allow further movement after finishing the track
    if ((turn == 0 && board1Progress > 59) || (turn == 1 && board2Progress > 59))
    {
        return;
    }

    //if you're already ahead, no more moving (can happen if a turn is extended by opening a patch
    if (IsCurrentBoardAhead(turn))
    {
        return;
    }

    //TODO: limit this by turn and by max jump from current position!
    auto selectedSpot = event->pos();
    int selectedX = floor(((double)(selectedSpot.x()))/((double)TRACK_SQUARE_DRAW_SIZE));
    int selectedY = floor(((double)(selectedSpot.y()))/((double)TRACK_SQUARE_DRAW_SIZE));

    int i=0;
    for (; i<65; ++i)
    {
        if (progress[i].x() == selectedX && progress[i].y() == selectedY)
        {
            if (turn == 0)
            {
                if (i - board1Progress <= 0) //no moving backwards
                {
                    return;
                }
                int delta = (board1Progress < 3)?i - 2:i - board1Progress;
                board1->AddTimeExternal(delta);
                board1Progress = i;
            }
            else
            {
                if (i - board2Progress <= 0)
                {
                    return;
                }
                int delta = (board2Progress < 3)?i - 2:i - board2Progress;
                board2->AddTimeExternal(delta);
                board2Progress = i;
            }
            break;
        }
    }
    if (i<65)
    {
        DrawBoard();
    }

}

bool Calendar::IsCurrentBoardAhead(int current)
{
    if (current == 0 && board1Progress > board2Progress)
    {
        return true;
    }
    else if (current == 1 && board2Progress > board1Progress)
    {
        return true;
    }
    return false;
}

void Calendar::enablePatches()
{
    while (!patchStack.empty() && (patchStack.top() <= board1Progress || patchStack.top() <= board2Progress))
    {
        patchStack.pop();
        auto patch = patchList->at(patchCount++);
        patch->SetLivePatch(true);
        patch->SetHighlight(true);
        patch->move(915, 750);
        patch->DrawPiece();
    }
}

