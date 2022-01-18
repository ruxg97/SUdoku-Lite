#include <algorithm>
#include <QDebug>
#include <QString>

#include "mainwindow.h"
#include "changecellcommand.h"
#include "tableio.h"
#include "ui_mainwindow.h"

TableIO::TableIO(QTableWidget *uitable) : data(new Data), table(uitable) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            table->setItem(i, j, item);
        }
    }
    drawBackground(true);
    undoStack.setUndoLimit(10);
}

void TableIO::newGame(void) {
    qDebug() << "received new game signal";
    switch(MainWindow::MODE){
    case MainWindow::RANDOM_GENERATE:
        int blanknum;
        switch (current_difficulty) {
        case EASY:blanknum = 20; break;
        case NORMAL:blanknum = 40; break;
        case HARD:blanknum = 60; break;
        }
        data->newPuzzle(blanknum);
        break;

    case MainWindow::INTERNAL_PUZZLE:


        break;
    case MainWindow::SOLVE_PUZZLE:


        break;
    }
    undoStack.clear();
    setPuzzle();
    drawBackground(true);
    MainWindow::STATUS = MainWindow::PLAYING;
}

void TableIO::checkAns(void) {
    switch(MainWindow::MODE){
    case MainWindow::RANDOM_GENERATE:
    case MainWindow::INTERNAL_PUZZLE:
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                auto digit = table->item(i, j)->text().toInt();
                if(digit >= 1 && digit <= 9){
                    data->setUserAns(i + 1, j + 1, digit);
                }
                else{
                    emit answerWrong();
                    return;
                }
            }
        }
        if (data->checkUserAns()) {
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (!isCellPrefilled(i, j)) {
                        table->item(i, j)->setTextColor(QColor(0, 255, 0));
                    }
                }
            }
            emit answerRight();
        } else {
            emit answerWrong();
        }
        break;

    case MainWindow::SOLVE_PUZZLE:
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                auto digit = table->item(i, j)->text().toInt();
                data->setUserAns(i + 1, j + 1, digit);
            }
        }
        if(data->solveSudoku()){
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if(table->item(i, j)->text().toInt() == 0){
                        table->item(i, j)->setText(QString::number(data->getUserAns(i + 1, j + 1)));
                        table->item(i, j)->setTextColor(COLOR_HINT);
                    }
                }
            }
            emit puzzleSolved();
        }
        else{
            emit unableToSolve();
        }
        break;
    }
}

void TableIO::difficultyChanged(Difficulty difficulty) {
    current_difficulty = difficulty;
}

void TableIO::clear(void) {
    undoStack.clear();
    setPuzzle();
    drawBackground(true);
    emit tableContentChanged();
}

void TableIO::hint(void) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if(data->getPuzzle(i + 1, j + 1) == 0){
                auto *item = table->item(i, j);
                QString str = item->text();
                int ans = data->getAns(i + 1, j + 1);
                if(str.isEmpty()){
                    auto *command = new ChangeCellCommand(item, this);
                    command->setNewState(QString::number(ans), item->font(), COLOR_HINT);
                    command->setPreviousState(item->text(), item->font(), item->textColor());
                    undoStack.push(command);
                    return;
                }
                else if(str.toInt() != ans){
                    if(item->textColor() != COLOR_RED){
                        item->setTextColor(COLOR_RED);
                        return;
                    }
                }
            }
        }
    }
}

void TableIO::setPuzzle(void) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if(MainWindow::MODE == MainWindow::SOLVE_PUZZLE){
                table->item(i, j)->setText(QString());
                table->item(i, j)->setTextColor(COLOR_BLACK);
            }
            else{
                int num = data->getPuzzle(i + 1, j + 1);
                if (num) {
                    table->item(i, j)->setText(QString::number(num));
                    table->item(i, j)->setTextColor(COLOR_DARK_GREY);
                } else {
                    table->item(i, j)->setFont(table->font());
                    table->item(i, j)->setText(QString());
                    table->item(i, j)->setTextColor(COLOR_BLACK);
                }
            }
        }
    }
}

QVector<int> TableIO::getCurrentNumbers()
{
    return getNumbersInCell(table->currentRow(), table->currentColumn());
}

QVector<int> TableIO::getNumbersInCell(int row, int column)
{
    QStringList list = table->item(row, column)->text().split(" ");
    QVector<int> result;
    for(const QString &s : list){
        if(s.length() == 1)
            result.push_back(s.toInt());
    }
    return result;
}

