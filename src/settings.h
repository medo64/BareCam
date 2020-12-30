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

        static int lastLeft();
        static void setLastLeft(int newLastLeft);
        static int defaultLastLeft() { return 0; }

        static int lastTop();
        static void setLastTop(int newLastTop);
        static int defaultLastTop() { return 0; }

        static int lastWidth();
        static void setLastWidth(int newLastWidth);
        static int defaultLastWidth() { return 0; }

        static int lastHeight();
        static void setLastHeight(int newLastHeight);
        static int defaultLastHeight() { return 0; }

};
