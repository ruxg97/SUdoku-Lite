#ifndef INTERNALSUDOKU_H
#define INTERNALSUDOKU_H

#include <QMetaType>
#include <QString>

class InternalSudoku
{
public:
    InternalSudoku();
    InternalSudoku(int _id, int _difficulty,
                   QString _puzzle, QString _solution, QString _userGiven,
                   int _bestTime, bool _success);
    int id;
    int difficulty;
    QString puzzle;
    QString solution;
    QString userGiven;
    int bestTime;
    bool success;
};

Q_DECLARE_METATYPE(InternalSudoku);

#endif // INTERNALSUDOKU_H
