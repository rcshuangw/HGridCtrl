 #include "hgridcellbase.h"
#include "hgridctrl.h"
 HGridCellBase::HGridCellBase(QObject* parent)
 {
     reset();
 }

HGridCellBase::~HGridCellBase()
{

}

void HGridCellBase::load(int v,QDataStream* ds)
{
    if(!ds) return;
    int n;
    *ds>>n;
    m_nState = n;
    *ds>>n;
    m_MergeCellID.row = n;
    *ds>>n;
    m_MergeCellID.col = n;
    bool b;
    *ds>>b;
    m_bMergeWithOthers = b;
    *ds>>b;
    m_bShow = b;

    m_MergeRange.set();
    *ds>>m_MergeRange;

}

void HGridCellBase::save(int v,QDataStream* ds)
{
    if(!ds) return;
    int n;
    *ds<<(quint32)m_nState;
    *ds<<(int)m_MergeCellID.row;
    *ds<<(int)m_MergeCellID.col;
    *ds<<(bool)m_bMergeWithOthers;
    *ds<<(bool)m_bShow;
    *ds<<m_MergeRange;
}

void HGridCellBase::operator=( HGridCellBase& cell)
{
    if (this == &cell) return;

    setGrid(cell.grid());    // do first in case of dependencies

    setText(cell.text());
    setImage(cell.image());
    setData(cell.data());
    setState(cell.state());
    setFormat(cell.format());
    setTextClr(cell.textClr());
    setBackClr(cell.backClr());
    setFont(cell.font());
    setMargin(cell.margin());

    setMergeCellID(cell.mergeCellID());
    setMergeRange(cell.mergeRange());
    setShow(cell.isShow());

}

HGridCellBase* HGridCellBase::defaultCell() const
{
    if (grid())
        return grid()->defaultCell(isFixedRow(), isFixedCol());
    return NULL;
}

void HGridCellBase::reset()
{
    m_nState  = 0;
    m_bShow = true;
    m_MergeCellID.row = -1;
    m_MergeCellID.col = -1;
    m_bMergeWithOthers = false;
    m_MergeRange.set();
}

