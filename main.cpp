#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QtSql/QSqlDatabase>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    QString databasePath = dir.absoluteFilePath("sudoku.db");

    if(!QFile::exists(databasePath)){
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QFile::copy(":/assets/sudoku.db", databasePath);
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);
    db.open();

    MainWindow *main = new MainWindow(&db);
    main->show();
    int ret = app.exec();
    db.close();
    return  ret;
}
