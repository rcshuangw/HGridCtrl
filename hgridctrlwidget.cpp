#include "hgridctrlwidget.h"
#include "hgridctrl.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>

HGridCtrlWidget::HGridCtrlWidget(QWidget* parent)
 :QWidget(parent)
{
    m_bEnableShowEditBar = false;
    m_bEnableShowTab = false;
    m_bEnableVirtualMode = false;
    m_pGridCtrl = new HGridCtrl;
}

HGridCtrlWidget::~HGridCtrlWidget()
{
    reset();
}

void HGridCtrlWidget::reset()
{
    if(m_pGridCtrl)
    {
        delete m_pGridCtrl;
        m_pGridCtrl = NULL;
    }
}

void HGridCtrlWidget::refreshGridCtrl()
{

}

bool HGridCtrlWidget::load(const QString& strFile)
{
    if(strFile.isNull() || strFile.isEmpty())
        return false;
    QFile file(strFile);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QDataStream in(&file);
    int v;
    m_pGridCtrl->load(v,&in);
    return true;
}

bool HGridCtrlWidget::save(const QString& strFile)
{
    //保存表格文件
    if(strFile.isNull() || strFile.isEmpty())
        return false;
    QFile file(strFile);
    if(!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    int v;
    m_pGridCtrl->save(v,&out);
    return true;
}

void HGridCtrlWidget::initReportWidget()
{
    QVBoxLayout* vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    if(m_bEnableShowEditBar)
    {
        QHBoxLayout* hBoxLayout = new QHBoxLayout();
        m_rowColLabel = new QLabel(this);
        m_rowColLabel->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(12);
        m_rowColLabel->setFont(font);
        m_rowColLabel->setFrameShape(QFrame::Box);
        m_rowColLabel->setAlignment(Qt::AlignCenter);

        hBoxLayout->addWidget(m_rowColLabel);

        m_inputLineEdit = new QLineEdit(this);
        m_inputLineEdit->setFont(font);
        m_inputLineEdit->setObjectName(QStringLiteral("lineEdit"));

        hBoxLayout->addWidget(m_inputLineEdit);

        hBoxLayout->setStretch(0, 1);
        hBoxLayout->setStretch(1, 9);
        vBoxLayout->addLayout(hBoxLayout);
        vBoxLayout->addWidget(m_pGridCtrl);
        vBoxLayout->setStretch(0,1);
        vBoxLayout->setStretch(1,9);
        connect(m_inputLineEdit,SIGNAL(textEdited(QString)),this,SLOT(lineedit_textEdited(QString)));
    }
    else
    {
        vBoxLayout->addWidget(m_pGridCtrl);
    }
    setLayout(vBoxLayout);
    connect(m_pGridCtrl,SIGNAL(clicked(HCellID)),this,SLOT(gridCell_clicked(HCellID)));
    rowColRefresh();
    m_pGridCtrl->autoColumnHeader();
    m_pGridCtrl->autoRowHeader();

}

void HGridCtrlWidget::enableShowEditBar(bool b)
{
    m_bEnableShowEditBar = b;
}

void HGridCtrlWidget::enableShowTab(bool b)
{
    m_bEnableShowTab = b;
}

void HGridCtrlWidget::setVirtualMode(bool b)
{
    m_bEnableVirtualMode = b;
    m_pGridCtrl->setVirtualMode(b);
}

//以下属性是打印表格所用
void HGridCtrlWidget::enableHorizontalHeader(bool bEnable)
{
    m_bHorizontalHeader = bEnable;
}

void HGridCtrlWidget::enableVerticalHeader(bool bEnable)
{

    m_bVerticalHeader = bEnable;
}

void HGridCtrlWidget::enablePrintColour(bool b)
{

}

void HGridCtrlWidget::enableShowGrids(bool b)
{

}

void HGridCtrlWidget::setMaxRowCol(int rowMax,int colMax)
{
    m_pGridCtrl->setRowCount(rowMax);
    m_pGridCtrl->setColumnCount(colMax);
    m_pGridCtrl->setFixedColumnCount(1);
    m_pGridCtrl->setFixedRowCount(1);
}

void HGridCtrlWidget::selectedRect(QRect &cellRect)
{
    HCellRange cellRange = m_pGridCtrl->selectedCellRange();
    if(!cellRange.isValid())
    {
        cellRect = QRect();
        return;
    }
    m_pGridCtrl->cellRangeRect(cellRange,cellRect);
}


//对单元格
void HGridCtrlWidget::setText(int row,int col,const QString& s)
{
    if(row > m_pGridCtrl->rowCount() || col > m_pGridCtrl->columnCount())
        return;
    m_pGridCtrl->setItemText(row,col,s);
}

void HGridCtrlWidget::rowColRefresh()
{
    if(m_bHorizontalHeader)
        m_pGridCtrl->setColumnWidth(0,25);
    else
        m_pGridCtrl->setColumnWidth(0,0);

    if(!m_bVerticalHeader)
        m_pGridCtrl->setRowHeight(0,0);
    else
    {
        m_pGridCtrl->setRowHeight(0,m_pGridCtrl->defCellHeight());
    }
}

void HGridCtrlWidget::setFormat(quint32 f)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    for(int rangeRow = range.minRow(); rangeRow <= range.maxRow();rangeRow++)
    {
        for(int rangeCol = range.minCol();rangeCol <= range.maxCol();rangeCol++)
        {
            HGridCellBase* pCell = m_pGridCtrl->getCell(rangeRow,rangeCol);
            if(NULL == pCell) return;

                pCell->setFormat(f);
        }
    }
}

