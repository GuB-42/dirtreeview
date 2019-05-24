TEMPLATE = app
TARGET = dirtreeview

CONFIG += debug

# QMAKE_CC = clang
# QMAKE_CXX = clang++
# QMAKE_CXX = clang++

# QMAKE_CXXFLAGS_RELEASE += -g
# QMAKE_CFLAGS_RELEASE += -g
# QMAKE_LFLAGS_RELEASE =

QT = core gui xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
main.cpp \
MainApplication.cpp \
MainWindow.cpp \
Node.cpp \
ReaderThread.cpp \
TreeModel.cpp

HEADERS += \
MainApplication.h \
MainWindow.h \
Node.h \
ReaderThread.h \
TreeModel.h \
mempool.h

FORMS += \
MainWindow.ui
