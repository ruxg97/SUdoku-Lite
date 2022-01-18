#include <cstring>
#include <QString>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSignalMapper>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include "mainwindow.h"
#include "internalsudoku.h"

MainWindow::PLAY_MODE MainWindow::MODE = RANDOM_GENERATE;
MainWindow::GAME_STATUS MainWindow::STATUS = UNINITIALIZED;

MainWindow::MainWindow(QSqlDatabase *_db, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    db(_db) {
    qApp->installTranslator(&translator);
    initInternal();
}

void MainWindow::initInternal() {

    ui->setupUi(this);

    setFixedSize(size());

    QFile styleFile(":/style.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    setStyleSheet(style);

    QPushButton *but[9] = {
        ui->button_1,
        ui->button_2,
        ui->button_3,
        ui->button_4,
        ui->button_5,
        ui->button_6,
        ui->button_7,
        ui->button_8,
        ui->button_9
    };
    memcpy(buttons, but, sizeof(but));

    lcdtimer = new Timer(ui->timerLCD);
    countLCD = ui->countLCD;
    tio = new TableIO(ui->tableWidget);
    mapper = new QSignalMapper();

    initializeSignals();
    undoStatusChanged();
    initGUI();
}

void MainWindow::initGUI() {
    lcdtimer->pause();
    QSqlQuery query;
    switch(MODE){
    case RANDOM_GENERATE:
        ui->comboDifficulty->clear();
        ui->comboDifficulty->insertItem(0, QIcon(":/image/GreenDot"), tr("Easy"), TableIO::EASY);
        ui->comboDifficulty->insertItem(1, QIcon(":/image/YellowDot"), tr("Normal"), TableIO::NORMAL);
        ui->comboDifficulty->insertItem(2, QIcon(":/image/RedDot"), tr("Hard"), TableIO::HARD);
        break;
    case INTERNAL_PUZZLE:
        ui->comboDifficulty->clear();
        query = db->exec("SELECT * FROM InternalGame");
        while(query.next()){
            int id = query.value("id").toInt();
            int difficulty = query.value("difficulty").toInt();
            QString puzzle = query.value("puzzle").toString();
            QString solution = query.value("solution").toString();
            QString userGiven = query.value("userGiven").toString();
            int bestTime = query.value("bestTime").toInt();
            bool success = query.value("success").toBool();
            QString levelName = QString::number(difficulty) % "-" % QString::number(id % 10 == 0 ? 10 : id % 10);
            InternalSudoku sudoku(id, difficulty, puzzle, solution, userGiven, bestTime, success);
            ui->comboDifficulty->insertItem(id - 1, levelName, QVariant::fromValue<InternalSudoku>(sudoku));
        }
        break;
    case SOLVE_PUZZLE:
        break;
    }
    setButtonStatus();
    tio->drawBackground();
}

MainWindow::~MainWindow() {
    delete ui;
    delete tio;
    delete lcdtimer;
    delete mapper;
    delete countLCD;
}

void MainWindow::newGame() {
    if(STATUS == PLAYING){
        if (showWarningDialog(tr("Starting a new game will lose your current progress. sure?")) != QMessageBox::Yes) return;
    }
    STATUS = PLAYING;
    setButtonStatus();
    lcdtimer->restart();
    tio->newGame();
    setRemainCell();
}

void MainWindow::difficultyChanged(int index)
{
    if(index == -1) return;
    if(STATUS==PLAYING){
        if(showWarningDialog(tr("Switching the difficulty/level will start a new game, and the current game progress will be lost, are you sure?")) != QMessageBox::Yes) return;
    }
    if(MODE == RANDOM_GENERATE){
        auto difficulty = TableIO::Difficulty(ui->comboDifficulty->itemData(index).toInt());
        tio->difficultyChanged(difficulty);
        newGame();
    }
    else if (MODE == INTERNAL_PUZZLE){
        if(STATUS == PLAYING){
            //TODO save now data
        }
        auto sudoku = ui->comboDifficulty->itemData(index).value<InternalSudoku>();
        tio->setInternalGame(sudoku.puzzle, sudoku.solution, sudoku.userGiven);
        lcdtimer->restart();
        STATUS = PLAYING;
        setButtonStatus();
        setRemainCell();
    }
}
void MainWindow::setRemainCell() {
    int count = 0;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            QString str = ui->tableWidget->item(i, j)->text();
            if (str.isEmpty()) {
                ++count;
            }
        }
    }
    countLCD->display(count);
}

