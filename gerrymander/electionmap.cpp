#include "electionmap.h"
#include <QPainter>
#include <QDebug>

ElectionMap::ElectionMap(QWidget *parent) : QLabel(parent), totalBackers(0), totalMoney(5), totalTime(0), boardNumber(0), turnOn(false)
{
    zeroBoard();
    DrawBoard();

    coinMultipliers.push(59);
    coinMultipliers.push(52);
    coinMultipliers.push(45);
    coinMultipliers.push(39);
    coinMultipliers.push(32);
    coinMultipliers.push(25);
    coinMultipliers.push(18);
    coinMultipliers.push(11);
    coinMultipliers.push(6);
}

void ElectionMap::DrawBoard()
{
    QImage image(MAX_BOARD_SIZE*District::SQUARE_DRAW_SIZE, (MAX_BOARD_SIZE+2)*District::SQUARE_DRAW_SIZE, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    if (turnOn)
    {
        QColor redBrush(255, 0, 0, 255);
        QColor blueBrush(0, 0, 255, 255);
        if (boardNumber == 0)
        {
            painter.setBrush(redBrush);
        }
        else
        {
            painter.setBrush(blueBrush);
        }
        painter.drawRect(QRectF(0, 0, District::SQUARE_DRAW_SIZE*MAX_BOARD_SIZE, District::SQUARE_DRAW_SIZE));
    }

    QColor whiteBrush(0, 0, 0, 0);
    QColor darkBrush(0, 128, 128, 128);

    for (int x=0; x<MAX_BOARD_SIZE; ++x)
    {
        for (int y=0; y<MAX_BOARD_SIZE; ++y)
        {
            if (board[y][x] > 0)
            {
                painter.setBrush(darkBrush);
            }
            else
            {
                painter.setBrush(whiteBrush);
            }
            painter.drawRect(QRectF(x*District::SQUARE_DRAW_SIZE, (y+1)*District::SQUARE_DRAW_SIZE, District::SQUARE_DRAW_SIZE, District::SQUARE_DRAW_SIZE));
        }
    }


    QString formattedString = "";
    if (totalTime < 60)
    {
        formattedString = QString("          money:%1          backers:%2").arg(totalMoney).arg(totalBackers);
    }
    else
    {
        int emptySpaces = GetEmptySpaces();
        formattedString = QString("money:%1 backers:%2 SCORE:%3").arg(totalMoney).arg(totalBackers).arg(totalMoney - emptySpaces);
    }

    QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, formattedString);
    QFont font;
    font.setStyleStrategy(QFont::ForceOutline);
    painter.setFont(font);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(0, 255), size), Qt::AlignCenter, formattedString);

    painter.end();

    setPixmap(QPixmap::fromImage(image));
}

//External time means you don't buy a piece, but instead sacrifice time for backers
void ElectionMap::AddTimeExternal(int time)
{
    if (totalTime < 60)
    {
        totalMoney += time;
    }
    AddTimeInternal(time);
}

void ElectionMap::AddTimeInternal(int time)
{
    if (totalTime < 60 || (TurnChangeObject != nullptr && TurnChangeObject->IsThereLiveTownshipWaiting()))
    {
        if (totalTime == 0)
        {
            totalTime = 2;  //compensate for the fact that the trackboard has 3 initial non-scoring spaces and jumpts to 3 with the first 1 time added.
        }
        totalTime += time;

        if (!coinMultipliers.empty())
        {
            while (!coinMultipliers.empty() && totalTime > coinMultipliers.top())
            {
                totalMoney += totalBackers;
                qDebug() << "multipliers: totalBackers added=" << totalBackers << ":multiplier=" << coinMultipliers.top();
                coinMultipliers.pop();
            }
        }

        if (TimeCallback != nullptr)
        {
            TimeCallback(totalTime);
        }
    }

    if (TurnChangeObject != nullptr)
    {
        TurnChangeObject->SwitchTurn();
    }

    DrawBoard();
    qDebug() << "Distance added:totalMoney=" << totalMoney << ";totalBackers=" << totalBackers << ";totalTime=" << totalTime;
}

