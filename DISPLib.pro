QT += core
QT -= gui

TARGET = testLib
TEMPLATE = lib

DEFINES += DISPLIB_LIBRARY

TARGET = DISPLib
CONFIG += lib

SOURCES += \
    DIPSLib.cpp \
    processor.cpp

HEADERS += \
    DIPSLib.h \
    Resource.h \
    displib_global.h \
    CmnHdr.h \
    processor.h

