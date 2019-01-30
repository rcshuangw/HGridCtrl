#include "hreportprint.h"
#include <QPrinter>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QFontMetrics>
#include "hgridctrl.h"
#include "QDateTime"
#include "hgridreportwidget.h"
#include "hgridctrlwidget.h"
HPrintInfo::HPrintInfo()
{

}

HPrintInfo::~HPrintInfo()
{

}

HReportPrint::HReportPrint(HGridReportWidget* widget)
    :m_pGridReportWidget(widget)
{
    m_PrintInfo.m_pGridCtrl = NULL;
    m_PrintInfo.m_rectDraw = QRect();
    m_nLeftMargin = 1;
    m_nRightMargin = 1;
    m_nGap = 0;
    m_nHeaderHeight = 2;
    m_nFooterHeight = 1;
}

void HReportPrint::onPrintBegin(QPainter *pDC, HPrintInfo *pInfo)
{
    //本函数只是求出页面宽度和表格宽度和页面高度
    Q_ASSERT(pDC && pInfo);
    if (!pDC || !pInfo) return;
    // Create the printer font
    pDC->save();
    int nFontSize = 10;
    QString strFontName = "宋体";
    m_PrinterFont = QFont(strFontName,nFontSize);

    pDC->setFont(m_PrinterFont);

    // Get the average character width (in GridCtrl units) and hence the margins
    QFontMetrics fontMetrics(m_PrinterFont);
    m_CharSize = fontMetrics.size(Qt::TextSingleLine,("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"));
    m_CharSize.setWidth(m_CharSize.width()/52);
    int nMargins = (m_nLeftMargin+m_nRightMargin)*m_CharSize.width(); //边距

    // 页面大小
    m_PaperSize = QSize(pDC->window().width(),pDC->window().height());
    m_LogicalPageSize.setWidth(pInfo->m_pGridCtrl->virtualWidth()+nMargins);
    m_LogicalPageSize.setHeight(int(m_LogicalPageSize.width()*m_PaperSize.height()/m_PaperSize.height()));
    m_nPageHeight = m_LogicalPageSize.height() - (m_nHeaderHeight+m_nFooterHeight + 2*m_nGap)*m_CharSize.height();

    //页面宽度 = 逻辑页面宽度 - 固定列宽 - 边距
    m_nPageWidth = m_LogicalPageSize.width() - nMargins;
    m_nPageMultiplier = 1;
    pDC->restore();
}

