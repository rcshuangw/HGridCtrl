//////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//单元格编辑框
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPLACEEDIT_H__ECD42821_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_INPLACEEDIT_H__ECD42821_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <QLineEdit>
#include <QAbstractScrollArea>
class HGridCtrl;
class HInPlaceEdit : public QLineEdit
{
    Q_OBJECT
public:
    HInPlaceEdit(QWidget* pParent, const QRect& rect, quint32 dwStyle,
                 int nRow, int nColumn, QString sInitText);
    virtual ~HInPlaceEdit();

public:
    void endEdit();
public slots:
    void onEditingFinished();
    void onTextChanged(const QString &text);
signals:
    void endEditCell(int nRow, int nCol, QString str);
private:
    bool    m_bSend;
    int     m_nRow;
    int     m_nColumn;
    QString m_sInitText;
    uint    m_nLastChar;
    bool    m_bExitOnArrows;
    QRect   m_Rect;
};
 
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEEDIT_H__ECD42821_16DF_11D1_992F_895E185F9C72__INCLUDED_)
