QT += quick
QT += webengine
QT += webenginewidgets

CONFIG += c++
CONFIG += disable-desktop

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    config.cpp \
    console_log_handler.cpp \
    fileio.cpp \
    oshelper.cpp \
    ptlogger.cpp

HEADERS += unix_signal_manager.h \
    config.h \
    console_log_handler.h \
    fileio.h \
    oshelper.h \
    ptlogger.h

LIBS += -lcrypt

RESOURCES += qml.qrc
linux:contains(QMAKE_HOST.arch, arm.*) {
    message("Host is Pi - leaving out QRC")
    RESOURCES -= qml.qrc
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target
