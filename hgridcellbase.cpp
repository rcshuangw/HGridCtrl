 #include "hgridcellbase.h"
#include "hgridctrl.h"
 HGridCellBase::HGridCellBase(QObject* parent)
 {
     reset();
 }

HGridCellBase::~HGridCellBase()
{

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

    /*
    SetMergeCellID(cell.GetMergeCellID());
    SetMergeRange(cell.GetMergeRange());
    Show(cell.IsShow());
    */
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
}

/*
bool HGridCellBase::draw(QPainter* painter, int nRow, int nCol, QRect rect, bool bEraseBkgnd)
{
    // Note - all through this function we totally brutalise 'rect'. Do not
        // depend on it's value being that which was passed in.

    //Used for merge cells
   // if(	m_Hide && !IsMerged())
   // {
  //      return TRUE;
  //  }

    HGridCtrl* pGrid = grid();
    Q_ASSERT(pGrid);

    if (!pGrid || !painter)
        return false;

    if( rect.width() <= 0 || rect.height() <= 0)  // prevents imagelist item from drawing even
        return false;                             //  though cell is hidden

    //单元格的边框可以在这里绘制，如果设置了边框颜色和线条格式，就按照设置项绘制，注意：如果绘制bottom要获取同列的下一行表格，top要设置和bottom一样的风格。
    painter->save();
    painter->setBackgroundMode(Qt::TransparentMode);

    // Get the default cell implementation for this kind of cell. We use it if this cell
    // has anything marked as "default"
    HGridDefaultCell *pDefaultCell = (HGridDefaultCell*) defaultCell();
    if (!pDefaultCell)
        return false;

    // Set up text and background colours
    QColor TextClr, TextBkClr;

    TextClr = (textClr() == QColor(QCLR_DEFAULT))? pDefaultCell->textClr() : textClr();
    if (backClr() == QColor(QCLR_DEFAULT))
        TextBkClr = pDefaultCell->backClr();
    else
    {
        bEraseBkgnd = true;
        TextBkClr = backClr();
    }

    // Draw cell background and highlighting (if necessary)
    //焦点单元格：1.最外边的边框2.填充矩形。3填充和外边框中间一个矩形分割
    if ( isFocused() || isDropHighlighted() )
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if(state() & GVIS_SELECTED)
        {
            TextBkClr = QColor(QCOLOR_HIGHLIGHT);
            TextClr = QColor(QCOLOR_HIGHLIGHTTEXT);
            bEraseBkgnd = true;
        }

        //rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
         rect = rect.adjusted(1,1,-2,-2);
        if (bEraseBkgnd)
        {
            QBrush brush(TextBkClr);
            painter->fillRect(rect, brush);

        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->gridLines() != GVL_NONE)
        {
            rect.adjust(-1,-1,2,2);
        }

        painter->setPen(QPen(TextClr));

        // Adjust rect after frame draw if no grid lines
        if(pGrid->gridLines() == GVL_NONE)
        {
            rect.adjust(0,-1,0,-1);
        }
        //rect = rect.marginsAdded(QMargins(0,1,1,1));
    }
    else if ((state() & GVIS_SELECTED))//设置多个单元格选中的颜色，和文字颜色
    {
        rect.adjust(-1,-1,0,0);    // FillRect doesn't draw RHS or bottom
        //painter->setOpacity(0.5);
        painter->fillRect(rect, QColor(QCOLOR_HIGHLIGHT));//也可以用brush

        rect.adjust(1,1,0,0);
        painter->setPen(QPen(QColor(QCOLOR_HIGHLIGHTTEXT)));//设置画笔的颜色
    }
    else
    {
        if (bEraseBkgnd)
        {
            //rect.adjust(0,1,0,1);    // FillRect doesn't draw RHS or bottom
            QBrush brush(TextBkClr);
            painter->fillRect(rect,brush);//也可以用brush
            //rect.adjust(0,-1,0,-1);
        }
        painter->setPen(QPen(TextClr));//设置画笔的颜色
    }

    // Draw lines only when wanted
    if (isFixed() && pGrid->gridLines() != GVL_NONE)
    {
        HCellID FocusCell = pGrid->focusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        bool bHiliteFixed = false;//pGrid->trackFocusCell() && pGrid->isValid(FocusCell) &&
                            //(FocusCell.row == nRow || FocusCell.col == nCol);

        // If this fixed cell is on the same row/col as the focus cell,
        // highlight it.
        //不要重绘选中的固定表头

        //绘制表头的颜色
        //{
            painter->save();
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
            painter->restore();
            rect.adjust(-1,-1,-1,-1);

        //}
    }

    // Draw Text and image


    rect.adjust(margin(),0,margin(),0);

    //绘制图像
    if (pGrid->imageList() && image() >= 0)
    {
        QImage* mImage;
        mImage = pGrid->imageList()->value(image());
        if(!mImage->isNull())
        {
            int nImageWidth = mImage->width() + 1;
            int nImageHeight = mImage->height() + 1;

            if( nImageWidth + rect.left() <= rect.right() + (int)(2*margin())
                && nImageHeight + rect.top() <= rect.bottom() + (int)(2*margin())  )
            {
                painter->drawImage(rect,*mImage);
            }
        }
    }

    // 排序绘制
    if (pGrid->sortColumn() == nCol && nRow == 0)
    {
        QSize size = textExtent(("M"),painter);
        int nOffset = 2;
        //绘制的前提条件就是将矩形稍微调小，防止绘制到边框外部
        size.setHeight(size.height() - nOffset * 2);

        if (size.height() >= rect.height())
            size.setHeight(rect.height() - (nOffset * 2));

        size.setWidth(size.height());      // Make the dimensions square

        // Kludge for vertical text
        //BOOL bVertical = (GetFont()->lfEscapement == 900);
        bool bVertical = false; //不考虑垂直文字
        // Only draw if it'll fit!
        if (size.width() + rect.left() < rect.right() + (int)(2*margin()))
        {
            int nTriangleBase = rect.bottom() - nOffset - size.height();    // Triangle bottom right
            int nTriangleLeft;
            if (bVertical)
                nTriangleLeft = (rect.right() + rect.left() - size.width())/2; // Triangle middle
            else
                nTriangleLeft = rect.right() - size.width();               // Triangle RHS

            QPen penShadow(QColor(QCOLOR_3DSHADOW),1,Qt::SolidLine);
            QPen penLight(QColor(QCOLOR_3DHILIGHT),1,Qt::SolidLine);
            if (pGrid->isSortAscending())
            {
                // Draw triangle pointing upwards
                painter->save();
                painter->setPen(penLight);
                QPainterPath sortPath;
                sortPath.moveTo( nTriangleLeft + 1, nTriangleBase + size.height() + 1);
                sortPath.lineTo( nTriangleLeft + (size.width() / 2) + 1, nTriangleBase + 1 );
                sortPath.lineTo( nTriangleLeft + size.width() + 1, nTriangleBase + size.height() + 1);
                sortPath.lineTo( nTriangleLeft + 1, nTriangleBase + size.height() + 1);

                painter->setPen(penShadow);
                sortPath.moveTo( nTriangleLeft, nTriangleBase + size.height() );
                sortPath.lineTo( nTriangleLeft + (size.width() / 2), nTriangleBase );
                sortPath.lineTo( nTriangleLeft + size.width(), nTriangleBase + size.height() );
                sortPath.lineTo( nTriangleLeft, nTriangleBase + size.height() );
                painter->restore();
            }
            else
            {
                // Draw triangle pointing downwards
                painter->save();
                painter->setPen(penLight);
                QPainterPath sortPath;
                sortPath.moveTo( nTriangleLeft + 1, nTriangleBase + 1 );
                sortPath.lineTo( nTriangleLeft + (size.width() / 2) + 1, nTriangleBase + size.height() + 1 );
                sortPath.lineTo( nTriangleLeft + size.width() + 1, nTriangleBase + 1 );
                sortPath.lineTo( nTriangleLeft + 1, nTriangleBase + 1 );

                painter->setPen(penShadow);
                sortPath.moveTo( nTriangleLeft, nTriangleBase );
                sortPath.lineTo( nTriangleLeft + (size.width() / 2), nTriangleBase + size.height() );
                sortPath.lineTo( nTriangleLeft + size.width(), nTriangleBase );
                sortPath.lineTo( nTriangleLeft, nTriangleBase );
                painter->restore();
            }

            if (!bVertical)
                rect.setRight(rect.right() - size.height());
        }
    }

    // We want to see '&' characters so use DT_NOPREFIX
    textRect(rect);
    painter->drawText(rect,QDT_LEFT|QDT_VCENTER|QDT_SINGLELINE,text());
    painter->restore();
    return true;
}*/

