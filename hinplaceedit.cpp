// InPlaceEdit.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
#include "hinplaceedit.h"
#include "hgridctrl.h"
#include "hgridcell.h"
/////////////////////////////////////////////////////////////////////////////
// HInPlaceEdit

HInPlaceEdit::HInPlaceEdit(QWidget* pParent, const QRect& rect, quint32 dwStyle,
                           int nRow, int nColumn, QString sInitText)
    :QLineEdit(pParent)
{
    m_bSend = false;
    m_sInitText     = sInitText;
    m_nRow          = nRow;
    m_nColumn       = nColumn;

    m_Rect = rect;  // For bizarre CE bug.
    
    HGridCellBase* pCell = ((HGridCtrl*)pParent)->getCell(nRow,nColumn);
    if(pCell)
    {
        setFont(pCell->font());
        setAlignment(Qt::Alignment(pCell->format()));

        QBrush myBrush;
        QPalette palette;
        myBrush = QBrush(pCell->textClr(),Qt::DiagCrossPattern);
        palette.setBrush( QPalette::Text,  myBrush);
        setPalette(palette);
    }
    else
    {
        setFont(((HGridCtrl*)pParent)->font());
        setAlignment(QDT_LEFT | QDT_CENTER);
    }
    setText(sInitText);
    setFocus();
    //setContextMenuPolicy(Qt::NoContextMenu);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(this,&HInPlaceEdit::endEditCell,(HGridCtrl*)pParent,&HGridCtrl::onEndEditCell);
    connect(this, &HInPlaceEdit::editingFinished, this,&HInPlaceEdit::onEditingFinished);
    installEventFilter(this);

}

void HInPlaceEdit::keyReleaseEvent(QKeyEvent *event)
{
    QString strTest = "";
}

HInPlaceEdit::~HInPlaceEdit()
{
}

void HInPlaceEdit::onEditingFinished()
{
    endEdit();
}

void HInPlaceEdit::onTextChanged(const QString &text)
{

}

////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit implementation

void HInPlaceEdit::endEdit()
{
    QString str;
    static bool bAlreadyEnding = false;
    if(bAlreadyEnding)
        return;
    bAlreadyEnding = true;
    str = text();
    emit endEditCell(m_nRow, m_nColumn, str);
    bAlreadyEnding = false;
    close();
}
