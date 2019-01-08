#include "hgridcell.h"
#include "hinplaceedit.h"
HGridCell::HGridCell(HGridCellBase *parent) : HGridCellBase(parent)
{
    reset();
}

HGridCell::~HGridCell()
{
    //delete m_plfFont;
}

void HGridCell::load(int v,QDataStream* ds)
{
    if(!ds) return;
    HGridCellBase::load(v,ds);
    QString str;
    *ds>>str;
    m_strText = str;
    quint32 n32;
    *ds>>n32;
    m_lParam = n32;
    int n;
    *ds>>n;
    m_nImage = n;
    *ds>>n32;
    m_nFormat = n32;
    *ds>>n32;
    m_nTextOption = n32;
    QColor cr;
    *ds>>cr;
    m_crFgClr = cr;
    *ds>>cr;
    m_crBkClr = cr;
    QFont font;
    *ds>>font;
    m_plfFont = font;
    uint ni;
    *ds>>ni;
    m_nMargin = ni;

    bool b;
    *ds>>b;
    m_bLeftBorder = b;
    *ds>>b;
    m_bTopBorder = b;
    *ds>>b;
    m_bRightBorder = b;
    *ds>>b;
    m_bBottomBorder = b;

    short sn;
    *ds>>sn;
    m_nLeftBorderStyle = sn;
    *ds>>sn;
    m_nTopBorderStyle = sn;
    *ds>>sn;
    m_nRightBorderStyle = sn;
    *ds>>sn;
    m_nBottomBorderStyle = sn;

    *ds>>cr;
    m_crLeftBoderClr = cr;
    *ds>>cr;
    m_crTopBoderClr = cr;
    *ds>>cr;
    m_crRightBoderClr = cr;
    *ds>>cr;
    m_crBottomBoderClr = cr;

}

void HGridCell::save(int v,QDataStream* ds)
{
    if(!ds) return;
    HGridCellBase::save(v,ds);
    *ds<<(QString)m_strText;
    *ds<<(quint32)m_lParam;
    *ds<<(int)m_nImage;
    *ds<<(quint32)m_nFormat;
    *ds<<(quint32)m_nTextOption;
    *ds<<(QColor)m_crFgClr;
    *ds<<(QColor)m_crBkClr;
    *ds<<(QFont)m_plfFont;
    *ds<<(uint)m_nMargin;

    *ds<<(bool)m_bLeftBorder;
    *ds<<(bool)m_bTopBorder;
    *ds<<(bool)m_bRightBorder;
    *ds<<(bool)m_bBottomBorder;

    *ds<<(short)m_nLeftBorderStyle;
    *ds<<(short)m_nTopBorderStyle;
    *ds<<(short)m_nRightBorderStyle;
    *ds<<(short)m_nBottomBorderStyle;

    *ds<<(QColor)m_crLeftBoderClr;
    *ds<<(QColor)m_crTopBoderClr;
    *ds<<(QColor)m_crRightBoderClr;
    *ds<<(QColor)m_crBottomBoderClr;
}


void HGridCell::operator=( HGridCell& cell)
{
    if (this != &cell) HGridCellBase::operator=(cell);
}

void HGridCell::reset()
{
    HGridCellBase::reset();

    m_strText  = "";
    m_nImage   = -1;
    m_pGrid    = NULL;
    m_bEditing = false;
    m_pEditWnd = NULL;

    m_nFormat = QDT_LEFT|QDT_VCENTER|QDT_SINGLELINE|QDT_NOPREFIX;  // Use default from CGridDefaultCell
    m_crBkClr = QColor(Qt::white);     // Background colour (or CLR_DEFAULT)
    m_crFgClr = QColor(Qt::black);     // Forground colour (or CLR_DEFAULT)
    m_nMargin = (uint)-1;              // Use default from CGridDefaultCell
    m_plfFont = QFont(QStringLiteral("宋体"),11, QFont::Normal);
    m_lParam  = quint32(0);
    m_Size    = QSize(100,25);            // Default Size
    //m_bBorderOutSide= false;
    m_bLeftBorder   = false;
    m_bTopBorder    = false;
    m_bRightBorder  = false;
    m_bBottomBorder = false;

    //m_nBorderStyle       = Qt::SolidLine;
    m_nLeftBorderStyle   = Qt::SolidLine;
    m_nTopBorderStyle    = Qt::SolidLine;
    m_nRightBorderStyle  = Qt::SolidLine;
    m_nBottomBorderStyle = Qt::SolidLine;

    //m_crBoderClr       = QColor(Qt::black);
    m_crLeftBoderClr   = QColor(Qt::black);
    m_crTopBoderClr    = QColor(Qt::black);
    m_crRightBoderClr  = QColor(Qt::black);
    m_crBottomBoderClr = QColor(Qt::black);
}

/////////////////////////////////////////////////////////////////////////////
quint32 HGridCell::format()  const
{
    if (m_nFormat == (quint32)-1)
    {
        HGridDefaultCell *pDefaultCell = (HGridDefaultCell*) defaultCell();
        if (!pDefaultCell)
            return 0;
        return pDefaultCell->format();
   }
    return m_nFormat;
}

bool HGridCell::edit(int nRow, int nCol, const QRect& rect, const QPoint& point)
{
    if ( m_bEditing )
    {
    }
    else
    {
        m_bEditing = true;
        quint32 dwStyle = format();
        HGridCtrl* pGrid = grid();
        m_pEditWnd = new HInPlaceEdit((QWidget*)pGrid, rect, dwStyle, nRow, nCol, text());
        m_pEditWnd->setFocusPolicy(Qt::StrongFocus);
        m_pEditWnd->resize(rect.width(),rect.height());
        m_pEditWnd->move(rect.left(),rect.top());
        m_pEditWnd->show();
    }
    return true;
}

void HGridCell::endEdit()
{
    if (m_pEditWnd)
    {
        ((HInPlaceEdit*)m_pEditWnd)->endEdit();
    }
}

void HGridCell::OnEndEdit()
{
    m_bEditing = false;
    m_pEditWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// HGridDefaultCell
HGridDefaultCell::HGridDefaultCell()
{

    m_nFormat = QDT_LEFT|QDT_VCENTER|QDT_SINGLELINE|QDT_NOPREFIX;
    m_crFgClr = QColor(Qt::black);
    m_crBkClr = QColor(Qt::white);
    m_Size    = QSize(100,25);
    m_dwStyle = 0;
    m_Font = QFont("宋体",11,QFont::Normal);
}

HGridDefaultCell::~HGridDefaultCell()
{
}

void HGridDefaultCell::setFont(const QFont& plf)
{
    HGridCell::setFont(plf);
    // Get the font size and hence the default cell size
    QFontMetrics fontMetrics(plf);
    setMargin(fontMetrics.size(Qt::TextSingleLine,tr(" ")).width());
    m_Size = fontMetrics.size(Qt::TextSingleLine,tr(" XXXXXXXXXXXX "));
    m_Size.setHeight(m_Size.height() * 3/2);
}