quint32 HGridCtrlWidget::format()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return 0;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return 0;
    return pCell->format();
}

void HGridCtrlWidget::setBorder(GV_BORDER_ITEM* item)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    for(int rangeRow = range.minRow(); rangeRow <= range.maxRow();rangeRow++)
    {
        for(int rangeCol = range.minCol();rangeCol <= range.maxCol();rangeCol++)
        {
            HGridCellBase* pCell = m_pGridCtrl->getCell(rangeRow,rangeCol);
            if(NULL == pCell) return;
            //外边框
            if(item->bBorderOutSide)
            {
                if(rangeRow == range.minRow())
                {
                    pCell->setBorderTopColor(QColor(item->strTopBoderClr));
                    pCell->setBorderTopStyle(item->nTopBorderStyle);
                    pCell->setDrawBorderTop(item->bBorderOutSide);
                }

                if(rangeRow == range.maxRow())//最大行的下一个单元格
                {
                    pCell->setBorderBottomColor(QColor(item->strBottomBoderClr));
                    pCell->setBorderBottomStyle(item->nBottomBorderStyle);
                    pCell->setDrawBorderBottom(item->bBorderOutSide);
                }

                if(rangeCol == range.minCol())
                {
                    pCell->setBorderLeftColor(QColor(item->strLeftBoderClr));
                    pCell->setBorderLeftStyle(item->nLeftBorderStyle);
                    pCell->setDrawBorderLeft(item->bBorderOutSide);
                }

                if(rangeCol == range.maxCol())
                {
                    pCell->setBorderRightColor(QColor(item->strRightBoderClr));
                    pCell->setBorderRightStyle(item->nRightBorderStyle);
                    pCell->setDrawBorderRight(item->bBorderOutSide);
                }
            }
            else
            {
                pCell->setBorderLeftColor(QColor(item->strLeftBoderClr));
                pCell->setBorderLeftStyle(item->nLeftBorderStyle);
                pCell->setDrawBorderLeft(item->bBorderLeft);
                pCell->setBorderRightColor(QColor(item->strRightBoderClr));
                pCell->setBorderRightStyle(item->nRightBorderStyle);
                pCell->setDrawBorderRight(item->bBorderRight);
                pCell->setBorderTopColor(QColor(item->strTopBoderClr));
                pCell->setBorderTopStyle(item->nTopBorderStyle);
                pCell->setDrawBorderTop(item->bBorderTop);
                pCell->setBorderBottomColor(QColor(item->strBottomBoderClr));
                pCell->setBorderBottomStyle(item->nBottomBorderStyle);
                pCell->setDrawBorderBottom(item->bBorderBottom);
            }

            //最小行的上一个单元格
            if(rangeRow == range.minRow())
            {
                HGridCellBase* pRowOutCell = m_pGridCtrl->getCell(rangeRow-1,rangeCol);
                if(pRowOutCell)
                {
                    pRowOutCell->setDrawBorderBottom(pCell->isDrawBorderTop());
                    pRowOutCell->setBorderBottomColor(pCell->borderTopColor());
                    pRowOutCell->setBorderBottomStyle(pCell->borderTopStyle());
                }
            }

            if(rangeRow == range.maxRow())//最大行的下一个单元格
            {
                HGridCellBase* pRowOutCell = m_pGridCtrl->getCell(rangeRow+1,rangeCol);
                if(pRowOutCell)
                {
                    pRowOutCell->setDrawBorderTop(pCell->isDrawBorderBottom());
                    pRowOutCell->setBorderTopColor(pCell->borderBottomColor());
                    pRowOutCell->setBorderTopStyle(pCell->borderBottomStyle());
                }
            }

            if(rangeCol == range.minCol())
            {
                HGridCellBase* pRowOutCell = m_pGridCtrl->getCell(rangeRow,rangeCol-1);
                if(pRowOutCell)
                {
                    pRowOutCell->setDrawBorderRight(pCell->isDrawBorderLeft());
                    pRowOutCell->setBorderRightColor(pCell->borderLeftColor());
                    pRowOutCell->setBorderRightStyle(pCell->borderLeftStyle());
                }
            }

            if(rangeCol == range.maxCol())
            {
                HGridCellBase* pRowOutCell = m_pGridCtrl->getCell(rangeRow,rangeCol+1);
                if(pRowOutCell)
                {
                    pRowOutCell->setDrawBorderLeft(pCell->isDrawBorderRight());
                    pRowOutCell->setBorderLeftColor(pCell->borderRightColor());
                    pRowOutCell->setBorderLeftStyle(pCell->borderRightStyle());
                }
            }
        }
    }
}