/**Set the basic grey and white background colour **/
void TableIO::drawBackground(bool init) {
    QVector<int> numbers;
    bool toDrawHint = MainWindow::MODE != MainWindow::SOLVE_PUZZLE &&
            !init && currentColumn >= 0 && currentRow >= 0;
    if(toDrawHint)
        numbers = getNumbersInCell(currentRow, currentColumn);
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++) {
            QColor color = nullptr;
            int bigRow = i / 3;
            int bigColumn = j / 3;
            if(toDrawHint){
                if(eyestate){
                    //Eyes
                    if(i == currentRow || j==currentColumn ||
                            (bigRow == currentRow / 3 && bigColumn == currentColumn / 3))
                        color = COLOR_EYE;

                    //Highlight same numbers
                    for(int k : numbers){
                        if(getNumbersInCell(i, j).contains(k)){
                            color = COLOR_HIGHLIGHT;
                        }
                    }
                }
            }

            if(color == nullptr){
                //Set the nine-square grid 2 4 6 8 the background colour is white
                if ((bigRow + bigColumn) % 2) {
                    color = COLOR_WHITE;
                }
                //Set the nine-square grid 1 3 5 7 9 the background colour is grey
                else {
                    color = COLOR_GREY;
                }
            }
            table->item(i, j)->setBackgroundColor(color);
        }
    }
}


void TableIO::cellChanged()
{
    currentRow = table->currentRow();
    currentColumn = table->currentColumn();
    drawBackground();
    emit tableContentChanged();
}

void TableIO::changeEyestate() {
    eyestate = !eyestate;
    drawBackground();
}

bool TableIO::isCellPrefilled(int row, int column){
    return data->getPuzzle(row + 1, column + 1) != 0;
}

bool TableIO::isCurrentCellPrefilled()
{
    return isCellPrefilled(table->currentRow(), table->currentColumn());
}

void TableIO::setInternalGame(QString _puzzle, QString _solution, QString _userGiven)
{
    undoStack.clear();
    QVector<int> puzzle, solution;
    for(const auto &s : _solution.split(",").toVector()){
        solution.push_back(s.toInt());
    }
    for(const auto &s : _puzzle.split(",").toVector()){
        puzzle.push_back(s.toInt());
    }

    data->setPuzzle(puzzle);
    data->setSolution(solution);
    setPuzzle();
}

void TableIO::clearCell()
{
    if(MainWindow::MODE == MainWindow::SOLVE_PUZZLE){
        table->currentItem()->setText(QString());
        table->currentItem()->setTextColor(COLOR_BLACK);
    }
    if(!isCurrentCellPrefilled() && !table->currentItem()->text().isNull()){
        auto *widget = table->currentItem();
        ChangeCellCommand *command = new ChangeCellCommand(widget, this);
        command->setNewState(QString(), widget->font(), QColor(0, 0, 0));
        command->setPreviousState(widget->text(), widget->font(), widget->textColor());
        undoStack.push(command);
    }
}

void TableIO::numberPressed(int number)
{
    if(MainWindow::MODE == MainWindow::SOLVE_PUZZLE){
        table->currentItem()->setText(QString::number(number));
        return;
    }

    if(isCurrentCellPrefilled() || !(currentColumn >= 0 && currentRow >= 0)){
        return;
    }
    auto currentNumbers = getCurrentNumbers();

    auto i = currentNumbers.indexOf(number);
    if(i != -1){
        currentNumbers.remove(i);
    }
    else if(currentNumbers.length() <= 3){
        currentNumbers.push_back(number);
    }
    else{
        return;
    }
    auto *widget = table->currentItem();

    QString result;
    QFont font;
    switch(currentNumbers.length()){
    case 1:
        result = QString::number(currentNumbers[0]);
        font.setPointSize(25);
        break;
    case 2:
        result = QString::number(currentNumbers[0]) % " " % QString::number(currentNumbers[1]);
        font.setPointSize(15);
        break;
    case 3:
        result = QString::number(currentNumbers[0]) % " " % QString::number(currentNumbers[1]) % "      "
                % QString::number(currentNumbers[2]);
        font.setPointSize(12);
        break;
    case 4:
        result = QString::number(currentNumbers[0]) % " " % QString::number(currentNumbers[1]) % "      "
                % QString::number(currentNumbers[2]) % " " % QString::number(currentNumbers[3]);
        font.setPointSize(12);
        break;
    }
    auto *command = new ChangeCellCommand(widget, this);
    command->setNewState(result, font, QColor(0, 0, 0));
    command->setPreviousState(widget->text(), widget->font(), widget->textColor());
    undoStack.push(command);
}
