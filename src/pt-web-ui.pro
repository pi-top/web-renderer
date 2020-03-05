QT += quick
QT += webengine

CONFIG += c++
CONFIG += disable-desktop

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp

RESOURCES += qml.qrc
linux:contains(QMAKE_HOST.arch, arm.*) {
    message("Host is Pi - leaving out QRC")
    RESOURCES -= qml.qrc
}


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target
