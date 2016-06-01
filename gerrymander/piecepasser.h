#ifndef PIECEPASSER_H
#define PIECEPASSER_H

class PiecePasser
{
public:
    static const int MAX_SIZE = 5;
    virtual void Pass(const int shape[PiecePasser::MAX_SIZE][PiecePasser::MAX_SIZE], int bkrs, int cst, int tm)=0;
};

#endif // PIECEPASSER_H