bool HGridCellBase::draw(QPainter* painter, int nRow, int nCol, QRect rect, bool bEraseBkgnd)
{
    // Note - all through this function we totally brutalise 'rect'. Do not
        // depend on it's value being that which was passed in.

    //有合并同时不显示此单元格
    if(	!isShow() && !isMerged())
        return true;

    HGridCtrl* pGrid = grid();
    Q_ASSERT(pGrid);

    if (!pGrid || !painter)
        return false;

    if( rect.width() <= 0 || rect.height() <= 0)  // prevents imagelist item from drawing even
        return false;                             //  though cell is hidden

    //单元格的边框可以在这里绘制，如果设置了边框颜色和线条格式，就按照设置项绘制，注意：如果绘制bottom要获取同列的下一行表格，top要设置和bottom一样的风格。
    painter->save();
    painter->setBackgroundMode(Qt::TransparentMode);
    //增加绘制表格外框
    QRect rectBoard = rect.adjusted(-1,-1,0,0);

    // Get the default cell implementation for this kind of cell. We use it if this cell
    // has anything marked as "default"
    HGridDefaultCell *pDefaultCell = (HGridDefaultCell*) defaultCell();
    if (!pDefaultCell)
        return false;

    // Set up text and background colours
    QColor TextClr, TextBkClr;
    TextBkClr = backClr();
    TextClr = textClr();
    /*TextClr = (textClr() == QColor(QCLR_DEFAULT))? pDefaultCell->textClr() : textClr();
    if (backClr() == QColor(QCLR_DEFAULT))
        TextBkClr = pDefaultCell->backClr();
    else
    {
        bEraseBkgnd = true;
        TextBkClr = backClr();
    }*/
    if ( isFocused() || isDropHighlighted() )
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        //如果选择的cell背景和字体都要变化就要用此功能
        if(state() & GVIS_SELECTED)
        {
            TextBkClr = backClr();
            TextClr = textClr();
            bEraseBkgnd = true;
        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->gridLines() != GVL_NONE)
        {
            rect.adjust(0,0,-2,-2);
        }

        if (bEraseBkgnd)
        {
            QBrush brush(TextBkClr);
            painter->fillRect(rect, brush);
        }

        if (pGrid->isFrameFocusCell())
        {
            // Use same color as text to outline the cell so that it shows
            // up if the background is black.
            QPen fPen(Qt::black);
            painter->setPen(fPen);
            painter->drawRect(rect);
        }

        painter->setPen(QPen(TextClr));

        // Adjust rect after frame draw if no grid lines
        if(pGrid->gridLines() == GVL_NONE)
        {
            rect.adjust(0,0,2,2);;
        }
        rect = rect.marginsAdded(QMargins(0,1,1,1));
    }
    else if ((state() & GVIS_SELECTED))//设置多个单元格选中的颜色，和文字颜色
    {
        painter->setCompositionMode(QPainter::CompositionMode_Difference);
        painter->fillRect(rect, QColor(QCOLOR_HIGHLIGHT));//也可以用brush
        painter->setPen(QPen(QColor(QCOLOR_HIGHLIGHTTEXT)));//设置画笔的颜色
    }
    else
    {
        if (bEraseBkgnd)
        {
            rect.adjust(0,0,-2,-2);    // FillRect doesn't draw RHS or bottom
            QBrush brush(TextBkClr);
            painter->fillRect(rect,brush);//也可以用brush
            rect.adjust(0,0,2,2);
        }
        painter->setPen(QPen(TextClr));//设置画笔的颜色
    }

    // Draw lines only when wanted
    if (isFixed() && pGrid->gridLines() != GVL_NONE)
    {
        HCellID FocusCell = pGrid->focusCell();
        bool bHiliteFixed = pGrid->isTrackFocusCell() && pGrid->isValid(FocusCell) &&
                            (FocusCell.row == nRow || FocusCell.col == nCol);
        //如果当前选择的是合并单元格，同时该单元格处在此合并单元格内
        if(pGrid->getCell(FocusCell))
        {
            HCellRange mergeRange = pGrid->getCell(FocusCell)->mergeRange();
            if(mergeRange.isValid())
            {
                if((nRow >= mergeRange.minRow() && nRow <= mergeRange.maxRow()) ||
                    (nCol >= mergeRange.minCol()&& nCol <= mergeRange.maxCol()))
                    bHiliteFixed = true;

            }
        }

        painter->save();
        if (bHiliteFixed)
        {
            //rect.right++; rect.bottom++;
            //rect.adjust(0,0,-2,-2);
            QBrush brush((QColor("#FDB759")));
            painter->fillRect(rect,brush);
            //painter->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
            //rect.DeflateRect(1,1);
        }
        else
        {
            QPen lightpen(QColor(QCOLOR_3DHIGHLIGHT),1,Qt::SolidLine);
            QPen darkpen(QColor(QCOLOR_3DDKSHADOW),1,Qt::SolidLine);

            painter->setPen(lightpen);
            QPainterPath fixPath;
            fixPath.moveTo(rect.right(),rect.top());
            fixPath.lineTo(rect.left(), rect.top());
            fixPath.lineTo(rect.left(), rect.bottom());
            painter->drawPath(fixPath);

            painter->setPen(darkpen);
            QPainterPath fix1Path;
            fix1Path.moveTo(rect.right(),rect.top());
            fix1Path.lineTo(rect.right(), rect.bottom());
            fix1Path.lineTo(rect.left(), rect.bottom());
            painter->drawPath(fix1Path);
            rect.adjust(-1,-1,-1,-1);
        }
        painter->restore();
    }

    //绘制边框
    if(!isFixed() && pGrid->gridLines() != GVL_NONE)
    {
        painter->save();
        if(isDrawBorderLeft())
        {
            QPen leftPen(borderLeftColor());
            leftPen.setStyle(Qt::PenStyle(borderLeftStyle()));
            painter->setPen(leftPen);
            painter->drawLine(QPoint(rectBoard.left(),rectBoard.top()),QPoint(rectBoard.left(),rectBoard.bottom()));
        }

        if(isDrawBorderRight())
        {
            QPen rightPen(borderRightColor());
            rightPen.setStyle(Qt::PenStyle(borderRightStyle()));
            painter->setPen(rightPen);
            painter->drawLine(QPoint(rectBoard.right(),rectBoard.top()),QPoint(rectBoard.right(),rectBoard.bottom()));
        }

        if(isDrawBorderTop())
        {
            QPen topPen(borderTopColor());
            topPen.setStyle(Qt::PenStyle(borderTopStyle()));
            painter->setPen(topPen);
            painter->drawLine(QPoint(rectBoard.left(),rectBoard.top()),QPoint(rectBoard.right(),rectBoard.top()));
        }

        if(isDrawBorderBottom())
        {
            QPen bottomPen(borderBottomColor());
            bottomPen.setStyle(Qt::PenStyle(borderBottomStyle()));
            painter->setPen(bottomPen);
            painter->drawLine(QPoint(rectBoard.left(),rectBoard.bottom()),QPoint(rectBoard.right(),rectBoard.bottom()));
        }
        painter->restore();
    }

    rect.adjust(margin(),margin(),0,0);

    textRect(rect);
    painter->setFont(font());
    painter->drawText(rect,format()|QDT_NOPREFIX,text());
    painter->restore();
    return true;
}

