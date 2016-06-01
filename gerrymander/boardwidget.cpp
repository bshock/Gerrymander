#include "district.h"
#include "boardwidget.h"
#include "electionmap.h"
#include "calendar.h"
#include <QtWidgets>
#include <QPainter>
#include <QPushButton>

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent), selectedPiece(nullptr), lastSelectedPieceIndex(0), currentStartPieceIndex(0), turn(0), solid7x7Found(false)
{
    QFile dictionaryFile(":/dictionary/words.txt");
    dictionaryFile.open(QFile::ReadOnly);
    QTextStream inputStream(&dictionaryFile);

    int x = 0;
    int y = 0;
    int count = 0;
    int offsetX = 250;
    int offsetY = 20;
    int increment = PiecePasser::MAX_SIZE*District::SQUARE_DRAW_SIZE + 5;
    std::vector<QString> pieceList;
    while (!inputStream.atEnd())
    {
        QString word;
        inputStream >> word;
        pieceList.push_back(word);
    }


    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pieceList.begin(), pieceList.end()-2, g);

    for (QString &word: pieceList)
    {
        if (!word.isEmpty())
        {
            if (count <10)
            {
                x = count*increment + offsetX;
                y = offsetY;
            }
            else if (count > 9 && count < 17)
            {
                x = 10*increment + offsetX;
                y = (count-10)*increment + offsetY;
            }
            else if (count > 16 && count <27)
            {
                x = (9-(count-17))*increment + offsetX;
                y = 7*increment + offsetY;
            }
            else
            {
                x = offsetX;
                y = (5-(count-27))*increment + offsetY + increment;
            }


            District *piece = new District(word, true, this);
            piece->move(x, y);
            piece->show();
            piece->setAttribute(Qt::WA_DeleteOnClose);

            pieces.push_back(piece);

            ++count;
        }
    }

    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Window, Qt::white);
    setPalette(newPalette);

    //setMinimumSize(400, qMax(200, y));
    setMinimumSize(1920, 1080);
    setWindowTitle(tr("Patchwork"));

    setAcceptDrops(true);


    gameBoard1 = new ElectionMap(this);
    gameBoard1->move(400, 625);
    gameBoard1->show();
    gameBoard1->setAttribute(Qt::WA_DeleteOnClose);
    gameBoard1->SetBoardNumber(0);
    using namespace std::placeholders;
    gameBoard1->SetTurnChanger(this);
    gameBoard1->SetTurn(true);


    gameBoard2 = new ElectionMap(this);
    gameBoard2->move(1260, 625);
    gameBoard2->show();
    gameBoard2->setAttribute(Qt::WA_DeleteOnClose);
    gameBoard2->SetBoardNumber(1);
    using namespace std::placeholders;
    gameBoard2->SetTurnChanger(this);
    gameBoard2->SetTurn(false);

    int tOffsetX = 730;
    int tOffsetY = 200;
    calendar = new Calendar(this);
    calendar->move(tOffsetX,tOffsetY);
    calendar->show();
    calendar->setAttribute(Qt::WA_DeleteOnClose);
    calendar->SetBoard1(gameBoard1);
    calendar->SetBoard2(gameBoard2);
    calendar->SetTurn(0);

    std::for_each(pieces.begin()+currentStartPieceIndex, pieces.begin()+currentStartPieceIndex+ENABLED_PIECE_NUMBER, [](District *p){p->SetHighlight(true);} );

    QString townshipText = "1000000000000000000000000000";
    int tSqOffset = Calendar::TRACK_SQUARE_DRAW_SIZE*0.25;

    //Note: townships should NOT be set to delete on close -- we need them to hang around longer, such as in IsLivePatchWaiting.
    District *township1 = new District(townshipText, false, this);
    township1->move(tOffsetX + tSqOffset, tOffsetY + tSqOffset + (6*Calendar::TRACK_SQUARE_DRAW_SIZE));
    township1->show();
    townships.push_back(township1);

    District *township2 = new District(townshipText, false, this);
    township2->move(tOffsetX + tSqOffset + (2*Calendar::TRACK_SQUARE_DRAW_SIZE), tOffsetY + tSqOffset + (1*Calendar::TRACK_SQUARE_DRAW_SIZE));
    township2->show();
    townships.push_back(township2);

    District *township3 = new District(townshipText, false, this);
    township3->move(tOffsetX + tSqOffset + (6*Calendar::TRACK_SQUARE_DRAW_SIZE), tOffsetY + tSqOffset + (4*Calendar::TRACK_SQUARE_DRAW_SIZE));
    township3->show();
    townships.push_back(township3);

    District *township4 = new District(townshipText, false, this);
    township4->move(tOffsetX + tSqOffset + (3*Calendar::TRACK_SQUARE_DRAW_SIZE), tOffsetY + tSqOffset + (2*Calendar::TRACK_SQUARE_DRAW_SIZE));
    township4->show();
    townships.push_back(township4);

    District *township5 = new District(townshipText, false, this);
    township5->move(tOffsetX + tSqOffset + (3*Calendar::TRACK_SQUARE_DRAW_SIZE), tOffsetY + tSqOffset + (5*Calendar::TRACK_SQUARE_DRAW_SIZE));
    township5->show();
    townships.push_back(township5);

    calendar->SetPatches(&townships);
}

void BoardWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-patchwork")) {
        if (children().contains(event->source())) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void BoardWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-patchwork"))
    {
        if (children().contains(event->source()))
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void BoardWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-patchwork")) {

        auto pieceLeftTop = event->pos() - hotSpot;

        if (gameBoard1->Intersect(pieceLeftTop))
        {
            QPoint boardLocalCoords = pieceLeftTop - gameBoard1->pos();
            QPoint compensation(0,District::SQUARE_DRAW_SIZE);
            boardLocalCoords -= compensation;

            QPoint boardUnitOffset = boardLocalCoords/District::SQUARE_DRAW_SIZE;

            if (gameBoard1->PlacePiece(*selectedPiece, boardUnitOffset))
            {
                gameBoard1->DrawBoard();
                acceptDropEvent(event);
                pushStartForward();
                return;
            }
        }
        if (gameBoard2->Intersect(pieceLeftTop))
        {
            QPoint boardLocalCoords = pieceLeftTop - gameBoard2->pos();
            QPoint compensation(0,District::SQUARE_DRAW_SIZE);
            boardLocalCoords -= compensation;
            QPoint boardUnitOffset = boardLocalCoords/District::SQUARE_DRAW_SIZE;
            if (gameBoard2->PlacePiece(*selectedPiece, boardUnitOffset))
            {
                gameBoard2->DrawBoard();
                acceptDropEvent(event);
                pushStartForward();
                return;
            }
        }
    }
    else
    {
        event->ignore();
    }
}

void BoardWidget::acceptDropEvent(QDropEvent *event)
{
    if (event->source() == this)
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->acceptProposedAction();
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    District *child = static_cast<District*>(childAt(event->pos()));
    if (!child)
    {
        return;
    }
    if (!child->IsHighlighted())
    {
        return;
    }

    selectedPiece = child;
    auto btn = event->button();
    if (btn == Qt::RightButton)
    {
        child->ShowContextMenu(event->pos());
        return;
    }

    auto childLocation = std::find(pieces.begin(), pieces.end(), child);
    if (childLocation != pieces.end())
    {
        lastSelectedPieceIndex = childLocation - pieces.begin();
    }

    hotSpot = event->pos() - child->pos();
    oldPosition = child->pos();

    //TODO: possibly remove the next two blocks of code...
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << child->labelText() << QPoint(hotSpot);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-patchwork", itemData);
    mimeData->setText(child->labelText());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(*child->pixmap());
    drag->setHotSpot(hotSpot);

    child->hide();

    if (drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction) == Qt::MoveAction)
    {
       child->close();
    }
    else
    {
        child->show();
    }
}

