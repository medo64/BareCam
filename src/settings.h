#pragma once

class Settings {

    public:

        static bool useEscapeToExit();
        static void setUseEscapeToExit(bool newUseEscapeToExit);
        static bool defaultUseEscapeToExit() { return true; }

};
