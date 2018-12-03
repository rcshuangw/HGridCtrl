#include "hgridctrlwidget.h"
#include "hgridctrl.h"
#include "hgridctrlhelper.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
HGridCtrlWidget::HGridCtrlWidget(QWidget* parent)
{
    m_pGridCtrl = new HGridCtrl;
}

void HGridCtrlWidget::initReportWidget(quint8 type)
{
    //
    m_uGridReportType = type;
    QVBoxLayout* vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    if(m_uGridReportType == GRIDREPORT_TYPE_EDITOR)
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
        m_inputLineEdit->setObjectName(QStringLiteral("lineEdit"));

        hBoxLayout->addWidget(m_inputLineEdit);

        hBoxLayout->setStretch(0, 1);
        hBoxLayout->setStretch(1, 9);
        vBoxLayout->addLayout(hBoxLayout);
        vBoxLayout->addWidget(m_pGridCtrl);
        vBoxLayout->setStretch(0,1);
        vBoxLayout->setStretch(1,9);
    }
    else if(m_uGridReportType == GRIDREPORT_TYPE_BROWSER)
    {
        vBoxLayout->addWidget(m_pGridCtrl);
    }
    setLayout(vBoxLayout);

}

void HGridCtrlWidget::setGridCtrlItem(HGridCtrlInfo* pItem)
{
    if(NULL == pItem) return;
    m_pGridCtrlInfo = pItem;//是通过指针赋值还是直接赋值  --huangw
    m_pGridCtrl->setRowCount(m_pGridCtrlInfo->m_GridCtrlItem.nMaxRow);
    m_pGridCtrl->setColumnCount(m_pGridCtrlInfo->m_GridCtrlItem.nMaxCol);
    m_pGridCtrl->setFixedColumnCount(0);
    m_pGridCtrl->setFixedRowCount(0);
    int row ,col = 0;
    if(m_uGridReportType == GRIDREPORT_TYPE_EDITOR)
    {
        //设置固定行列
        m_pGridCtrl->setFixedColumnCount(1);
        m_pGridCtrl->setFixedRowCount(1);
        for(int i = 0; i < m_pGridCtrlInfo->m_pGridCellItemList.count();i++)
        {
            HGridCellInfo* pInfo = (HGridCellInfo*)m_pGridCtrlInfo->m_pGridCellItemList[i];
            if(pInfo) continue;
            //GV_ITEM Item = pInfo->m_GridCellItem;
            pInfo->m_GridCellItem.mask = GVIF_TEXT|GVIF_FORMAT;
            row = pInfo->m_GridCellItem.row;
            col = pInfo->m_GridCellItem.col;
            if (row < 1)
            {
                pInfo->m_GridCellItem.nFormat = QDT_LEFT|QDT_WORDBREAK;
                pInfo->m_GridCellItem.strText = QString("%d").arg(row);
            }
            else if (col < 1)
            {
                pInfo->m_GridCellItem.nFormat = QDT_RIGHT|QDT_VCENTER|QDT_SINGLELINE|QDT_NOPREFIX;
                pInfo->m_GridCellItem.strText = QString("%d").arg(col);
            }
            else
            {
                pInfo->m_GridCellItem.nFormat = QDT_CENTER|QDT_VCENTER|QDT_SINGLELINE|QDT_NOPREFIX;
            }
            m_pGridCtrl->setItem(&pInfo->m_GridCellItem);
        }
    }
    else if(m_uGridReportType == GRIDREPORT_TYPE_BROWSER)
    {
        for(int i = 0; i < m_pGridCtrlInfo->m_pGridCellItemList.count();i++)
        {
            HGridCellInfo* pInfo = (HGridCellInfo*)m_pGridCtrlInfo->m_pGridCellItemList[i];
            if(pInfo) continue;
            pInfo->m_GridCellItem.mask = GVIF_TEXT|GVIF_FORMAT;
            row = pInfo->m_GridCellItem.row;
            col = pInfo->m_GridCellItem.col;
            pInfo->m_GridCellItem.nFormat = QDT_CENTER|QDT_VCENTER|QDT_SINGLELINE|QDT_NOPREFIX;
            m_pGridCtrl->setItem(&pInfo->m_GridCellItem);
        }
    }

}

void HGridCtrlWidget::getGridCtrlItem(HGridCtrlInfo* pItem)
{
    pItem = m_pGridCtrlInfo;
    for(int i = 0; i < m_pGridCtrlInfo->m_pGridCellItemList.count();i++)
    {
        HGridCellInfo* pInfo = (HGridCellInfo*)m_pGridCtrlInfo->m_pGridCellItemList[i];
        if(pInfo) continue;
        //pInfo->m_GridCellItem.row = row;
        //pInfo->m_GridCellItem.col = col;
        m_pGridCtrl->item(&pInfo->m_GridCellItem);
    }
}

void HGridCtrlWidget::gridCell_Clicked(HGridCell* pCell)
{
    emit gridcellclickded(pCell);
}



