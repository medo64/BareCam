#include "screensaver.h"

#if defined(Q_OS_WIN)
EXECUTION_STATE* Screensaver::_lastExecutionState = nullptr;
#endif

void Screensaver::Suspend() {
#if defined(Q_OS_WIN)
    if (_lastExecutionState == nullptr) { //do it only once
        _lastExecutionState = new EXECUTION_STATE();
        *_lastExecutionState = SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
    }
#elif defined(Q_OS_LINUX)
#endif
}

void Screensaver::Resume() {
#if defined(Q_OS_WIN)
    if (_lastExecutionState != nullptr) { //do it only once
        SetThreadExecutionState(*_lastExecutionState);
        delete _lastExecutionState;
        _lastExecutionState = nullptr;
    }
#elif defined(Q_OS_LINUX)
#endif
}
