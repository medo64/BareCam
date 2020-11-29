#pragma once

#include <QWindow>
#if defined(Q_OS_WIN)
    #include <windows.h>
#elif defined(Q_OS_LINUX)
#else
    #error "Only Linux and Windows are supported!"
#endif

class Screensaver {

    public:
        static void Suspend(WId windowId);
        static void Resume();

#if defined(Q_OS_WIN)
    private:
        static EXECUTION_STATE* _lastExecutionState;
#elif defined(Q_OS_LINUX)
    private:
        static WId* _suspendedWindowId;
#endif

};
