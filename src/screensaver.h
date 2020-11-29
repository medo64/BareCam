#pragma once

#include <QtGlobal>
#if defined(Q_OS_WIN)
    #include <windows.h>
#elif defined(Q_OS_LINUX)
#else
    #error "Only Linux and Windows are supported!"
#endif

class Screensaver {

    public:
        static void Suspend();
        static void Resume();

    private:
#if defined(Q_OS_WIN)
        static EXECUTION_STATE* _lastExecutionState;
#endif

};
