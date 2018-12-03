#include "hgridctrlhelper.h"
#include "hgridcelldef.h"

HGridCtrlInfo::HGridCtrlInfo()
{

}

HGridCtrlInfo::~HGridCtrlInfo()
{

}

HGridCtrlInfo& HGridCtrlInfo::operator =(const HGridCtrlInfo& info)
{
    if(this == &info) return *this;
    m_GridCtrlItem.wReportID = info.m_GridCtrlItem.wReportID;
    m_GridCtrlItem.nMaxCol = info.m_GridCtrlItem.nMaxCol;
    m_GridCtrlItem.nMaxRow = info.m_GridCtrlItem.nMaxRow;
    m_GridCtrlItem.btType = info.m_GridCtrlItem.btType;
    m_GridCtrlItem.strReportName = info.m_GridCtrlItem.strReportName;
    return *this;
}

void HGridCtrlInfo::loadGridCtrlData(int v,QDataStream* ds)
{
    if(!ds) return;
    quint8 n8;
    *ds>>n8;
    m_GridCtrlItem.btType = n8;
    quint32 l;
    *ds>>l;
    m_GridCtrlItem.nMaxCol = l;
    *ds>>l;
    m_GridCtrlItem.nMaxRow = l;
    quint32 n32;
    *ds>>n32;
    m_GridCtrlItem.wReportID = n32;
    QString s;
    *ds>>s;
    m_GridCtrlItem.strReportName = s;
    int n;
    *ds>>n;
    for(int i = 0; i < n;i++)
    {
        HGridCellInfo* pInfo = new HGridCellInfo;
        pInfo->loadGridCellData(v,ds);
        m_pGridCellItemList.append(pInfo);
    }
}

void HGridCtrlInfo::saveGridCtrlData(int v,QDataStream* ds)
{
    if(!ds) return;
    *ds<<v;
    *ds<<(quint8)m_GridCtrlItem.btType;
    *ds<<(quint32)m_GridCtrlItem.nMaxCol;
    *ds<<(quint32)m_GridCtrlItem.nMaxRow;
    *ds<<(quint32)m_GridCtrlItem.wReportID;
    *ds<<(QString)m_GridCtrlItem.strReportName;
    *ds<<(int)m_pGridCellItemList.count();
    for(int i = 0; i < m_pGridCellItemList.count();i++)
    {
        HGridCellInfo* pInfo = (HGridCellInfo*)m_pGridCellItemList[i];
        pInfo->saveGridCellData(v,ds);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//表格信息
HGridCellInfo::HGridCellInfo()
{

}

HGridCellInfo::~HGridCellInfo()
{

}

//gvItem初始化必须要赋值
void HGridCellInfo::loadGridCellData(int v,QDataStream* ds)
{
    if(!ds) return;
    int n;
    *ds>>n; //version
    //version = n;
    *ds>>n;
    m_GridCellItem.row = n;
    *ds>>n;
    m_GridCellItem.col = n;
    uint un;
    *ds>>un;
    m_GridCellItem.mask = n;
    *ds>>un;
    m_GridCellItem.nState = n;
    quint32 n32;
    *ds>>n32;
    m_GridCellItem.nFormat = n32;
    *ds>>n;
    m_GridCellItem.nImage = n;
    QColor clr;
    *ds>>clr;
    m_GridCellItem.crBkClr = clr;
    *ds>>clr;
    m_GridCellItem.crFgClr = clr;
    quint32 l;
    *ds>>l;
    m_GridCellItem.lParam = l;
    QFont ft;
    *ds>>ft;
    m_GridCellItem.lfFont = ft;
    *ds>>un;
    m_GridCellItem.nMargin = un;
    QString s;
    *ds>>s;
    m_GridCellItem.strText = s;

}

void HGridCellInfo::saveGridCellData(int v,QDataStream* ds)
{
    if(!ds) return;
    *ds<<v;
    *ds<<(int)m_GridCellItem.row;
    *ds<<(int)m_GridCellItem.col;
    *ds<<(uint)m_GridCellItem.mask;
    *ds<<(uint)m_GridCellItem.nState;
    *ds<<(quint32)m_GridCellItem.nFormat;
    *ds<<(int)m_GridCellItem.nImage;
    *ds<<(QColor)m_GridCellItem.crBkClr;
    *ds<<(QColor)m_GridCellItem.crFgClr;
    *ds<<(quint32)m_GridCellItem.lParam;
    *ds<<(QFont)m_GridCellItem.lfFont;
    *ds<<(uint)m_GridCellItem.nMargin;
    *ds<<(QString)m_GridCellItem.strText;
}


//表格文件，不是报表文件

HGridCtrlFile::HGridCtrlFile()
{
    m_pCurGridCtrlInfo = NULL;
}

HGridCtrlFile::~HGridCtrlFile()
{

}



//从文件路径加载报表文件
bool HGridCtrlFile::loadGridCtrlFile()
{
    return true;
}

//存储报表文件
bool HGridCtrlFile::saveGridCtrlFile()
{
    return true;
}

HGridCtrlInfo* HGridCtrlFile::addGridCtrlInfo(GC_ITEM* pItem)
{
    if(NULL == pItem)
        return NULL;
    quint16 reportID = 0;
    while(findGridCtrlInfo(reportID))
        reportID++;
    HGridCtrlInfo* pInfo = new HGridCtrlInfo;
    pInfo->m_GridCtrlItem.wReportID = reportID;
    pInfo->m_GridCtrlItem.nMaxCol = pItem->nMaxCol;
    pInfo->m_GridCtrlItem.nMaxRow = pItem->nMaxRow;
    pInfo->m_GridCtrlItem.btType = pItem->btType;
    pInfo->m_GridCtrlItem.strReportName = pItem->strReportName;
    m_pGridCtrlInfoList.append(pInfo);
    m_pCurGridCtrlInfo = pInfo;
    return pInfo;
}

bool HGridCtrlFile::delGridCtrlInfo(int id)
{
    bool bok = findGridCtrlInfo(id);
    if(!bok) return true;
    HGridCtrlInfo* pInfo = getGridCtrlInfoById(id);
    if(pInfo)
    {
        m_pGridCtrlInfoList.removeOne(pInfo);
        delete pInfo;
    }
    return true;
}

bool HGridCtrlFile::findGridCtrlInfo(int id)
{
    if(id > m_pGridCtrlInfoList.count())
        return false;
    for(int i = 0;i < m_pGridCtrlInfoList.count();i++)
    {
        HGridCtrlInfo* pInfo = m_pGridCtrlInfoList[i];
        if(pInfo && pInfo->m_GridCtrlItem.wReportID == id)
            return true;
    }
    return false;
}

//通过模板ID来获取对应的表格控件信息
HGridCtrlInfo*  HGridCtrlFile::getGridCtrlInfoById(int id)
{
    if(id > m_pGridCtrlInfoList.count())
        return false;
    HGridCtrlInfo* pInfo = NULL;
    for(int i = 0;i < m_pGridCtrlInfoList.count();i++)
    {
        pInfo = m_pGridCtrlInfoList[i];
        if(pInfo && pInfo->m_GridCtrlItem.wReportID == id)
            break;
    }
    m_pCurGridCtrlInfo;
    return pInfo;
}

void HGridCtrlFile::setGridCtrlInfoById(int id, HGridCtrlInfo *pInfo)
{
    bool bok = findGridCtrlInfo(id);
    if(!bok) return;
    HGridCtrlInfo* pSrcInfo = getGridCtrlInfoById(id);
    *pSrcInfo = *pInfo;
}


