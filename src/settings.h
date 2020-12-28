#pragma once

#include <QString>

class Settings {

    public:
        static bool useEscapeToExit();
        static void setUseEscapeToExit(bool newUseEscapeToExit);
        static bool defaultUseEscapeToExit() { return true; }

        static bool disableScreensaver();
        static void setDisableScreensaver(bool newDisableScreensaver);
        static bool defaultDisableScreensaver() { return true; }

        static bool fullScreenStartup();
        static void setFullScreenStartup(bool newFullScreenStartup);
        static bool defaultFullScreenStartup() { return true; }

    public:
        static QString lastUsedDevice();
        static void setLastUsedDevice(QString newUseEscapeToExit);
        static QString defaultLastUsedDevice() { return QString(); }

        static int lastAlignment();
        static void setLastAlignment(int newLastAlignment);
        static int defaultLastAlignment() { return 0; }

};
