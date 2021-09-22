QT += quick
QT += webengine
QT += webenginewidgets

CONFIG += c++
CONFIG += disable-desktop
CONFIG += warn_on

DEFINES += QT_DEPRECATED_WARNINGS

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = web-renderer
TEMPLATE = app

DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32
macx:OBJECTS_DIR = ../build/o/mac

SOURCES += main.cpp \
    console_log_handler.cpp \
    logger.cpp

HEADERS += unix_signal_manager.h \
    console_log_handler.h \
    logger.h

LIBS += -lcrypt


RESOURCES += qml.qrc
linux:contains(QMAKE_HOST.arch, arm.*) {
    # Leave out QRC to fetch from /usr/lib/web-renderer/ on Pi
    RESOURCES -= qml.qrc
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target
