#-------------------------------------------------
#
# Project created by /huangw 2018-10-23T10:06:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gridctrl
TEMPLATE = lib
DEFINES += GRIDCTRL_DLL
DESTDIR = ../../lib
INCLUDEPATH += \
                ../include \

SOURCES +=  hgridcellbase.cpp \
            hgridcell.cpp   \
            hgridctrl.cpp \
            hinplaceedit.cpp \
            hgridctrlcommand.cpp \
    hcellrange.cpp
            #main.cpp

HEADERS  +=    \
            ../include/hcellrange.h \
            ../include/hgridcelldef.h \
            ../include/hgridcellbase.h \
            ../include/hgridcell.h     \
            ../include/hinplaceedit.h  \
            ../include/hgridctrl.h \
            ../include/hgridglobal.h \
            ../include/hgridctrlcommand.h



unix{
    UI_DIR = temp/ui
    MOC_DIR = temp/moc
    OBJECTS_DIR	= temp/obj
}

win32{
    UI_DIR = temp/ui
    MOC_DIR = temp/moc
    OBJECTS_DIR	= temp/obj
}
