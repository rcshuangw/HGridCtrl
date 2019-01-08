#include "hcellrange.h"

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &s, const HCellRange &cellRange)
{
    s<<(int)cellRange.minRow();
    s<<(int)cellRange.maxRow();
    s<<(int)cellRange.minCol();
    s<<(int)cellRange.maxCol();
    return s;
}

QDataStream &operator>>(QDataStream &s, HCellRange &cellRange)
{
    int n;
    s>>n;
    cellRange.setMinRow(n);
    s>>n;
    cellRange.setMaxRow(n);
    s>>n;
    cellRange.setMinCol(n);
    s>>n;
    cellRange.setMaxCol(n);
    return s;
}
#endif

inline void HCellRange::set(int minRow, int minCol, int maxRow, int maxCol)
{
     m_nMinRow = minRow;
     m_nMinCol = minCol;
     m_nMaxRow = maxRow;
     m_nMaxCol = maxCol;
}

inline void HCellRange::operator=(const HCellRange& rhs)
{
    if (this != &rhs) set(rhs.m_nMinRow, rhs.m_nMinCol, rhs.m_nMaxRow, rhs.m_nMaxCol);
}

inline int HCellRange::operator==(const HCellRange& rhs)
{
     return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) &&
             (m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

inline int HCellRange::operator!=(const HCellRange& rhs)
{
     return !operator==(rhs);
}

inline int HCellRange::isValid() const
{
     return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 &&
             m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

inline int HCellRange::inRange(int row, int col) const
{
     return (row >= m_nMinRow && row <= m_nMaxRow && col >= m_nMinCol && col <= m_nMaxCol);
}

inline int HCellRange::inRange(const HCellID& cellID) const
{
     return inRange(cellID.row, cellID.col);
}

inline HCellID HCellRange::topLeft() const
{
     return HCellID(m_nMinRow, m_nMinCol);
}

inline HCellRange HCellRange::intersect(const HCellRange& rhs) const
{

     return HCellRange(qMax(m_nMinRow,rhs.m_nMinRow), qMax(m_nMinCol,rhs.m_nMinCol),
                       qMin(m_nMaxRow,rhs.m_nMaxRow), qMin(m_nMaxCol,rhs.m_nMaxCol));
}
