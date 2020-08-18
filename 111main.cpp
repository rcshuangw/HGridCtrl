#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

#include <QApplication>
#include "hgridctrl.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HGridCtrl m_pGridCtrl;// = new HGridCtrl;
    m_pGridCtrl.resize(600,600);
    m_pGridCtrl.setRowCount(5);
    m_pGridCtrl.setColumnCount(10);
    m_pGridCtrl.setFixedRowCount(1);
    m_pGridCtrl.setFixedColumnCount(1);
    m_pGridCtrl.show();

    return a.exec();
}
