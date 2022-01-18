#include "internalsudoku.h"

InternalSudoku::InternalSudoku(){

}

InternalSudoku::InternalSudoku(int _id, int _difficulty,
                               QString _puzzle, QString _solution,
                               QString _userGiven, int _bestTime,
                               bool _success):
    id(_id), difficulty(_difficulty), puzzle(_puzzle),
    solution(_solution), userGiven(_userGiven),
    bestTime(_bestTime), success(_success){}
