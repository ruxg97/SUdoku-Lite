#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QTranslator>
#include <QtSql/QSqlDatabase>

#include "tableio.h"
#include "timer.h"
#include "ui_mainwindow.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlDatabase *_db, QWidget *parent = 0);
    void initInternal();
    void initGUI();
    ~MainWindow();

    enum PLAY_MODE {
        RANDOM_GENERATE,
        INTERNAL_PUZZLE,
        SOLVE_PUZZLE
    };
    enum GAME_STATUS {
        UNINITIALIZED,
        PLAYING,
        PAUSED,
        SUCCESS,
        FAILED
    };
    static PLAY_MODE MODE;
    static GAME_STATUS STATUS;

public slots:
    void setRemainCell();
    void pause();
    void changeEyeIcon();
    void success();
    void failed();
    void undoStatusChanged();

signals:
    void overlayWindowClosing();
    void gameModeChanged();
    void playStatusChanged();

private slots:
    void onPlayModeChanged(int index);
    void cellChanged();
    void clearRequested();
    void newGame();
    void difficultyChanged(int index);

private:

    bool eyestate=1;

    Ui::MainWindow *ui;
    TableIO *tio = nullptr;
    Timer *lcdtimer = nullptr;
    QLCDNumber *countLCD = nullptr;
    QSignalMapper *mapper;
    QPushButton *buttons[9];
    QSqlDatabase *db;
    QTranslator translator;

    void initializeSignals();
    void showOverlayWindow(QString iconPath, bool transparent);
    void setButtonStatus();
    void changeGameMode();
    int showWarningDialog(QString message);

    enum {
        ENGLISH
    } LANGUAGE;
};

#endif // MAINWINDOW_H
