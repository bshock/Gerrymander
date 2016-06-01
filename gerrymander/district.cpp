#include "district.h"
#include <QtWidgets>
#include <QTextStream>
#include <string>

//! [0]
District::District(const QString &text, bool showtext, QWidget *parent)
    : QLabel(parent), angle(0), flip(0), showHighlight(false), showText(showtext),
      livePatch(false)
{
    readText(text);
    //print();

    DrawPiece();
    m_labelText = text;

}

void District::ShowContextMenu(const QPoint& pos) // this is a slot
{
    QMenu myMenu;
    QAction *rotate = new QAction("Rotate", this);
    QAction *flip = new QAction("Flip", this);
    myMenu.addAction(rotate);
    myMenu.addAction(flip);

    QAction* selectedItem = myMenu.exec(pos);
    if (selectedItem)
    {
        if (selectedItem == rotate)
        {
            Rotate();
            DrawPiece();
        }
        else if (selectedItem == flip)
        {
            Flip();
            DrawPiece();
        }
    }
}

QString District::labelText() const
{
    QString result;
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            if (shape[y][x] == 0)
            {
                result += "0";
            }
            else if (shape[y][x] == 1)
            {
                result += "1";
            }
            else if (shape[y][x] == 2)
            {
                result += "2";
            }

        }
    }

    QString s1;
    QString s2;
    QString s3;
    s1 = (cost == 10)?"A":QString("%1").arg(cost);
    s2 = (time == 10)?"A":QString("%1").arg(time);
    s3 = (coinNumber == 10)?"A":QString("%1").arg(coinNumber);
    result += s1;
    result += s2;
    result += s3;

    //return m_labelText;
    return result;
}

int District::labelAngle() const
{
    return angle;
}

int District::labelFlip() const
{
    return flip;
}

void District::readText(const QString &text)
{
    int i=0;
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            QString digitStr = text[i];
            std::string ds = digitStr.toStdString();
            int digit = std::stoi(ds);
            shape[y][x] = digit;
            ++i;
        }
    }

    QString costStr = text[i];
    if (costStr == "A")
    {
        cost = 10;
    }
    else
    {
        std::string c = costStr.toStdString();
        cost = std::stoi(c);
    }

    ++i;
    QString distStr = text[i];
    std::string d = distStr.toStdString();
    time = std::stoi(d);

    ++i;
    QString coinStr = text[i];
    std::string b = coinStr.toStdString();
    coinNumber = std::stoi(b);
}

void District::print()
{
    QTextStream out(stdout);
    out << "\n";
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            out << shape[x][y] << " ";
        }
        out << "\n";
    }
    out << "\n";
}

void District::print(int a[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE])
{
    QTextStream out(stdout);
    out << "\n";
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            out << a[x][y] << " ";
        }
        out << "\n";
    }
    out << "\n";
}

void District::DrawPiece()
{
    int pieceSize = PiecePasser::MAX_SIZE*SQUARE_DRAW_SIZE;
    if (!showText)
    {
        pieceSize = SQUARE_DRAW_SIZE;
    }
    QImage image(pieceSize, pieceSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

//    QFont font;
//    font.setStyleStrategy(QFont::ForceOutline);

    drawFromArray(image);

    setPixmap(QPixmap::fromImage(image));
}

void District::drawFromArray(QImage &image)
{
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    if (showHighlight)
    {
        QColor greenBrush(0, 255, 0, 32);
        painter.setBrush(greenBrush);
        painter.drawRect(0, 0, PiecePasser::MAX_SIZE*SQUARE_DRAW_SIZE, PiecePasser::MAX_SIZE*SQUARE_DRAW_SIZE);
    }

    QColor darkBrush(0, 128, 128, 128);
    QColor coinBrush(0, 0, 0, 128);

    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            if (shape[y][x] > 0)
            {
                painter.setBrush(darkBrush);
                painter.drawRect(QRectF(x*SQUARE_DRAW_SIZE, y*SQUARE_DRAW_SIZE, SQUARE_DRAW_SIZE, SQUARE_DRAW_SIZE));
            }
            if (shape[y][x] == 2)
            {
                painter.setBrush(coinBrush);
                painter.drawEllipse(QRectF(x*SQUARE_DRAW_SIZE, y*SQUARE_DRAW_SIZE, SQUARE_DRAW_SIZE, SQUARE_DRAW_SIZE));
            }
        }
    }

    if (showText)
    {
        QString formattedString = QString("c:%1 d:%2").arg(cost).arg(time);
        QFontMetrics metric(font());
        QSize size = metric.size(Qt::TextSingleLine, formattedString);
        QFont font;
        font.setStyleStrategy(QFont::ForceOutline);
        painter.setFont(font);
        painter.setBrush(Qt::black);
        painter.drawText(QRect(QPoint(0, 100), size), Qt::AlignCenter, formattedString);
    }
    painter.end();
}

void District::Rotate()
{
    int temp[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE];

    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            temp[x][PiecePasser::MAX_SIZE - y - 1] = shape[y][x];
        }
    }

    ++angle;
    angle %= 4;

    CopyArrayAToArrayB(temp, shape);

    normalize();
}

void District::Flip()
{
    int temp[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE];

    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            temp[y][PiecePasser::MAX_SIZE - x - 1] = shape[y][x];
        }
    }

    ++flip;
    flip %= 2;

    CopyArrayAToArrayB(temp, shape);
    normalize();
}

void District::CopyArrayAToArrayB(int a[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE], int b[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE])
{
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            b[y][x] = a[y][x];
        }
    }
}

void District::Passing(PiecePasser* passer) const
{
    if (passer != nullptr)
    {
        passer->Pass(shape, coinNumber, cost, time);
    }
}

void District::normalize()
{
    int offsetY = 0;
    //1) find empty top rows
    for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
    {
        for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
        {
            if (shape[y][x] > 0)
            {
                goto CHECK_COLUMNS;
            }
        }
        ++offsetY;
    }
CHECK_COLUMNS:
    int offsetX = 0;
    //2) find empty left columns
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            if (shape[y][x] > 0)
            {
                goto MOVE_TO_OFFSETS;
            }
        }
        ++offsetX;
    }
MOVE_TO_OFFSETS:

    int temp[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE];
    zeroArray(temp);
    //print(temp);

    //3) move everything by row/column offsets
    for (int x=0; x<PiecePasser::MAX_SIZE - offsetX; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE - offsetY; ++y)
        {
            if (shape[y+offsetY][x+offsetX] > 0)
            {
                temp[y][x] = shape[y+offsetY][x+offsetX];
            }
        }
    }

    CopyArrayAToArrayB(temp, shape);
}

void District::zeroArray(int a[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE])
{
    for (int x=0; x<PiecePasser::MAX_SIZE; ++x)
    {
        for (int y=0; y<PiecePasser::MAX_SIZE; ++y)
        {
            a[y][x] = 0;
        }
    }
}
