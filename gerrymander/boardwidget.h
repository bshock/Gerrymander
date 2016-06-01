#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <vector>
#include "turnchange.h"

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

/*
 * Note:
 * I don't know this for a fact, but I believe this widget's drag/drop capabilities are interfering with button presses in it.
 * -- events are QPushButton aren't responding at all, if they're in this widget.
 * -- Note that the events aren't coming through the widget's event handlers, though.
 */

class ElectionMap;
class Calendar;
class District;
class BoardWidget : public QWidget, public TurnChange
{
    static const unsigned int ENABLED_PIECE_NUMBER = 3;

    QPoint hotSpot;
    QPoint oldPosition;

    ElectionMap *gameBoard1;
    ElectionMap *gameBoard2;
    Calendar *calendar;

    std::vector<District*> pieces;
    District *selectedPiece;
    unsigned int lastSelectedPieceIndex;
    unsigned int currentStartPieceIndex;
    std::vector<District*> townships;

    int turn;

    bool solid7x7Found;
public:
    BoardWidget(QWidget *parent = 0);

    virtual void SwitchTurn() override;
    virtual bool IsThereLiveTownshipWaiting() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void acceptDropEvent(QDropEvent *event);
    void pushStartForward();

};

#endif // BOARDWIDGET_H

