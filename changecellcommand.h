#ifndef CHANGECELLCOMMAND_H
#define CHANGECELLCOMMAND_H

#include <QTableWidgetItem>
#include <QString>
#include <QFont>
#include <QColor>
#include "tableio.h"

class ChangeCellCommand : public QUndoCommand
{
public:
    ChangeCellCommand(QTableWidgetItem *_item, TableIO *_tio);
    void setPreviousState(const QString& string, const QFont& font, const QColor& color);
    void setNewState(const QString& string, const QFont& font, const QColor& color);
    virtual void undo() override;
    virtual void redo() override;

private:
    QTableWidgetItem *item;
    QString previousString, newString;
    QFont previousFont, newFont;
    QColor previousColor, newColor;
    TableIO *tio;
};

#endif // CHANGECELLCOMMAND_H
