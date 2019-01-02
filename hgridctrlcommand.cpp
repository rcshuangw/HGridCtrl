#include "hgridctrlcommand.h"
HUndoCommand::HUndoCommand()
{

}

HUndoCommand::~HUndoCommand()
{

}

int HUndoCommand::id() const
{
    return 0;
}

void HUndoCommand::redo()
{

}

void HUndoCommand::undo()
{

}

//删除

HDelUndoCommand::HDelUndoCommand(HGridCtrl* pGridCtrl,const HCellRange& range,QString strText)
    :m_pGridCtrl(pGridCtrl)
{
    cellRange = range;
    strDeleteText = strText;
    bFirstTime = true;
}

HDelUndoCommand::~HDelUndoCommand()
{

}

int HDelUndoCommand::id() const
{
return 0;
}

//再次删除 第一次不做任何操作
void HDelUndoCommand::redo()
{
    if(bFirstTime)
    {
        bFirstTime = false;
        return;
    }

    //把表格中的文字删除
    if(!m_pGridCtrl) return;
    m_pGridCtrl->clearCells(cellRange);
}

//undo 不删除
void HDelUndoCommand::undo()
{
    //把删除掉的文字重新set到表格中
    if(!m_pGridCtrl) return;
    QStringList list = strDeleteText.split(QRegExp("\\s+"));
    if(list.count() != cellRange.count())
        return;
    int colCount = cellRange.maxCol() - cellRange.minCol() + 1;
    for(int i = cellRange.minRow(); i < cellRange.maxRow();i++)
    {
        for(int j = cellRange.minCol(); j < cellRange.maxCol();j++)
        {
            QString str = list[i*colCount+j];
            m_pGridCtrl->setItemText(i,j,str);
        }
    }
}


//粘贴
HPasteUndoCommand::HPasteUndoCommand(HGridCtrl* pGridCtrl,const HCellRange& range,QString newText,QString oldText)
    :m_pGridCtrl(pGridCtrl)
{
    cellRange = range;
    strNewText = newText;
    strOldText = oldText;
    bFirstTime = true;
}

HPasteUndoCommand::~HPasteUndoCommand()
{

}

int HPasteUndoCommand::id() const
{
    return 0;
}

//将新的回填
void HPasteUndoCommand::redo()
{

    if(bFirstTime)
    {
        bFirstTime = false;
        return;
    }
    //把删除掉的文字重新set到表格中
    if(!m_pGridCtrl || !strNewText.isEmpty()) return;
    QStringList list = strNewText.split(QRegExp("\\s+"));
    if(list.count() != cellRange.count())
        return;
    int colCount = cellRange.maxCol() - cellRange.minCol() + 1;
    for(int i = cellRange.minRow(); i < cellRange.maxRow();i++)
    {
        for(int j = cellRange.minCol(); j < cellRange.maxCol();j++)
        {
            QString str = list[i*colCount+j];
            m_pGridCtrl->setItemText(i,j,str);
        }
    }
}

//将old回填
void HPasteUndoCommand::undo()
{
    //把删除掉的文字重新set到表格中
    if(!m_pGridCtrl || !strOldText.isEmpty()) return;
    QStringList list = strOldText.split(QRegExp("\\s+"));
    if(list.count() != cellRange.count())
        return;
    int colCount = cellRange.maxCol() - cellRange.minCol() + 1;
    for(int i = cellRange.minRow(); i < cellRange.maxRow();i++)
    {
        for(int j = cellRange.minCol(); j < cellRange.maxCol();j++)
        {
            QString str = list[i*colCount+j];
            m_pGridCtrl->setItemText(i,j,str);
        }
    }
}


//字体
HFontUndoCommand::HFontUndoCommand(HGridCtrl *pGridCtrl, const HCellRange &range, const QFont &newFont, const QFont &oldFont)
    :m_pGridCtrl(pGridCtrl)
{
    m_cellRange = range;
    m_newFont = newFont;
    m_oldFont = oldFont;
}

HFontUndoCommand::~HFontUndoCommand()
{

}

int HFontUndoCommand::id() const
{
    return 0;
}

//将新的回填
void HFontUndoCommand::redo()
{
    if(bFirstTime)
    {
        bFirstTime = false;
        return;
    }
    //把删除掉的文字重新set到表格中
    if(!m_pGridCtrl ) return;
    for(int i = m_cellRange.minRow(); i < m_cellRange.maxRow();i++)
    {
        for(int j = m_cellRange.minCol(); j < m_cellRange.maxCol();j++)
        {
            m_pGridCtrl->setItemFont(i,j,m_newFont);
        }
    }

}

//将old回填
void HFontUndoCommand::undo()
{
    //把删除掉的文字重新set到表格中
    if(!m_pGridCtrl) return;
    for(int i = m_cellRange.minRow(); i < m_cellRange.maxRow();i++)
    {
        for(int j = m_cellRange.minCol(); j < m_cellRange.maxCol();j++)
        {
            m_pGridCtrl->setItemFont(i,j,m_oldFont);
        }
    }
}