bool HGridCellBase::draw(QPainter* painter, int nRow, int nCol, QRect rect, bool bEraseBkgnd)
{
    // Note - all through this function we totally brutalise 'rect'. Do not
        // depend on it's value being that which was passed in.

    //Used for merge cells
    /*if(	m_Hide && !IsMerged())
    {
        return TRUE;
    }*/

    HGridCtrl* pGrid = grid();
    Q_ASSERT(pGrid);

    if (!pGrid || !painter)
        return false;

    if( rect.width() <= 0 || rect.height() <= 0)  // prevents imagelist item from drawing even
        return false;                             //  though cell is hidden

    //单元格的边框可以在这里绘制，如果设置了边框颜色和线条格式，就按照设置项绘制，注意：如果绘制bottom要获取同列的下一行表格，top要设置和bottom一样的风格。
    painter->save();
    painter->setBackgroundMode(Qt::TransparentMode);

    // Get the default cell implementation for this kind of cell. We use it if this cell
    // has anything marked as "default"
    HGridDefaultCell *pDefaultCell = (HGridDefaultCell*) defaultCell();
    if (!pDefaultCell)
        return false;

    // Set up text and background colours
    QColor TextClr, TextBkClr;

    TextClr = (textClr() == QColor(QCLR_DEFAULT))? pDefaultCell->textClr() : textClr();
    if (backClr() == QColor(QCLR_DEFAULT))
        TextBkClr = pDefaultCell->backClr();
    else
    {
        bEraseBkgnd = true;
        TextBkClr = backClr();
    }

    if ( isFocused() || isDropHighlighted() )
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if(state() & GVIS_SELECTED)
        {
            TextBkClr = QColor(QCOLOR_HIGHLIGHT);
            TextClr = QColor(QCOLOR_HIGHLIGHTTEXT);
            bEraseBkgnd = true;
        }

        if (bEraseBkgnd)
        {
            //QBrush brush(TextBkClr);
            //painter->fillRect(rect, brush);

        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->gridLines() != GVL_NONE)
        {
            rect.adjust(0,0,-2,-2);
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
            rect.adjust(0,0,1,1);    // FillRect doesn't draw RHS or bottom
            QBrush brush(TextBkClr);
            painter->fillRect(rect,brush);//也可以用brush
            rect.adjust(0,0,-1,-1);
        }
        painter->setPen(QPen(TextClr));//设置画笔的颜色
    }

    // Draw lines only when wanted
    if (isFixed() && pGrid->gridLines() != GVL_NONE)
    {
        HCellID FocusCell = pGrid->focusCell();
        bool bHiliteFixed = pGrid->isTrackFocusCell() && pGrid->isValid(FocusCell) &&
                            (FocusCell.row == nRow || FocusCell.col == nCol);
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

    rect.adjust(margin(),margin(),0,0);

    // We want to see '&' characters so use DT_NOPREFIX
    textRect(rect);
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

QSize HGridCellBase::textExtent(const QString& szText, QPainter* painter)
{
    //初始化单元格的时候
    HGridCtrl* pGrid = grid();
    Q_ASSERT(pGrid);

    if (painter == NULL || szText.isEmpty())
    {
        HGridDefaultCell* pDefCell = (HGridDefaultCell*) defaultCell();
        Q_ASSERT(pDefCell);
        return QSize(pDefCell->width(), pDefCell->height());
    }

    QSize size;
    int nFormat = format();

    painter->setFont(font());
    QFontMetrics fontMetrics(font());
    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    if ((nFormat & QDT_WORDBREAK) && !(nFormat & QDT_SINGLELINE))
    {
        QString str = szText;
        int nMaxWidth = 0;
        //需要这样做吗？--huangw
        while (true)
        {
            int nPos = str.indexOf(('\n'));
            QString TempStr = (nPos < 0)? str : str.left(nPos);
            int nTempWidth = fontMetrics.width(TempStr);
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;

            if (nPos < 0)
                break;
            str = str.mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }

        QRect rect;
        rect.setRect(0,0, nMaxWidth+1, 0);
        QRect boundingRect;
        painter->drawText(rect,nFormat,szText,&boundingRect);
        size = boundingRect.size();
    }

    size = fontMetrics.size(nFormat,szText);
    size += QSize(4*margin(), 2*margin());

    //Qt暂不支持垂直字体
    // Kludge for vertical text
    /*LOGFONT *pLF = GetFont();
    if (pLF->lfEscapement == 900 || pLF->lfEscapement == -900)
    {
        int nTemp = size.cx;
        size.cx = size.cy;
        size.cy = nTemp;
        size += CSize(0, 4*GetMargin());
    }

    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);*/
    return size;
}

//单元格有文字和图片的size
QSize HGridCellBase::cellExtent(QPainter* painter)
{
    QSize size = textExtent(text(),painter);
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

    // EFW - Added to print cells properly
bool HGridCellBase::printCell(QPainter* painter, int nRow, int nCol, QRect& rect)
{
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

