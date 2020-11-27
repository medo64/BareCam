APP_PRODUCT = "BareCam"
APP_COMPANY = "Josip Medved"
APP_VERSION = "0.0.1"
APP_COPYRIGHT = "Copyright 2020 Josip Medved <jmedved@jmedved.com>"
APP_DESCRIPTION = "The simplest full-screen camera."

DEFINES += "APP_PRODUCT=\"\\\"$$APP_PRODUCT\\\"\""
DEFINES += "APP_COMPANY=\"\\\"$$APP_COMPANY\\\"\""
DEFINES += "APP_VERSION=\\\"$$APP_VERSION\\\""
DEFINES += "APP_COPYRIGHT=\"\\\"$$APP_COPYRIGHT\\\"\""
DEFINES += "APP_DESCRIPTION=\"\\\"$$APP_DESCRIPTION\\\"\""
DEFINES += "APP_QT_VERSION=\\\"$$QT_VERSION\\\""


TEMPLATE = app
TARGET = barecam

QT+= core gui widgets
QT += multimedia multimediawidgets

unix {
    QT  += x11extras
    LIBS += -lX11 -lxcb
}

win32 {
    QMAKE_TARGET_PRODUCT = $$APP_PRODUCT
    QMAKE_TARGET_COMPANY = $$APP_COMPANY
    QMAKE_TARGET_COPYRIGHT = $$APP_COPYRIGHT
    QMAKE_TARGET_DESCRIPTION = $$APP_DESCRIPTION
    RC_ICONS = icons/app.ico
    VERSION = $$APP_VERSION + ".0"
}


DEFINES += QT_DEPRECATED_WARNINGS
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

CONFIG += c++11
QMAKE_CXXFLAGS_WARN_ON += -Wall
QMAKE_CXXFLAGS_WARN_ON += -Wextra
QMAKE_CXXFLAGS_WARN_ON += -Wshadow
QMAKE_CXXFLAGS_WARN_ON += -Wdouble-promotion


HEADERS = \
    icons.h \
    mainwindow.h \
    medo/appsetupmutex.h \
    medo/singleinstance.h

SOURCES = \
    app.cpp \
    icons.cpp \
    mainwindow.cpp \
    medo/appsetupmutex.cpp \
    medo/singleinstance.cpp

FORMS += \
    mainwindow.ui


RESOURCES += \
    BareCam.qrc

DISTFILES += \
    .astylerc


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