void HGridCtrlWidget::getBorder(GV_BORDER_ITEM* item)
{
    if(NULL == item) return;
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return;
    item->nLeftBorderStyle = pCell->borderLeftStyle();
    item->nRightBorderStyle = pCell->borderRightStyle();
    item->nTopBorderStyle = pCell->borderTopStyle();
    item->nBottomBorderStyle = pCell->borderBottomStyle();

    item->bBorderLeft   = pCell->isDrawBorderLeft();
    item->bBorderRight  = pCell->isDrawBorderRight();
    item->bBorderTop    = pCell->isDrawBorderTop();
    item->bBorderBottom = pCell->isDrawBorderBottom();

    item->strLeftBoderClr   = pCell->borderLeftColor().name();
    item->strRightBoderClr  = pCell->borderRightColor().name();
    item->strTopBoderClr    = pCell->borderTopColor().name();
    item->strBottomBoderClr = pCell->borderBottomColor().name();
}


void HGridCtrlWidget::enableResetFormat(bool b)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    if(b)
    {
        m_pGridCtrl->clearFormats(range);
    }
}

void HGridCtrlWidget::enableResetText(bool b)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    if(b)
    {
        m_pGridCtrl->clearFormatting(range);
    }
}

void HGridCtrlWidget::enableResetAllFormat(bool b)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    if(b)
    {
        m_pGridCtrl->clearCells(range);
    }
}

void HGridCtrlWidget::setFont(const QFont& font)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    for(int rangeRow = range.minRow(); rangeRow <= range.maxRow();rangeRow++)
    {
        for(int rangeCol = range.minCol();rangeCol <= range.maxCol();rangeCol++)
        {
            HGridCellBase* pCell = m_pGridCtrl->getCell(rangeRow,rangeCol);
            if(NULL == pCell) return;

                pCell->setFont(font);
        }
    }
}

QFont HGridCtrlWidget::font()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return QFont();
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return QFont();
    return pCell->font();
}

void HGridCtrlWidget::setTextColor(const QString& s)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    for(int rangeRow = range.minRow(); rangeRow <= range.maxRow();rangeRow++)
    {
        for(int rangeCol = range.minCol();rangeCol <= range.maxCol();rangeCol++)
        {
            HGridCellBase* pCell = m_pGridCtrl->getCell(rangeRow,rangeCol);
            if(NULL == pCell) return;

                pCell->setTextClr(s);
        }
    }
}

QString HGridCtrlWidget::textColor()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return "";
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return "";
    return pCell->textClr().name();
}

void HGridCtrlWidget::setTextBkColor(const QString& s)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return;
    for(int rangeRow = range.minRow(); rangeRow <= range.maxRow();rangeRow++)
    {
        for(int rangeCol = range.minCol();rangeCol <= range.maxCol();rangeCol++)
        {
            HGridCellBase* pCell = m_pGridCtrl->getCell(rangeRow,rangeCol);
            if(NULL == pCell) return;

                pCell->setBackClr(s);
        }
    }
}

