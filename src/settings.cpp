#include "medo/config.h"
#include "settings.h"


bool Settings::useEscapeToExit() {
    return Config::read("UseEscapeToExit", defaultUseEscapeToExit());
}

void Settings::setUseEscapeToExit(bool newUseEscapeToExit) {
    Config::write("UseEscapeToExit", newUseEscapeToExit);
}
