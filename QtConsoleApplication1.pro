# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = app
TARGET = QtConsoleApplication1
DESTDIR = ../Win32/Release
QT += core
CONFIG += release console
DEFINES += QT_DLL
INCLUDEPATH += . \
    ./GeneratedFiles/Release \
    ../../../boost_1_62_0
LIBS += -L"../../../boost_1_62_0/lib32/lib"
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(QtConsoleApplication1.pri)