void MainWindow::pause() {
    if(STATUS == PLAYING){
        STATUS = PAUSED;
        showOverlayWindow(":/image/PlayIcon", false);
    }
}

void MainWindow::changeEyeIcon() {
    if (eyestate) {
        ui->eyeButton->setIcon(QIcon(":/image/EyeOnIcon"));
    } else {
        ui->eyeButton->setIcon(QIcon(":/image/EyeOffIcon"));
    }
    eyestate = !eyestate;
}

void MainWindow::success() {
    STATUS = SUCCESS;
    showOverlayWindow(":/image/bingo", true);
    if(MODE == INTERNAL_PUZZLE){
        //TO write best time and success flag to database
    }
}

void MainWindow::failed() {
    STATUS = FAILED;
    showOverlayWindow(":/image/failed", true);
}

void MainWindow::undoStatusChanged()
{
    ui->button_undo->setEnabled(tio->undoStack.canUndo());
    ui->button_redo->setEnabled(tio->undoStack.canRedo());
}

void MainWindow::setButtonStatus()
{
    switch(MODE){
    case RANDOM_GENERATE:
    case INTERNAL_PUZZLE:
        ui->comboDifficulty->setEnabled(true);
        switch(STATUS){
        case PLAYING:
            if(MODE != INTERNAL_PUZZLE)
                ui->newGameButton->setEnabled(true);
            ui->pauseButton->setEnabled(true);
            ui->checkAnsButton->setEnabled(true);
            ui->clearButton->setEnabled(true);
            ui->pauseButton->setEnabled(true);
            ui->hintButton->setEnabled(true);
            ui->eyeButton->setEnabled(true);
            break;
        case PAUSED:
        case SUCCESS:
        case FAILED:
        case UNINITIALIZED:
            if(MODE == INTERNAL_PUZZLE){
                ui->newGameButton->setEnabled(false);
            }
            else{
                ui->newGameButton->setEnabled(true);
            }
            ui->checkAnsButton->setEnabled(false);
            ui->eyeButton->setEnabled(false);
            ui->clearButton->setEnabled(false);
            ui->pauseButton->setEnabled(false);
            ui->hintButton->setEnabled(false);
            break;
        }
        break;
    case SOLVE_PUZZLE:
        ui->newGameButton->setEnabled(false);
        ui->eyeButton->setEnabled(false);
        ui->hintButton->setEnabled(false);
        ui->comboDifficulty->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->clearButton->setEnabled(true);
        ui->checkAnsButton->setEnabled(true);
        ui->timerLCD->display("CAFE");
        ui->countLCD->display("FF");
        for(QPushButton *button : buttons){
            button->setEnabled(true);
        }
        ui->button_del->setEnabled(true);
        break;
    }
}

void MainWindow::changeGameMode()
{
    STATUS = UNINITIALIZED;
    setButtonStatus();
    initGUI();
    if(MODE == RANDOM_GENERATE) newGame();
}

int MainWindow::showWarningDialog(QString message)
{
    return QMessageBox::warning(this, tr("Warning"), message,
                                QMessageBox::Yes, QMessageBox::No);
}

void MainWindow::clearRequested()
{
    if(showWarningDialog(tr("Are you sure you want to clear all filled numbers? This operation is irreversible and the current progress will be lost.")) != QMessageBox::Yes) return;
    if(MODE != SOLVE_PUZZLE) lcdtimer->restart();
    tio->clear();
}

