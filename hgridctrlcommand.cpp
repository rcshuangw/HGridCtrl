#include "hgridctrlcommand.h"
HUndoCommand::HUndoCommand()
{

}

HUndoCommand::~HUndoCommand()
{

}

int HUndoCommand::id() const
{

}

void HUndoCommand::redo()
{

}

void HUndoCommand::undo()
{

}

//删除

HDelUndoCommand::HDelUndoCommand(const HCellRange& range,QString strText)
{
    cellRange = range;
    strDeleteText = strText;
}

HDelUndoCommand::~HDelUndoCommand()
{

}

int HDelUndoCommand::id() const
{

}

//再次删除 第一次不做任何操作
void HDelUndoCommand::redo()
{

}

//undo 不删除
void HDelUndoCommand::undo()
{
    //把删除掉的文字重新set到表格中
    QStringList list = strDeleteText.split(QRegExp("\\s+"));
    if(list.count() != cellRange.count())
        return;
    int colCount = cellRange.maxCol() - cellRange.minCol() + 1;
    for(int i = cellRange.minRow(); i < cellRange.maxRow();i++)
    {
        for(int j = cellRange.minCol(); j < cellRange.maxCol();j++)
        {
            QString str = list[i*colCount+j];
        }
    }
}


//粘贴
HPasteUndoCommand::HPasteUndoCommand(const HCellRange& range,QString strText)
{

}

HPasteUndoCommand::~HPasteUndoCommand()
{

}

int HPasteUndoCommand::id() const
{

}
void HPasteUndoCommand::redo()
{

}

void HPasteUndoCommand::undo()
{

}
