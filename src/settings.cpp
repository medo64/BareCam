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
    int value = Config::stateRead("Alignment", defaultLastAlignment());
    if ((value < -1) || (value > 9)) { return  defaultLastAlignment(); }
    return value;
}

void Settings::setLastAlignment(int newLastAlignment) {
    Config::stateWrite("Alignment", newLastAlignment);
}


int Settings::lastLeft() {
    return Config::stateRead("Left", defaultLastLeft());
}

void Settings::setLastLeft(int newLastLeft) {
    Config::stateWrite("Left", newLastLeft);
}


int Settings::lastTop() {
    return Config::stateRead("Top", defaultLastTop());
}

void Settings::setLastTop(int newLastTop) {
    Config::stateWrite("Top", newLastTop);
}


int Settings::lastWidth() {
    int value = Config::stateRead("Width", defaultLastWidth());
    if (value < 0) { return  defaultLastWidth(); }
    return value;
}

void Settings::setLastWidth(int newLastWidth) {
    Config::stateWrite("Width", newLastWidth);
}


int Settings::lastHeight() {
    int value = Config::stateRead("Height", defaultLastHeight());
    if (value < 0) { return  defaultLastHeight(); }
    return value;
}

void Settings::setLastHeight(int newLastHeight) {
    Config::stateWrite("Height", newLastHeight);
}