//对rect进行判断如果是图片就要重新计算rect,如果不是照片就正常返回
bool HGridCellBase::textRect( QRect& rect)
{
    if (image() >= 0)
    {
       HGridCtrl* pGrid = grid();
       QImageList* pImageList = pGrid->imageList();
       if(pImageList)
       {
           QImage *image1 = pImageList->value(image());
           if(!image1->isNull())
           {
               int nImageWidth = image1->width()+1;
               rect.setLeft(rect.left() + nImageWidth + margin());
           }
       }
    }
    return true;
}

//获取文字的长宽
QSize HGridCellBase::textExtent(const QRect& rect,const QString& strText)
{
    //初始化单元格的时候
    HGridCtrl* pGrid = grid();
    Q_ASSERT(pGrid);
    QString szText = strText;
    if (szText.isEmpty())
    {
        HGridDefaultCell* pDefCell = (HGridDefaultCell*) defaultCell();
        Q_ASSERT(pDefCell);
        return QSize(pDefCell->width(), pDefCell->height());
    }

    int nFormat = format();
    QFontMetrics fontMetrics(font());
    QRect textRect = fontMetrics.boundingRect(rect,nFormat,strText);
    QSize size = QSize(textRect.width(),textRect.height());
    size += QSize(1, 1);
    return size;
}

//单元格有文字和图片的size
QSize HGridCellBase::cellExtent(const QRect& rect)
{
    QSize size = textExtent(rect,text());
    QSize imageSize(0,0);
    if (image() >= 0)
    {

       HGridCtrl* pGrid = grid();
       Q_ASSERT(pGrid);
       QImageList* pImageList = pGrid->imageList();
       if(pImageList)
       {
           QImage *image1 = pImageList->value(image());
           if(!image1->isNull())
           {
               imageSize = image1->size();
           }
       }
    }

    return QSize(size+imageSize);
}

bool HGridCellBase::validateEdit(QString& str)
{
    Q_UNUSED(str);
    return true;
}