//Qt打印的边缘由Qt本身提供接口设置
void HReportPrint::onPrint(QPainter *pDC, HPrintInfo *pInfo)
{
    if (!pDC || !pInfo)
        return;
    //打印的原点仍然在左上角
    int transPointX = 0;
    int transPointY = 0;
    bool bEnableShowRow = false;
    bool bEnableShowCol = false;

    pDC->translate(0,0);
    pDC->setWindow(QRect(QPoint(0,0),m_LogicalPageSize));
    pDC->setViewport(QRect(QPoint(0,0),m_PaperSize));
    transPointX = m_nLeftMargin * m_CharSize.width();
    pDC->setFont(m_PrinterFont);
    HGridCtrl* pGridCtrl = pInfo->m_pGridCtrl;
    if(!pGridCtrl) return;

    // Header
    pInfo->m_rectDraw.setTop(0);
    pInfo->m_rectDraw.setLeft(m_nLeftMargin * m_CharSize.width());
    pInfo->m_rectDraw.setRight(m_LogicalPageSize.width() - (0 + m_nRightMargin) * m_CharSize.width());
    pInfo->m_rectDraw.setBottom(m_nHeaderHeight * m_CharSize.height());

    //报表头文字
    printHeader(pDC, pInfo);

    //移动原点到报表头下面
    transPointY += m_nHeaderHeight * m_CharSize.height();

    //头和表格之间的空间
    transPointY += 5;//m_nGap * m_CharSize.height();

    //表格居中打印
    int iColumnOffset = 0;
    int nColumnCount = 0;
    if(bEnableShowCol)
        nColumnCount = pGridCtrl->columnCount();
    else
        nColumnCount = pGridCtrl->columnCount() - pGridCtrl->fixedColumnCount();
    for(int i1=0; i1 < nColumnCount; i1++)
    {
        iColumnOffset += pGridCtrl->columnWidth( i1);
    }

    int deltaX = m_nPageWidth - iColumnOffset;
    if(deltaX > 0)
        deltaX = deltaX / 2;
    //transPointX += deltaX;

    //移动绘制表格原点到transPointX,transPointY处
    pDC->translate(transPointX,transPointY);

    //初始行列为0
    m_nCurrPrintRow = 0;
    m_nPrintColumn = 0;

    //如果要打印固定列则打印
    if(bEnableShowRow && pGridCtrl->fixedRowCount() > 0)
        printRowHeadings(pDC, pInfo);   // 打印每行的表头
    else
        m_nCurrPrintRow += pGridCtrl->fixedRowCount();//如果不打印固定行头 则行移动到第一个非固定列出


    if(bEnableShowCol && pGridCtrl->fixedColumnCount() > 0)
    {       
       PrintColumnHeadings(pDC, pInfo);
    }
    else
    {
        m_nPrintColumn = pGridCtrl->fixedColumnCount();
    }

    bool bFirstPrintedRow = true;
    QRect rect;
    rect.setBottom(-1);
    while (m_nCurrPrintRow < pGridCtrl->rowCount())
    {
        rect.setTop(rect.bottom()+1);
        rect.setBottom(rect.top() + pGridCtrl->rowHeight(m_nCurrPrintRow) - 1);

        if (rect.bottom() > m_nPageHeight) break;            // Gone past end of page

        rect.setRight(-1);

        // modified to allow printing of wide grids on multiple pages
        for (int col = m_nPrintColumn; col < pGridCtrl->columnCount(); col++)
        {
            rect.setLeft(rect.right() + 1);
            rect.setRight(rect.left() + pGridCtrl->columnWidth(col) -1);

            if( rect.right() > m_nPageWidth)
                break;

            HGridCellBase* pCell = pGridCtrl->getCell(m_nCurrPrintRow, col);
            if (pCell)
            {
                int row=m_nCurrPrintRow;
                if(!pCell->isMerged())
                {
                    if(!pCell->isMergeWithOthers())
                    {
                        pCell->printCell(pDC, row, col, rect);
                    }
                }
                else
                {
                    QRect mergerect=rect;
                    if(pGridCtrl->cellRangeRect(pCell->mergeRange(),mergerect))
                    {
                        mergerect.adjust(0,0,-1,-1);
                        pCell->printCell(pDC, row, col, mergerect);
                    }
                }
            }
        }
        m_nCurrPrintRow++;
        bFirstPrintedRow = false;
    }

    // Footer
    pDC->translate(-transPointX,-transPointY);
    pInfo->m_rectDraw.setTop(m_LogicalPageSize.height() - m_nFooterHeight * m_CharSize.height());
    pInfo->m_rectDraw.setLeft(0);
    pInfo->m_rectDraw.setRight(m_LogicalPageSize.width() - (0 + m_nRightMargin) * m_CharSize.width());
    pInfo->m_rectDraw.setBottom(m_LogicalPageSize.height());
    printFooter(pDC, pInfo);

    pDC->restore();
}

void HReportPrint::printRowHeadings(QPainter *pDC, HPrintInfo* pInfo)
{
    if(!pDC || !pInfo)
        return;
    pDC->save();
    pDC->setFont(m_PrinterFont);

    QRect rect;
    rect.setRight(-1);

    for( int iCol = 0; iCol < pInfo->m_pGridCtrl->fixedColumnCount(); iCol++)
    {
        rect.setLeft(rect.right() + 1);
        rect.setRight(rect.left() + pInfo->m_pGridCtrl->columnWidth( iCol) - 1);

        rect.setBottom(-1);
        for( int iRow = m_nCurrPrintRow; iRow < pInfo->m_pGridCtrl->rowCount(); iRow++)
        {
            rect.setTop(rect.bottom() + 1);
            rect.setBottom(rect.top() + pInfo->m_pGridCtrl->rowHeight( iRow) - 1);

            if( rect.bottom() > m_nPageHeight)
                break;

            HGridCellBase* pCell = pInfo->m_pGridCtrl->getCell(iRow, iCol);
            if (pCell)
                pCell->printCell(pDC, iRow, iCol, rect);
        }
    }
    pDC->restore();
}

void HReportPrint::PrintColumnHeadings(QPainter *pDC, HPrintInfo* pInfo)
{
    if(!pDC || !pInfo)
        return;
    pDC->save();
    pDC->setFont(m_PrinterFont);

    QRect rect;
    rect.setBottom(-1);

    bool bFirst = true;
    bool bOriginal;

    // modified to allow column hdr printing of multi-page wide grids
    for (int row = 0; row < pInfo->m_pGridCtrl->fixedRowCount(); row++)
    {
        rect.setTop(rect.bottom() + 1);
        rect.setBottom(rect.top() + pInfo->m_pGridCtrl->rowHeight(row) - 1);

        rect.setRight(-1);
        for (int col = m_nPrintColumn; col < pInfo->m_pGridCtrl->columnCount(); col++)
        {
           rect.setLeft(rect.right() + 1);
           rect.setRight(rect.left() + pInfo->m_pGridCtrl->columnWidth( col) - 1);

           if( rect.right() > m_nPageWidth)
              break;

           HGridCellBase* pCell = pInfo->m_pGridCtrl->getCell(row, col);
           if(pCell)
               pCell->printCell(pDC, row, col, rect);
        }

    }
    pDC->restore();
}


