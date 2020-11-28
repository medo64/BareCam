#include "medo/config.h"
#include "settings.h"


bool Settings::useEscapeToExit() {
    return Config::read("UseEscapeToExit", defaultUseEscapeToExit());
}

void Settings::setUseEscapeToExit(bool newUseEscapeToExit) {
    Config::write("UseEscapeToExit", newUseEscapeToExit);
}



// state

QString Settings::lastUsedDevice() {
    return Config::stateRead("LastUsedDevice", defaultLastUsedDevice());
}

void Settings::setLastUsedDevice(QString newLastUsedDevice) {
    Config::stateWrite("LastUsedDevice", newLastUsedDevice);
}