//表格绘制都用行列方式
bool HGridCellBase::printCell(QPainter* pDC, int nRow, int nCol, QRect& rect)
{
    //Used for merge cells
/*
    if(	!isShow() && !isMerged())
    {
        return true;
    }

    QColor crFG, crBG;
    GV_ITEM Item;

    HGridCtrl* pGrid = grid();
    if (!pGrid || !pDC)
        return false;

    if( rect.width() <= 0 || rect.height() <= 0)
        return false;

    pDC->save();
    //rect.InflateRect(1,1);
    //pDC->Rectangle(rect);
    //rect.DeflateRect(1,1);


    crBG = QColor(QCLR_DEFAULT);
    crFG = QColor(0, 0, 0);
    //带阴影绘制表格
    if (pGrid->GetShadedPrintOut())
    {
        // Get the default cell implementation for this kind of cell. We use it if this cell
        // has anything marked as "default"
        HGridDefaultCell *pDefaultCell = (HGridDefaultCell*) defaultCell();
        if (!pDefaultCell)
            return false;

        // Use custom color if it doesn't match the default color and the
        // default grid background color.  If not, leave it alone.
        if(isFixed())
            crBG = (backClr() != QColor(QCLR_DEFAULT)) ? backClr() : pDefaultCell->backClr();
        else
            crBG = (backClr() != QColor(QCLR_DEFAULT) && backClr() != pDefaultCell->backClr()) ? backClr() : QColor(CLR_DEFAULT);

        // Use custom color if the background is different or if it doesn't
        // match the default color and the default grid text color.
        if(isFixed())
            crFG = (backClr() != QColor(QCLR_DEFAULT)) ? textClr() : pDefaultCell->textClr();
        else
            crFG = (backClr() != QColor(QCLR_DEFAULT)) ? textClr() : pDefaultCell->textClr();

        // If not printing on a color printer, adjust the foreground color
        // to a gray scale if the background color isn't used so that all
        // colors will be visible.  If not, some colors turn to solid black
        // or white when printed and may not show up.  This may be caused by
        // coarse dithering by the printer driver too (see image note below).
        //if(pDC->GetDeviceCaps(NUMCOLORS) == 2 && crBG == CLR_DEFAULT)
        //    crFG = RGB(GetRValue(crFG) * 0.30, GetGValue(crFG) * 0.59,
       //         GetBValue(crFG) * 0.11);

        // Only erase the background if the color is not the default
        // grid background color.
        if(crBG != QColor(QCLR_DEFAULT))
        {
            //QBrush brush(crBG);
            //rect.right++; rect.bottom++;
            //pDC->FillRect(rect, &brush);
            //rect.right--; rect.bottom--;
        }
    }
    else
    {
        crBG = QColor(QCLR_DEFAULT);
        crFG = QColor(0, 0, 0);
    }

    //设置文字颜色
    pDC->setPen(crFG);

    //设置字体
    pDC->setFont(font());

    // Draw lines only when wanted on fixed cells.  Normal cell grid lines
    // are handled in OnPrint.
    if(pGrid->gridLines() != GVL_NONE && isFixed())
    {
        QPen lightpen(QColor(QCOLOR_3DHIGHLIGHT),1,Qt::SolidLine);
        QPen darkpen(QColor(QCOLOR_3DDKSHADOW),1,Qt::SolidLine);

        painter->setPen(lightpen);
        QPainterPath fixPath;
        fixPath.moveTo(rect.right(),rect.top());
        fixPath.lineTo(rect.left(), rect.top());
        fixPath.lineTo(rect.left(), rect.bottom());
        painter->drawPath(fixPath);

        painter->setPen(darkpen);
        QPainterPath fix1Path;
        fix1Path.moveTo(rect.right(),rect.top());
        fix1Path.lineTo(rect.right(), rect.bottom());
        fix1Path.lineTo(rect.left(), rect.bottom());
        painter->drawPath(fix1Path);

        rect.DeflateRect(1,1);
    }

    rect.DeflateRect(GetMargin(), 0);

    if(pGrid->GetImageList() && GetImage() >= 0)
    {
        // NOTE: If your printed images look like fuzzy garbage, check the
        //       settings on your printer driver.  If it's using coarse
        //       dithering and/or vector graphics, they may print wrong.
        //       Changing to fine dithering and raster graphics makes them
        //       print properly.  My HP 4L had that problem.

        IMAGEINFO Info;
        if(pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw without clipping so as not to lose text when printed for real
    // DT_NOCLIP removed 01.01.01. Slower, but who cares - we are printing!

    pDC->drawText(rect,format() | QDT_NOPREFIX,text());

    pDC->restore();*/

    return true;
}

void HGridCellBase::OnEndEdit()
{

}

void HGridCellBase::onMouseEnter()
{

}

void HGridCellBase::onMouseOver()
{

}

void HGridCellBase::onMouseLeave()
{

}

void HGridCellBase::onClick( QPoint PointCellRelative)
{

}

void HGridCellBase::onClickDown( QPoint PointCellRelative)
{

}

void HGridCellBase::onRClick( QPoint PointCellRelative)
{

}

void HGridCellBase::onDblClick( QPoint PointCellRelative)
{

}

bool HGridCellBase::onSetCursor()
{
    return false;
}

//如果是合并单元格 此单元格不必再绘制
void HGridCellBase::setShow(bool b)
{
    m_bShow = b;
}

bool HGridCellBase::isShow()
{
    return m_bShow;
}

bool HGridCellBase::isMerged()
{
    return m_MergeRange.count() > 1;
}

void HGridCellBase::setMergeRange(HCellRange range)
{
    m_MergeRange = range;
}

HCellRange HGridCellBase::mergeRange()
{
    return m_MergeRange;
}

bool HGridCellBase::isMergeWithOthers()
{
    return m_bMergeWithOthers;
}

HCellID HGridCellBase::mergeCellID()
{
    return m_MergeCellID;
}

void HGridCellBase::setMergeCellID(HCellID cell)
{
    m_MergeCellID = cell;
    if(cell.row != -1)
        m_bMergeWithOthers = true;
    else
        m_bMergeWithOthers = false;
}

void HGridCellBase::split()
{
    m_bShow = true;
    m_MergeCellID.row = -1;
    m_MergeCellID.col = -1;
    m_bMergeWithOthers = false;
    m_MergeRange.set();
}