void HReportPrint::onPrintEnd(QPainter *p, HPrintInfo *pInfo)
{

}

void HReportPrint::printHeader(QPainter *pDC, HPrintInfo *pInfo)
{
    QRect   rc(pInfo->m_rectDraw);
    if( !m_strHeaderText.isEmpty() )
        pDC->drawText( rc, QDT_SINGLELINE | QDT_NOPREFIX | QDT_CENTER, m_strHeaderText);
    // draw ruled-line across top
    pDC->save();
    QPen pen(Qt::black);
    pen.setWidth(1);
    pDC->setPen(pen);
    pDC->drawLine(QPoint(rc.left(), rc.bottom()),QPoint(rc.right(), rc.bottom()));
    pDC->restore();
}

void HReportPrint::printFooter(QPainter *pDC, HPrintInfo *pInfo)
{
    if(!pDC || !pInfo)
        return;
    QRect rc(pInfo->m_rectDraw);
    //先画条线 如果不需要可以删除
    QPen pen(Qt::black);
    pen.setWidth(1);
    pDC->setPen(pen);
    pDC->drawLine(QPoint(rc.left(), rc.top()),QPoint(rc.right(), rc.top()));

    if( !m_strFooterText.isEmpty() )
       pDC->drawText(rc,QDT_SINGLELINE | QDT_NOPREFIX | QDT_CENTER,m_strFooterText);
    pDC->restore();
}

void HReportPrint::printPage(QPainter* p)
{
    onPrintBegin(p,&m_PrintInfo);
    onPrint(p,&m_PrintInfo);
    onPrintEnd(p,&m_PrintInfo);
}

void HReportPrint::print()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,NULL);
    if (printDialog.exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        //预览打印是支持分页打印的
        int firstPage = printer.fromPage() - 1;
        if(firstPage >= m_pGridReportWidget->numSheet())
            return;
        if(firstPage == (int)-1)
            firstPage = 0;
        int lastPage = printer.toPage() - 1;
        if(lastPage == -1 || lastPage >= m_pGridReportWidget->numSheet())
            lastPage = m_pGridReportWidget->numSheet();
        int numPages = lastPage - firstPage - 1;

        for(int i = 0;i < printer.numCopies();i++)
        {
            for(int j= 0;j < numPages;j++) //页数
            {
              if(i != 0 || j != 0)
                  printer.newPage();
              HGridCtrl* pGridCtrl = ((HGridCtrlWidget*)m_pGridReportWidget->m_tabWidget->widget(j))->gridCtrl();
              if(!pGridCtrl) continue;
              m_PrintInfo.m_pGridCtrl = pGridCtrl;
              m_PrintInfo.m_nCurPage = j;
              initPrintParams();
              printPage(&painter);
            }
        }
    }
}

void HReportPrint::printPreview()
{
    QPrinter printer;
    QPrintPreviewDialog dlg(&printer,NULL);
    connect(&dlg, SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    dlg.exec();
}

void HReportPrint::printPreview(QPrinter* p)
{
    QPainter painter(p);
    for(int j= 0;j < m_pGridReportWidget->numSheet() ;j++) //页数
    {
      if(j != 0)
          p->newPage();

      HGridCtrl* pGridCtrl = ((HGridCtrlWidget*)m_pGridReportWidget->m_tabWidget->widget(j))->gridCtrl();
      if(!pGridCtrl) continue;
      m_PrintInfo.m_pGridCtrl = pGridCtrl;
      m_PrintInfo.m_nCurPage = j;
      initPrintParams();
      printPage(&painter);
    }

}

void HReportPrint::initPrintParams()
{
    if(!m_PrintInfo.m_pGridCtrl)
        return;
    int nGap;
    m_PrintInfo.m_pGridCtrl->printMarginInfo(m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,m_nRightMargin, m_nTopMargin, m_nBottomMargin,nGap);
    m_PrintInfo.m_pGridCtrl->printHeadFoot(m_strHeaderText,m_strFooterText);
    m_PrintInfo.m_pGridCtrl->printOther(m_bHorizontalHeader,m_bVerticalHeader, m_bShowGrids,m_bPrintColour);
}

void HReportPrint::printPages(QPrinter* p)
{

}

