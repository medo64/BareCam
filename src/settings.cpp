#include "medo/config.h"
#include "settings.h"


bool Settings::useEscapeToExit() {
    return Config::read("UseEscapeToExit", defaultUseEscapeToExit());
}

void Settings::setUseEscapeToExit(bool newUseEscapeToExit) {
    Config::write("UseEscapeToExit", newUseEscapeToExit);
}


bool Settings::disableScreensaver() {
    return Config::read("DisableScreensaver", defaultDisableScreensaver());
}

void Settings::setDisableScreensaver(bool newDisableScreensaver) {
    Config::write("DisableScreensaver", newDisableScreensaver);
}


bool Settings::fullScreenStartup() {
    return Config::read("FullScreenStartup", defaultFullScreenStartup());
}

void Settings::setFullScreenStartup(bool newFullScreenStartup) {
    Config::write("FullScreenStartup", newFullScreenStartup);
}


// state

QString Settings::lastUsedDevice() {
    return Config::stateRead("LastUsedDevice", defaultLastUsedDevice());
}

void Settings::setLastUsedDevice(QString newLastUsedDevice) {
    Config::stateWrite("LastUsedDevice", newLastUsedDevice);
}


int Settings::lastAlignment() {
    int value = Config::stateRead("LastAlignment", defaultLastAlignment());
    if ((value < -1) || (value > 9)) { return  defaultLastAlignment(); }
    return value;
}

void Settings::setLastAlignment(int newLastAlignment) {
    Config::stateWrite("LastAlignment", newLastAlignment);
}
