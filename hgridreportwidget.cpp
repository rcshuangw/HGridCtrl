#include "hgridreportwidget.h"
#include <QTabBar>
#include <QVBoxLayout>
#include "hgridctrlwidget.h"

HGridReportWidget::HGridReportWidget(QWidget *parent)
   :QWidget(parent)
{
    m_bEnableShowEditBar = false;
    m_bEnableShowTab = false;
    m_bEnableVirtual = false;
    m_bHorizontalHeader = true;
    m_bVerticalHeader = true;
    m_bShowGrids = true;
    m_bPrintColour = true;
    m_tabWidget = new QTabWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_tabWidget);
    m_tabWidget->setTabPosition(QTabWidget::South);
    m_nNumSheets = 0;
}

HGridReportWidget::~HGridReportWidget()
{

}

void HGridReportWidget::setMaxRow(int row)
{
    m_nRow = row;
}

void HGridReportWidget::setMaxCol(int col)
{
    m_nCol = col;
}

void HGridReportWidget::setText(int row,int col,const QString& s)
{
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            return w->setText(row,col,s);
        }
    }
}

bool HGridReportWidget::load(const QString& strFile)
{
    m_strGridCtrlFile = strFile;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            return w->load(strFile);
        }
    }
}

bool HGridReportWidget::save(const QString& strFile)
{
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            return w->save(strFile);
        }
    }
}

void HGridReportWidget::init()
{
    clear();
    for(int i = 0; i < m_nNumSheets; i++)
    {
        HGridCtrlWidget* w = new HGridCtrlWidget(this);
        connect(w,SIGNAL(gridcellclicked()),this,SLOT(gridCell_clicked()));
        setGridCtrlAttr(w);
        w->setMaxRowCol(m_nRow,m_nCol);
        w->initReportWidget();
        QString str = QString(QStringLiteral("第%1页")).arg(i+1);
        m_tabWidget->insertTab(i,w,str);
    }

    if(!m_bEnableShowTab)
        m_tabWidget->tabBar()->hide();
    else
        m_tabWidget->tabBar()->show();
}

void HGridReportWidget::update()
{
    int nAddCount = m_nNumSheets - m_tabWidget->count();
    if(nAddCount < 0)
    {
        while (m_tabWidget->count() > m_nNumSheets) {
            int index = m_tabWidget->count()-1;
            HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
            if(w)
            {
                delete w;
                w = NULL;
            }
            m_tabWidget->removeTab(index);
        }
    }
    else
    {
        int num = m_tabWidget->count();
        for(int i = 0; i < nAddCount; i++)
        {
            HGridCtrlWidget* w = new HGridCtrlWidget(this);
            connect(w,SIGNAL(gridcellclicked()),this,SLOT(gridCell_clicked()));
            setGridCtrlAttr(w);
            w->load(m_strGridCtrlFile);
            w->initReportWidget();
            QString str = QString(QStringLiteral("第%1页")).arg(num+1);
            m_tabWidget->insertTab(i,w,str);
        }
    }
    if(!m_bEnableShowTab)
        m_tabWidget->tabBar()->hide();
    else
        m_tabWidget->tabBar()->show();
}

void HGridReportWidget::clear()
{
    while (m_tabWidget->count()) {
        int index = m_tabWidget->count() - 1;
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
        if(w)
        {
            delete w;
            w = NULL;
        }
        m_tabWidget->removeTab(index);
    }
}

void HGridReportWidget::setGridCtrlAttr(QWidget *w)
{
    if(!w) return;
    HGridCtrlWidget* pGridCtrlWidget = (HGridCtrlWidget*)w;
    pGridCtrlWidget->enableShowEditBar(m_bEnableShowEditBar);
    pGridCtrlWidget->enableShowTab(m_bEnableShowTab);
    pGridCtrlWidget->setVirtualMode(m_bEnableVirtual);
    pGridCtrlWidget->enableHorizontalHeader(m_bHorizontalHeader);
    pGridCtrlWidget->enableVerticalHeader(m_bVerticalHeader);
    pGridCtrlWidget->enablePrintColour(m_bPrintColour);
    pGridCtrlWidget->enableShowGrids(m_bShowGrids);
}

void HGridReportWidget::setEditorGridReportAttr()
{
    enableShowEditBar(true);
    enableShowTab(false);
    enableVritual(false);
}

void HGridReportWidget::setBrowserGridReportAttr()
{
    //浏览框就是打印页，有很多属性和编辑框是不一样
    enableShowEditBar(false);
    enableShowTab(true);
    enableVritual(true);

    enableHorizontalHeader(false);
    enableVerticalHeader(false);
    enableShowGrids(true);
    enablePrintColour(false);
}

QString HGridReportWidget::tabName()
{
    return "";
}

void HGridReportWidget::setTabName(const QString& tableName)
{

}

QString HGridReportWidget::sheetName(short nSheet)
{
    if(nSheet > numSheet())
        return "";
    return m_tabWidget->tabBar()->tabText(nSheet);
}

void HGridReportWidget::setSheetName(short nSheet, const QString& strNewName)
{
    if(nSheet > numSheet())
        return;
    m_tabWidget->tabBar()->setTabText(nSheet,strNewName);
}

void HGridReportWidget::setNumSheet(long num)
{
    m_nNumSheets = num;
}

long HGridReportWidget::numSheet()
{
    return m_nNumSheets;
}

