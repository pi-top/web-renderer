QT += quick

CONFIG += c++
CONFIG += disable-desktop

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target