QString HGridCtrlWidget::textBkColor()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return "";
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return "";
    return pCell->backClr().name();
}

bool HGridCtrlWidget::mergeCell()
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid() || range.count() <=1)
        return false;
    m_pGridCtrl->setMergeSelectedCells();
    return true;
}

bool HGridCtrlWidget::isMergeCell()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return false;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return false;
    return pCell->isMerged();
}

bool HGridCtrlWidget::setRowHeight(int height)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    for(int rangeRow = range.minRow(); rangeRow <= range.maxRow();rangeRow++)
        m_pGridCtrl->setRowHeight(rangeRow,height);
    return true;
}

bool HGridCtrlWidget::setColumnWidth(int width)
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    for(int rangeCol = range.minCol();rangeCol <= range.maxCol();rangeCol++)
        m_pGridCtrl->setColumnWidth(rangeCol,width);
    return true;
}

int HGridCtrlWidget::rowHeight()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return 0;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return 0;
    return m_pGridCtrl->rowHeight(cellID.row);
}

int HGridCtrlWidget::columnWidth()
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return 0;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return 0;
    return m_pGridCtrl->columnWidth(cellID.col);
}

bool HGridCtrlWidget::splitCell()
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    m_pGridCtrl->setSplitSelectedCells();
    return true;
}

//行列插入最后位置
bool HGridCtrlWidget::insertGridRow()
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    int nStartRow = range.minRow();
    int nRowNum = range.maxRow() - range.minRow() + 1;
    if(m_pGridCtrl->rowCount() + nRowNum > ROWMAX_COUNT)
        return false;

    for(int i = 0; i < nRowNum; i++)
    {
       if((int)-1 == m_pGridCtrl->insertRow("",nStartRow))//strHeader是插入之后会自动刷新
           return false;
    }
    m_pGridCtrl->setSelectedRange(range);
    return true;
}


bool HGridCtrlWidget::insertGridColumn()
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    int nStartCol = range.minCol();
    int nColNum = range.maxCol() - range.minCol() + 1;
    if(m_pGridCtrl->columnCount() + nColNum > COLMAX_COUNT)
        return false;

    quint32 nFormat = m_pGridCtrl->defaultCell(false,false)->format();
    for(int i = 0; i < nColNum; i++)
    {
       if((int)-1 == m_pGridCtrl->insertColumn("",nFormat,nStartCol))//strHeader是插入之后会自动刷新
           return false;
    }
    m_pGridCtrl->setSelectedRange(range);
    return true;
}

//删除行列
bool HGridCtrlWidget::removeGridRow()
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    for(int row = range.minRow(); row <= range.maxRow(); row++)
    {
       if(!m_pGridCtrl->deleteRow(row))//strHeader是插入之后会自动刷新
           return false;
    }
    return true;
}

bool HGridCtrlWidget::removeGridColumn()
{
    HCellRange range = m_pGridCtrl->selectedCellRange();
    if(!range.isValid())
        return false;
    for(int col = range.minCol(); col <= range.maxCol(); col++)
    {
       if(!m_pGridCtrl->deleteColumn(col))//strHeader是插入之后会自动刷新
           return false;
    }
    return true;
}

void HGridCtrlWidget::autoSizeRows()
{
    if(!m_pGridCtrl)
        return;
    m_pGridCtrl->autoSizeRows();
}

void HGridCtrlWidget::autoSizeColumns()
{
    if(!m_pGridCtrl)
        return;
    m_pGridCtrl->autoSizeColumns();
}

void HGridCtrlWidget::lineedit_textEdited(const QString &text)
{
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return;
    pCell->setText(text);
}

void HGridCtrlWidget::gridCell_clicked(HCellID)
{
    //lineedit和行列标签的变化
    HCellID cellID = m_pGridCtrl->focusCell();
    if(!cellID.isValid()) return;
    HGridCellBase* pCell = m_pGridCtrl->getCell(cellID);
    if(!pCell) return;

    if(m_bEnableShowEditBar)
    {
        //设置编辑框的文字
        m_inputLineEdit->setText(pCell->text());
        //设置Label文字
        pCell = m_pGridCtrl->getCell(0,cellID.col);
        if(!pCell) return;
        QString strRow = pCell->text();
        pCell = m_pGridCtrl->getCell(cellID.row,0);
        if(!pCell) return;
        QString strCol = pCell->text();
        m_rowColLabel->setText(strRow+strCol);
    }
    emit gridcellclicked();
}