void HGridReportWidget::setSheet(long index)
{
    if(index > numSheet())
        return;
    if(m_tabWidget->tabBar()->currentIndex() != index)
    {
        m_tabWidget->tabBar()->setCurrentIndex(index);
    }
}

long HGridReportWidget::sheet()
{
    return 1;
}

void HGridReportWidget::enableShowEditBar(bool b)
{
    m_bEnableShowEditBar = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->enableShowEditBar(b);
        }
    }
}

void HGridReportWidget::enableShowTab(bool b)
{
    m_bEnableShowTab = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->enableShowTab(b);
        }
    }
}

void HGridReportWidget::enableVritual(bool b)
{
    m_bEnableVirtual = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->setVirtualMode(b);
        }
    }
}

void HGridReportWidget::enableHorizontalHeader(bool b)
{
    m_bHorizontalHeader = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->enableHorizontalHeader(b);
        }
    }
}

void HGridReportWidget::enableVerticalHeader(bool b)
{
    m_bVerticalHeader = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->enableVerticalHeader(b);
        }
    }
}

void HGridReportWidget::enablePrintColour(bool b)
{
    m_bPrintColour = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->enablePrintColour(b);
        }
    }
}

void HGridReportWidget::enableShowGrids(bool b)
{
    m_bShowGrids = b;
    int nTabNum = m_tabWidget->count();
    for(int i = 0; i < nTabNum; i++)
    {
        HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(i);
        if(w)
        {
            w->enableShowGrids(b);
        }
    }
}

void HGridReportWidget::paste()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        HGridCtrl* pGridCtrl = w->gridCtrl();
        if(pGridCtrl)
            pGridCtrl->onEditPaste();
    }
}

void HGridReportWidget::cut()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        HGridCtrl* pGridCtrl = w->gridCtrl();
        if(pGridCtrl)
            pGridCtrl->onEditCut();
    }
}

void HGridReportWidget::copy()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        HGridCtrl* pGridCtrl = w->gridCtrl();
        if(pGridCtrl)
            pGridCtrl->onEditCopy();
    }
}

//单元格属性设置
void HGridReportWidget::setFormat(quint32 f)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->setFormat(f);
    }
}

quint32 HGridReportWidget::format()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->format();
    }
    return 0;
}

void HGridReportWidget::setBorder(GV_BORDER_ITEM* item)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->setBorder(item);
    }
}

void HGridReportWidget::getBorder(GV_BORDER_ITEM* item)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->getBorder(item);
    }
}

void HGridReportWidget::enableResetFormat(bool b)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->enableResetFormat(b);
    }
}

void HGridReportWidget::enableResetText(bool b)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->enableResetText(b);
    }
}

void HGridReportWidget::enableResetAllFormat(bool b)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->enableResetAllFormat(b);
    }
}

//字体
void HGridReportWidget::setFont(const QFont& font)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->setFont(font);
    }
}

QFont HGridReportWidget::font()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->font();
    }
    return QFont();
}

//颜色
void HGridReportWidget::setTextColor(const QString& s)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->setTextColor(s);
    }
}

QString HGridReportWidget::textColor()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->textColor();
    }
    return "";
}

void HGridReportWidget::setTextBkColor(const QString& s)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->setTextBkColor(s);
    }
}

QString HGridReportWidget::textBkColor()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->textBkColor();
    }
    return "";
}

bool HGridReportWidget::setRowHeight(int height)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->setRowHeight(height);
    }
    return false;
}

bool HGridReportWidget::setColumnWidth(int width)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->setColumnWidth(width);
    }
    return false;
}

int  HGridReportWidget::rowHeight()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->rowHeight();
    }
    return 0;
}

int  HGridReportWidget::columnWidth()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->columnWidth();
    }
    return 0;
}

void HGridReportWidget::selectedRect(QRect& rect)
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->selectedRect(rect);
    }
}

//操作
bool HGridReportWidget::mergeCell()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->mergeCell();
    }
    return false;
}

bool HGridReportWidget::isMerged()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->isMergeCell();
    }
    return false;
}

bool HGridReportWidget::splitCell()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->splitCell();
    }
    return false;
}

//操作:插入删除行列部分
bool HGridReportWidget::insertGridRow()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->insertGridRow();
    }
    return false;
}

bool HGridReportWidget::insertGridColumn()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->insertGridColumn();
    }
    return false;
}

bool HGridReportWidget::removeGridRow()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->removeGridRow();
    }
    return false;
}

bool HGridReportWidget::removeGridColumn()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        return w->removeGridColumn();
    }
    return false;
}

void HGridReportWidget::autoSizeRows()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->autoSizeRows();
    }
}

void HGridReportWidget::autoSizeColumns()
{
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        w->autoSizeColumns();
    }
}

//打印部分
/*
void HGridReportWidget::printPreview()
{
    HReportPrint print(this);
    print.printPreview();
}

void HGridReportWidget::print()
{
    HReportPrint print(this);
    print.print();
}
*/
void HGridReportWidget::gridCell_clicked()
{
    //获取格式信息
    int index = m_tabWidget->currentIndex();
    HGridCtrlWidget* w = (HGridCtrlWidget*)m_tabWidget->widget(index);
    if(w)
    {
        //w->cellFormat(m_pReportManager->formatSet());
    }
    emit gridcellclicked();
}