void MainWindow::onPlayModeChanged(int index)
{
    if(STATUS==PLAYING){
        if(showWarningDialog(tr("Switching modes will cause the current game progress to be lost. Confirm?")) != QMessageBox::Yes) return;
    }
    PLAY_MODE newMode = static_cast<PLAY_MODE>(index);
    MODE = newMode;
    changeGameMode();
}


void MainWindow::initializeSignals()
{
    connect(ui->tableWidget, &QTableWidget::currentCellChanged,
            this, &MainWindow::cellChanged);

    connect(ui->comboPlayMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onPlayModeChanged(int)));

    connect(ui->comboDifficulty, SIGNAL(currentIndexChanged(int)),
            this, SLOT(difficultyChanged(int)));

    connect(tio, &TableIO::tableContentChanged, this, &MainWindow::setRemainCell);

    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::newGame);

    //Clear Button
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearRequested);

    //Hint Button
    connect(ui->hintButton, &QPushButton::clicked, tio, &TableIO::hint);

    //Check Answer Button
    connect(ui->checkAnsButton, &QPushButton::clicked, tio, &TableIO::checkAns);

    connect(tio, &TableIO::answerRight, this, &MainWindow::success);
    connect(tio, &TableIO::answerWrong, this, &MainWindow::failed);
    connect(tio, &TableIO::unableToSolve, this, &MainWindow::failed);

    //Pause Button
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pause);
    connect(this, &MainWindow::overlayWindowClosing, [&](){
        if(MODE != SOLVE_PUZZLE && (STATUS == PAUSED || STATUS == FAILED)){
            lcdtimer->resume();
            STATUS = PLAYING;
        }
        setButtonStatus();
    });

    //Eye Button
    connect(ui->eyeButton, &QPushButton::clicked, tio, &TableIO::changeEyestate);
    connect(ui->eyeButton, &QPushButton::clicked, this, &MainWindow::changeEyeIcon);

    //Delete Button
    connect(ui->button_del, &QPushButton::clicked, tio, &TableIO::clearCell);

    //Undo and Redo Button
    connect(ui->button_undo, &QPushButton::clicked, &tio->undoStack, &QUndoStack::undo);
    connect(ui->button_redo, &QPushButton::clicked, &tio->undoStack, &QUndoStack::redo);

    //UndoStack
    connect(&tio->undoStack, &QUndoStack::canUndoChanged, this, &MainWindow::undoStatusChanged);
    connect(&tio->undoStack, &QUndoStack::canRedoChanged, this, &MainWindow::undoStatusChanged);

    //Number Buttons
    for(int i=0;i<9;++i){
        connect(buttons[i], SIGNAL(clicked(bool)), mapper, SLOT(map()));
        mapper->setMapping(buttons[i], i+1);
    }
    connect(mapper, SIGNAL(mapped(int)), tio, SLOT(numberPressed(int)));
}

void MainWindow::showOverlayWindow(QString iconPath, bool transparent)
{
    QWidget *overlayWindow = new QWidget(ui->tableWidget, Qt::FramelessWindowHint);
    overlayWindow->setContentsMargins(0, 0, 0, 0);
    overlayWindow->setGeometry(0, 0, ui->tableWidget->width(), ui->tableWidget->height());
    overlayWindow->setObjectName("overlaywindow");
    if(!transparent)
        overlayWindow->setStyleSheet("#overlaywindow {background-color:rgba(153,204,255,255);}");

    QPushButton *button = new QPushButton(overlayWindow);
    button->setIcon(QIcon(iconPath));
    button->setIconSize(ui->tableWidget->size());
    button->setFlat(true);
    lcdtimer->pause();
    setButtonStatus();
    overlayWindow->show();
    connect(button, &QPushButton::clicked, overlayWindow, &QWidget::close);
    connect(button, &QPushButton::clicked, this, &MainWindow::overlayWindowClosing);
}

void MainWindow::cellChanged()
{
    if(MODE != SOLVE_PUZZLE){
        auto prefilled = tio->isCurrentCellPrefilled();
        for(QPushButton *button : buttons){
            button->setEnabled(!prefilled);
        }
        ui->button_del->setEnabled(!prefilled);
        tio->cellChanged();
    }
}
