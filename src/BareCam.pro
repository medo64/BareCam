APP_PRODUCT = "BareCam"
APP_COMPANY = "Josip Medved"
APP_VERSION = "0.0.0"
APP_COPYRIGHT = "Copyright 2020 Josip Medved <jmedved@jmedved.com>"
APP_DESCRIPTION = "The simplest full-screen camera."

DEFINES += "APP_PRODUCT=\"\\\"$$APP_PRODUCT\\\"\""
DEFINES += "APP_COMPANY=\"\\\"$$APP_COMPANY\\\"\""
DEFINES += "APP_VERSION=\\\"$$APP_VERSION\\\""
DEFINES += "APP_COPYRIGHT=\"\\\"$$APP_COPYRIGHT\\\"\""
DEFINES += "APP_DESCRIPTION=\"\\\"$$APP_DESCRIPTION\\\"\""
DEFINES += "APP_QT_VERSION=\\\"$$QT_VERSION\\\""

APP_COMMIT = $$system(git -C \"$$_PRO_FILE_PWD_\" log -n 1 --format=%h)
APP_COMMIT_DIFF = $$system(git -C \"$$_PRO_FILE_PWD_\" diff)
!isEmpty(APP_COMMIT_DIFF) {
    APP_COMMIT = $$upper($$APP_COMMIT) #upper-case if working directory is dirty
}
DEFINES += "APP_COMMIT=\"\\\"$$APP_COMMIT\\\"\""


TEMPLATE = app
TARGET = barecam

QT += core gui widgets
QT += network                       # SingleInstance:QLocalServer/QLocalSocket
QT += multimedia multimediawidgets

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
    medo/appsetupmutex.h \
    medo/config.h \
    medo/singleinstance.h \
    icons.h \
    mainwindow.h \
    screensaver.h \
    settings.h

SOURCES = \
    medo/appsetupmutex.cpp \
    medo/config.cpp \
    medo/singleinstance.cpp \
    app.cpp \
    icons.cpp \
    mainwindow.cpp \
    screensaver.cpp \
    settings.cpp

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
