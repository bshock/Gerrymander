#ifndef DISTRICT_H
#define DISTRICT_H

#include <QLabel>
#include "piecepasser.h"

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDragMoveEvent;
class QFrame;
QT_END_NAMESPACE

//! [0]
class District : public QLabel
{
public:
    static const int SQUARE_DRAW_SIZE = 25;
private:
    int shape[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE];
    int tmp[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE];
    int cost;
    int time;
    int coinNumber;
    int angle;
    int flip;
    bool showHighlight;
    bool showText;
    QString m_labelText;
    bool livePatch;
public:
    District(const QString &text, bool showtext, QWidget *parent);
    QString labelText() const;
    int labelAngle() const;
    int labelFlip() const;

    void Rotate();
    void Flip();
    void DrawPiece();

    void SetHighlight(bool set)
    {
        showHighlight = set;
        DrawPiece();
    }
    bool IsHighlighted() { return showHighlight; }
    bool IsTextShown() { return showText; } //only single-square patch pieces have showText = false

    bool IsLivePatch() {return livePatch; }
    void SetLivePatch(bool patch) { livePatch = patch; }

    void Passing(PiecePasser* passer) const;
public slots:
    void ShowContextMenu(const QPoint& pos);
private:
    void readText(const QString &text);
    void print();
    void print(int a[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE]);
    void zeroArray(int a[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE]);
    void normalize();
    void drawFromArray(QImage &image);
    void CopyArrayAToArrayB(int a[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE], int b[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE]);
};
//! [0]

#endif // DISTRICT_H