bool ElectionMap::DoesTmpShapeFit(const QPoint &offset)
{
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            if (tmpShape[y][x] > 0 && board[y+offset.y()][x+offset.x()] > 0)
            {
                return false;
            }
        }
    }

    return true;
}

void ElectionMap::Pass(const int shape[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE], int bkrs, int cst, int dist)
{
    ZeroTmpShape();
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            tmpShape[y][x] = shape[y][x];
        }
    }

    tempBackers = bkrs;
    tempCost = cst;
    tempTime = dist;
}

bool ElectionMap::PlacePiece(District& piece, const QPoint &offset)
{
    if (!turnOn)
    {
        return false;
    }
    if (totalTime < 60 || (TurnChangeObject != nullptr && TurnChangeObject->IsThereLiveTownshipWaiting()))
    {
        piece.Passing(this);
        if (!DoesTmpShapeFit(offset))
        {
            return false;
        }

        if (tempCost > totalMoney)
        {
            //TODO: include a message or callback indicating failure reason
            return false;
        }

        piece.SetLivePatch(false);  //just in case it's a patch, make sure to indicate that it was used up.
        totalBackers += tempBackers;
        qDebug() << "totalBackers" << totalBackers;
        totalMoney -= tempCost;
        AddTimeInternal(tempTime);
        tempBackers = 0;
        tempCost = 0;
        tempTime = 0;

        qDebug() << "Piece placed:totalMoney=" << totalMoney << ";totalBackers=" << totalBackers << ";totalTime=" << totalTime;

        for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
        {
            for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
            {
                if (tmpShape[y][x] > 0)
                {
                    board[y+offset.y()][x+offset.x()] = tmpShape[y][x];
                }
            }
        }
        return true;
    }
    return false;
}

void ElectionMap::ZeroTmpShape()
{
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            tmpShape[y][x] = 0;
        }
    }
}

bool ElectionMap::Intersect(const QPoint& point)
{
//    int left = this->pos().x() - District::SQUARE_DRAW_SIZE;
//    int top = this->pos().y() - District::SQUARE_DRAW_SIZE;
//    auto contents = this->contentsRect();

    if (this->pos().x() <= point.x()
        && this->pos().y() <= point.y()
        && this->pos().x() + this->contentsRect().right() > point.x()
        && this->pos().y() + this->contentsRect().bottom() > point.y())
    {
        return true;
    }

    return false;
}

void ElectionMap::zeroBoard()
{
    for (int x=0; x<MAX_BOARD_SIZE; ++x)
    {
        for (int y=0; y<MAX_BOARD_SIZE; ++y)
        {
            board[y][x] = 0;
        }
    }
}

void ElectionMap::SetTurn(bool t)
{
    turnOn = t;
    DrawBoard();
}

int ElectionMap::GetEmptySpaces() const
{
    int result = 0;
    for (int x=0; x<MAX_BOARD_SIZE; ++x)
    {
        for (int y=0; y<MAX_BOARD_SIZE; ++y)
        {
            if (board[y][x] == 0)
            {
                ++result;
            }
        }
    }
    return result;
}

bool ElectionMap::IsSolid7x7()
{
    if (FoundSolid7x7(0, 0) || FoundSolid7x7(0, 1) || FoundSolid7x7(1, 0) || FoundSolid7x7(1, 1))
    {
        totalMoney += 7;
        return true;
    }
    return false;
}

bool ElectionMap::FoundSolid7x7(int offsetX, int offsetY)
{
    if (offsetX > 1 || offsetX < 0 || offsetY > 1 || offsetY < 0)
    {
        return false;
    }

    for (int x=offsetX; x<offsetX+7; ++x)
    {
        for (int y=offsetY; y<offsetY+7; ++y)
        {
            if (board[y][x] == 0)
            {
                return false;
            }
        }
    }
    return true;
}
