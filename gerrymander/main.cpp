#include <QApplication>
#include "boardwidget.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gerrymander);

    QApplication app(argc, argv);
#ifdef QT_KEYPAD_NAVIGATION
    QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
#endif
    BoardWidget window;
    window.show();
    return app.exec();
}

/*
-- TODOS:
    1) make townships part of Calendar, so that you can hit them as part of Calendar. (Currently they're District objects, and hitting them doesn't register on Calendar.)
    2) disable all District displays when the game is over. You can't select them anyway, but it would look better to SHOW this.
    3) simplify logic for enabling and placing the "townships" (single-square pieces that start on the track board).
    -- The challenge is that townships are enabled only after a player lands or them or passes them on the track board,
    which usually (BUT NOT ALWAYS) happens when that player has exceeded the position of his opponent, ending his
    turn.
        -- In other words, how do you allow a single move (placing a patch on the current player's board) when that
        player's turn is over?
            -- The turn must be prolonged WITHOUT allowing that player to play a regular piece OR move on the track board.
    4) break up the game into a client/server system, so that players can use it on separate PCs.

 */
