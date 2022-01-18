#ifndef TABLEIO_H
#define TABLEIO_H

#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QUndoStack>
#include <QUndoCommand>
#include <QColor>
#include "data.h"


class TableIO :public QObject
{
    Q_OBJECT
public:
    TableIO(QTableWidget *uitable);
    void newGame();
    void clear();
    void hint();
    void checkAns();
    void drawBackground(bool init = false);
    void clearCell();
    void changeEyestate();
    bool isCurrentCellPrefilled();
    void setInternalGame(QString _puzzle, QString _solution, QString _userGiven);
    QUndoStack undoStack;
    enum Difficulty{
        EASY,
        NORMAL,
        HARD
    };
    void difficultyChanged(Difficulty difficulty);

public slots:
    void cellChanged();
    void numberPressed(int number);

signals:
    void answerRight();
    void answerWrong();
    void puzzleSolved();
    void unableToSolve();
    void tableContentChanged();

private:
    void setPuzzle(void);//set to table
    QVector<int> getCurrentNumbers();
    QVector<int> getNumbersInCell(int column, int row);
    bool isCellPrefilled(int row, int column);

    QTableWidget* table;
    Data *data;

    Difficulty current_difficulty = EASY;
    bool eyestate = true;

    int currentRow, currentColumn;
    const QColor COLOR_RED = QColor(255, 0, 0);
    const QColor COLOR_WHITE = QColor(255, 255, 255);
    const QColor COLOR_BLACK = QColor(0, 0, 0);
    const QColor COLOR_DARK_GREY = QColor(150, 150, 150);
    const QColor COLOR_GREY = QColor(235, 235, 235);
    const QColor COLOR_HIGHLIGHT = QColor(255, 255, 0);
    const QColor COLOR_EYE = QColor(217, 179, 255);
    const QColor COLOR_HINT = QColor(255, 204, 0);
};

#endif // TABLEIO_H
