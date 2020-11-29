#include <QProcess>
#include "screensaver.h"

#if defined(Q_OS_WIN)
    EXECUTION_STATE* Screensaver::_lastExecutionState = nullptr;
#elif defined(Q_OS_LINUX)
    WId* Screensaver::_suspendedWindowId = nullptr;
#endif

void Screensaver::Suspend(WId windowId) {
#if defined(Q_OS_WIN)
    Q_UNUSED(windowId)
    if (_lastExecutionState == nullptr) { //do it only once
        _lastExecutionState = new EXECUTION_STATE();
        *_lastExecutionState = SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
    }
#elif defined(Q_OS_LINUX)
    if (_suspendedWindowId == nullptr) { //do it only once
        QProcess::startDetached("xdg-screensaver", QStringList({ "suspend", "0x" + QString::number(windowId, 16) }));
        _suspendedWindowId = new WId();
        *_suspendedWindowId = windowId;
    }
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
    if (_suspendedWindowId != nullptr) { //do it only once
        QProcess::startDetached("xdg-screensaver", QStringList({ "result", "0x" + QString::number(*_suspendedWindowId, 16) }));
        delete _suspendedWindowId;
        _suspendedWindowId = nullptr;
    }
#endif
}
