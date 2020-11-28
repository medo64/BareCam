#pragma once

#include <QString>

class Settings {

    public:
        static bool useEscapeToExit();
        static void setUseEscapeToExit(bool newUseEscapeToExit);
        static bool defaultUseEscapeToExit() { return true; }

    public:
        static QString lastUsedDevice();
        static void setLastUsedDevice(QString newUseEscapeToExit);
        static QString defaultLastUsedDevice() { return QString(); }

};
