#include "changecellcommand.h"


ChangeCellCommand::ChangeCellCommand(QTableWidgetItem *_item, TableIO *_tio)
    : item(_item), tio(_tio) {}

void ChangeCellCommand::setPreviousState(const QString &string, const QFont &font, const QColor &color)
{
    previousString = string;
    previousFont = font;
    previousColor = color;
}

void ChangeCellCommand::setNewState(const QString &string, const QFont &font, const QColor &color)
{
    newString = string;
    newFont = font;
    newColor = color;
}

void ChangeCellCommand::undo()
{
    item->setFont(previousFont);
    item->setText(previousString);
    item->setTextColor(previousColor);
    tio->cellChanged();
}

void ChangeCellCommand::redo()
{
    item->setFont(newFont);
    item->setText(newString);
    item->setTextColor(newColor);
    tio->cellChanged();
}