void BoardWidget::pushStartForward()
{
    qDebug() << "pushStartForward:turn=" << turn;

    //only remove and erase if the piece is NOT a patch
    if (selectedPiece != nullptr && selectedPiece->IsTextShown())
    {
        pieces.erase(pieces.begin()+lastSelectedPieceIndex);
        selectedPiece->close();
    }

    currentStartPieceIndex = lastSelectedPieceIndex;


    if (calendar->IsCurrentBoardAhead(turn))
    {
        qDebug() << "pushStartForward:board ahead:" << turn;

        //if player has reached or exceeded a patch on the track while placing a piece, don't go on to highlight other pieces
        //  -- only the patch should be playable at this point
        if (IsThereLiveTownshipWaiting())
        {
            qDebug() << "pushStartForward:live patch waiting;" << turn;
            //don't allow any pieces to be highlighted (enabled) by going further in this function
            return;
        }
        //Note: we need this condition that duplicates SwitchTurn because pushStartForward comes up after SwitchTurn if you're placing pieces
    }


    //Special conditions
    //1) you deleted the end piece of the vector

    if (currentStartPieceIndex >= pieces.size())
    {
        qDebug() << "pushStartForward:special condition #1:turn=" << turn;
        currentStartPieceIndex = 0;
    }
    //2) part of the highlighted 3(ENABLED_PIECE_NUMBER) are at the end of the vector and part are at the beginning...
    std::for_each(pieces.begin(), pieces.end(), [](District *p){p->SetHighlight(false);});
    if (currentStartPieceIndex+ENABLED_PIECE_NUMBER > pieces.size() - 1)
    {
        qDebug() << "pushStartForward:special condition #2:turn=" << turn;
        //3) there are no pieces left
        if (pieces.size() <= 0)
        {
            qDebug() << "pushStartForward:special condition #3:turn=" << turn;
            //TODO: create GAME OVER event
            return;
        }
        //4) there are less than ENABLED_PIECE_NUMBER pieces left but more than 0
        else if (pieces.size() <= ENABLED_PIECE_NUMBER)
        {
            qDebug() << "pushStartForward:special condition #4:turn=" << turn;
            std::for_each(pieces.begin(), pieces.end(),[](District *p){p->SetHighlight(true);} );
        }
        else
        {
            qDebug() << "pushStartForward:special condition #5:turn=" << turn;
            std::for_each(pieces.begin()+currentStartPieceIndex, pieces.end(),[](District *p){p->SetHighlight(true);} );
            std::for_each(pieces.begin(), pieces.begin() + (ENABLED_PIECE_NUMBER - (pieces.size() - currentStartPieceIndex )),[](District *p){p->SetHighlight(true);} );
        }
    }
    else
    {
        qDebug() << "pushStartForward:ordinary condition:turn=" << turn;
        std::for_each(pieces.begin()+currentStartPieceIndex, pieces.begin()+currentStartPieceIndex+ENABLED_PIECE_NUMBER,[](District *p){p->SetHighlight(true);} );
    }
}

void BoardWidget::SwitchTurn()
{
    if (!solid7x7Found && gameBoard1->IsSolid7x7())
    {
        solid7x7Found = true;
    }
    else if (!solid7x7Found && gameBoard2->IsSolid7x7())
    {
        solid7x7Found = true;
    }

    if (calendar->IsCurrentBoardAhead(turn))
    {
        //if player has reached or exceeded a patch on the track, don't change turns yet
        if (IsThereLiveTownshipWaiting())
        {
            //but make sure he can't select any piece besides the live patch
            std::for_each(pieces.begin(), pieces.end(), [](District *p){p->SetHighlight(false);});
            return;
        }
        //if the player who reached a patch isn't ahead, no need to do anything yet; the patch will still be waiting for him

        ++turn;
        turn %= 2;
        if (turn == 0)
        {
            gameBoard1->SetTurn(true);
            gameBoard2->SetTurn(false);
        }
        else if (turn == 1)
        {
            gameBoard1->SetTurn(false);
            gameBoard2->SetTurn(true);
        }
        calendar->SetTurn(turn);

    }
}

bool BoardWidget::IsThereLiveTownshipWaiting()
{
    //TODO: fix this clumsy loop with stl
    for (auto patch:townships)
    {
        if (patch->IsLivePatch())
        {
            return true;
        }
    }
    return false;
}
