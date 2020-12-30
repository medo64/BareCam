#include <math.h>
#include <QCameraInfo>
#include <QDebug>
#include <QGuiApplication>
#include <QtWidgets>
#include "medo/singleinstance.h"
#include "icons.h"
#include "screensaver.h"
#include "settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowIcon(Icons::app());

    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    //single instance
    connect(SingleInstance::instance(), &SingleInstance::newInstanceDetected, this, [&]() {
        this->hide(); //workaround for Ubuntu
        this->show();
        this->raise(); //workaround for MacOS
        this->activateWindow(); //workaround for Windows
    });

    // restore settings
    _lastAlignment = (Alignment)Settings::lastAlignment();
    _lastLeft = Settings::lastLeft();
    _lastTop = Settings::lastTop();
    _lastWidth = Settings::lastWidth();
    _lastHeight = Settings::lastHeight();
    qDebug().noquote().nospace() << "[Window] Size restored: " << _lastLeft << ", " << _lastTop << ", " << _lastWidth << ", " << _lastHeight << " /" << _lastAlignment;

    // just disable screensaver
    if (Settings::disableScreensaver()) { Screensaver::Suspend(winId()); }

    //debug
#ifdef QT_DEBUG
    auto defaultCamera = QCameraInfo::defaultCamera();
    for (const QCameraInfo& dCameraInfo : QCameraInfo::availableCameras()) {
        bool isDefault = (dCameraInfo == defaultCamera);
        qDebug().noquote().nospace() << "[Camera]" << dCameraInfo.description() << (isDefault ? "*" : "");
        qDebug().noquote().nospace() << "[Camera]  " << dCameraInfo.deviceName();
        switch (dCameraInfo.position()) {
            case QCamera::FrontFace:
                qDebug().noquote().nospace() << "[Camera]  Rotation: " << dCameraInfo.orientation() << "° (front)";
                break;
            case QCamera::BackFace:
                qDebug().noquote().nospace() << "[Camera]  Rotation: " << dCameraInfo.orientation() << "° (rear)";
                break;
            default:
                qDebug().noquote().nospace() << "[Camera]  Rotation: " << dCameraInfo.orientation() << "°";
                break;
        }

        auto dCamera = new QCamera(dCameraInfo);
        dCamera->load();
        if (dCamera->state() == QCamera::LoadedState) {
            QString dResolutions;
            for (const QCameraViewfinderSettings& dSettings : dCamera->supportedViewfinderSettings()) {
                if (!dResolutions.isEmpty()) { dResolutions += ", "; }
                auto width = dSettings.resolution().width();
                auto height = dSettings.resolution().height();
                auto minFramerate = (int)dSettings.minimumFrameRate();
                auto maxFramerate = (int)dSettings.maximumFrameRate();
                if (minFramerate == maxFramerate) {
                    dResolutions += (QString::number(width) + "x" + QString::number(height) + "@" + QString::number(maxFramerate));
                } else {
                    dResolutions += (QString::number(width) + "x" + QString::number(height) + "@" + QString::number(minFramerate) + "-" + QString::number(maxFramerate));
                }
            }
            qDebug().noquote().nospace() << "[Camera]  " << "Resoultions: " << dResolutions;
        }
        dCamera->unload();
        delete dCamera;
    }
#endif

    startNextCamera(Settings::lastUsedDevice());
    if (Settings::fullScreenStartup()) {
        setWindowSize(0, 0, Alignment::FullScreen);
    }

    _statusUpdateTimer = new QTimer(this);
    connect(_statusUpdateTimer, &QTimer::timeout, this, &MainWindow::onStatusUpdate);
    _statusUpdateTimer->start(1000);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    Screensaver::Resume();
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    QMessageBox msgBox;

    switch (e->key()) {

        case Qt::Key_0:
            if (_lastAlignment != Alignment::FullScreen) {
                setWindowSize(_lastWidth, _lastHeight, Alignment::FullScreen);
            } else {
                setWindowSize(_lastWidth, _lastHeight, Alignment::Custom);
            }
            break;

        case Qt::Key_1: setWindowSize(_lastWidth, _lastHeight, Alignment::LowerLeft); break;
        case Qt::Key_2: setWindowSize(_lastWidth, _lastHeight, Alignment::LowerCenter); break;
        case Qt::Key_3: setWindowSize(_lastWidth, _lastHeight, Alignment::LowerRight); break;
        case Qt::Key_4: setWindowSize(_lastWidth, _lastHeight, Alignment::MiddleLeft); break;
        case Qt::Key_5: setWindowSize(_lastWidth, _lastHeight, Alignment::MiddleCenter); break;
        case Qt::Key_6: setWindowSize(_lastWidth, _lastHeight, Alignment::MiddleRight); break;
        case Qt::Key_7: setWindowSize(_lastWidth, _lastHeight, Alignment::UpperLeft); break;
        case Qt::Key_8: setWindowSize(_lastWidth, _lastHeight, Alignment::UpperCenter); break;
        case Qt::Key_9: setWindowSize(_lastWidth, _lastHeight, Alignment::UpperRight); break;

        case Qt::Key_Minus: changeWindowSize(-1); break;
        case Qt::Key_Plus: changeWindowSize(+1); break;

        case Qt::Key_Escape:
            if (Settings::useEscapeToExit()) { close(); }
            break;

        case Qt::Key_Return: {
                auto rect = this->contentsRect();
                auto point = rect.center();
                showMenu(point);
            }
            break;

        case Qt::Key_Space:
            startNextCamera();
            break;

    }

    QMainWindow::keyPressEvent(e);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        if (_lastAlignment != Alignment::FullScreen) {
            setWindowSize(_lastWidth, _lastHeight, Alignment::FullScreen);
        } else {
            setWindowSize(_lastWidth, _lastHeight, Alignment::Custom);
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if (_lastAlignment != Alignment::FullScreen) {
        if (!_lastClickLocation.isNull()) {
            QPoint delta = event->globalPos() - _lastClickLocation;
            move(x() + delta.x(), y() + delta.y());
            _lastClickLocation = event->globalPos();
            _lastAlignment = Alignment::Custom; //switch to custom alignment
        }
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::RightButton) {
        showMenu(event->pos());
    } else if ((windowState() != Qt::WindowFullScreen) && (event->button() == Qt::MouseButton::LeftButton)) {
        qDebug().noquote().nospace() << "[Window] Mouse pressed: " << geometry().left() << ", " << geometry().top() << ", " << geometry().width() << ", " << geometry().height();
        _lastClickLocation = event->globalPos();
        setCursor(Qt::SizeAllCursor);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        _lastClickLocation = QPoint();
        if (windowState() != Qt::WindowFullScreen) {
            qDebug().noquote().nospace() << "[Window] Mouse released: " << geometry().left() << ", " << geometry().top() << ", " << geometry().width() << ", " << geometry().height();
            unsetCursor();
            _lastLeft = geometry().left();
            _lastTop = geometry().top();
            Settings::setLastLeft(_lastLeft);
            Settings::setLastTop(_lastTop);
        }
    }
}


void MainWindow::startNextCamera(QString deviceName) {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString newDescription;

    auto cameras = QCameraInfo::availableCameras();
    auto camerasCount = cameras.length();

    QCameraInfo nextCamera = QCameraInfo::defaultCamera();

    if (_cameraName.isNull() || !deviceName.isEmpty()) { //start new
        for (int i = 0; i < camerasCount; i++) {
            if (cameras[i].deviceName() == deviceName) {
                nextCamera = cameras[i];
                break;
            }
        }
    } else { //switch to next
        for (int i = 0; i < camerasCount; i++) {
            if (cameras[i].deviceName() == _cameraName) {
                nextCamera = cameras[(i + 1) % camerasCount];
                break;
            }
        }
    }

    _cameraName = nextCamera.deviceName();
    newDescription = nextCamera.description();

    if (!_camera.isNull()) { disconnect(_camera.data(), &QCamera::stateChanged, this, &MainWindow::onStatusUpdate); }

    if (!newDescription.isEmpty()) {
        statusBar()->setVisible(true);
        statusBar()->showMessage("Activating " + newDescription);
        statusBar()->show();
        QCoreApplication::processEvents();
    }

    Settings::setLastUsedDevice(_cameraName);
    _camera.reset(new QCamera(_cameraName.toLatin1()));
    _camera->setViewfinder(ui->viewfinder);

    _camera->load();
    for (const QCameraViewfinderSettings& dSettings : _camera->supportedViewfinderSettings()) {
        _cameraWidth = dSettings.resolution().width();
        _cameraHeight = dSettings.resolution().height();
        break;
    }
    setWindowSize(0, 0, _lastAlignment);

    connect(_camera.data(), &QCamera::stateChanged, this, &MainWindow::onStatusUpdate);

    qDebug().noquote().nospace() << "[Camera] Starting " << _cameraName;
    _camera->start();

    QApplication::restoreOverrideCursor();
}

void MainWindow::showMenu(QPoint location) {
    QMenu menu(this);

    QString currentDeviceName = Settings::lastUsedDevice();

    for (const QCameraInfo& camera : QCameraInfo::availableCameras()) {
        QString deviceName = camera.deviceName();
        auto action = menu.addAction(camera.description(), this, &MainWindow::onMenuCameraSelected);
        action->setData(deviceName);
        if (deviceName == currentDeviceName) {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }

    menu.addSeparator();
    {
        QMenu* settingsMenu = new QMenu("&Settings", this);
        menu.addMenu(settingsMenu);

        auto fullscreenAction = settingsMenu->addAction("Force &Full-screen on Startup", this, &MainWindow::onMenuSettingsFullscreenStartup);
        fullscreenAction->setCheckable(true);
        fullscreenAction->setChecked(Settings::fullScreenStartup());

        auto useEscapeAction = settingsMenu->addAction("Use &Escape to Exit", this, &MainWindow::onMenuSettingsUseEscape);
        useEscapeAction->setCheckable(true);
        useEscapeAction->setChecked(Settings::useEscapeToExit());

        auto disableScreensaverAction = settingsMenu->addAction("Disable &Screensaver", this, &MainWindow::onMenuSettingsDisableScreensaver);
        disableScreensaverAction->setCheckable(true);
        disableScreensaverAction->setChecked(Settings::disableScreensaver());
    }

    menu.addAction("&About", this, &MainWindow::onMenuAbout);


    auto point = mapToGlobal(location);
    point.rx() -= menu.sizeHint().width() / 2; //move it left a bit
    point.ry() -= menu.sizeHint().height() / 2; //move it up a bit

    menu.exec(location);
}


void MainWindow::onMenuCameraSelected() {
    QAction* action = qobject_cast<QAction*>(sender());
    QString deviceName = action->data().toString();
    startNextCamera(deviceName);
}

void MainWindow::onMenuAbout() {
    QString description = QCoreApplication::applicationName() + " " + APP_VERSION;
    QString commit = APP_COMMIT;
    if (commit.length() > 0) {
        description.append("+");
        description.append(APP_COMMIT);
    }
#ifdef QT_DEBUG
    description.append("\nDEBUG");
#endif
    description.append("\n\nQt ");

    QString runtimeVersion = qVersion();
    QString compileVersion = APP_QT_VERSION;
    description.append(runtimeVersion);
    if (runtimeVersion != compileVersion) {
        description.append(" / ");
        description.append(compileVersion);
    }
    description.append("\n" + QSysInfo::prettyProductName());
    description.append("\n" + QSysInfo::kernelType() + " " + QSysInfo::kernelVersion());
    QMessageBox::about(this, "About",  description);
}

void MainWindow::onMenuSettingsFullscreenStartup() {
    Settings::setFullScreenStartup(!Settings::fullScreenStartup());
}

void MainWindow::onMenuSettingsUseEscape() {
    Settings::setUseEscapeToExit(!Settings::useEscapeToExit());
}

void MainWindow::onMenuSettingsDisableScreensaver() {
    Settings::setDisableScreensaver(!Settings::disableScreensaver());
    if (Settings::disableScreensaver()) {
        Screensaver::Suspend(winId());
    } else {
        Screensaver::Resume();
    }
}


void MainWindow::onStatusUpdate() {
    bool hasIssues = !_camera->isAvailable();
    if (hasIssues) { //show issues
        QString errorText = _camera->errorString();
        if (errorText.isEmpty()) { errorText = "Not available."; }
        if (!statusBar()->isVisible() || (errorText != statusBar()->currentMessage())) {
            this->statusBar()->showMessage(errorText);
            this->statusBar()->setVisible(true);
        }
    } else if (!hasIssues && this->statusBar()->isVisible()) { //no issues anymore
        this->statusBar()->setVisible(false);
        this->statusBar()->clearMessage();
    }
}

void MainWindow::setWindowSize(int width, int height, Alignment alignment) {
    int newWidth = width;
    int newHeight = height;

    auto screenNumber = QApplication::desktop()->screenNumber(this);
    QRect desktopRect = QGuiApplication::screens().at(screenNumber)->geometry();
    if (width == 0) { width = desktopRect.width() / 3; }
    if (height == 0) { height = desktopRect.height() / 3; }
    if (width > desktopRect.width()) { width = desktopRect.width(); }
    if (height > desktopRect.height()) { height = desktopRect.height(); }

    if ((_cameraWidth != 0) && (_cameraHeight != 0)) {  // check ratios if camera width and height are unknown
        newHeight = (int)round(((double)width / _cameraWidth) * _cameraHeight);
        if (newHeight <= height) {  // if window height is not too much, use it
            newWidth = width;
        } else {  // recalculate width instead
            newWidth = (int)round(((double)height / _cameraHeight) * _cameraWidth);
            newHeight = height;
        }
    }

    QRect rect = this->geometry();
    rect.setWidth(newWidth);
    rect.setHeight(newHeight);

    switch (alignment) {
        case Alignment::LowerLeft:
            rect.moveLeft(desktopRect.left());
            rect.moveTop(desktopRect.bottom() - newHeight + 1);
            break;

        case Alignment::LowerCenter:
            rect.moveLeft(desktopRect.left() + (desktopRect.width() - newWidth) / 2);
            rect.moveTop(desktopRect.bottom() - newHeight + 1);
            break;

        case Alignment::LowerRight:
            rect.moveLeft(desktopRect.right() - newWidth + 1);
            rect.moveTop(desktopRect.bottom() - newHeight + 1);
            break;

        case Alignment::MiddleLeft:
            rect.moveLeft(desktopRect.left());
            rect.moveTop(desktopRect.top() + (desktopRect.height() - newHeight) / 2);
            break;

        case Alignment::MiddleCenter:
            rect.moveLeft(desktopRect.left() + (desktopRect.width() - newWidth) / 2);
            rect.moveTop(desktopRect.top() + (desktopRect.height() - newHeight) / 2);
            break;

        case Alignment::MiddleRight:
            rect.moveLeft(desktopRect.right() - newWidth + 1);
            rect.moveTop(desktopRect.top() + (desktopRect.height() - newHeight) / 2);
            break;

        case Alignment::UpperLeft:
            rect.moveLeft(desktopRect.left());
            rect.moveTop(desktopRect.top());
            break;

        case Alignment::UpperCenter:
            rect.moveLeft(desktopRect.left() + (desktopRect.width() - newWidth) / 2);
            rect.moveTop(desktopRect.top());
            break;

        case Alignment::UpperRight:
            rect.moveLeft(desktopRect.right() - newWidth + 1);
            rect.moveTop(desktopRect.top());
            break;

        case Alignment::FullScreen:
            break;

        default: // free flowing
            rect.moveLeft(_lastLeft);
            rect.moveTop(_lastTop);
            if (rect.left() + newWidth > desktopRect.right()) { rect.moveLeft(desktopRect.right() - newWidth); }
            if (rect.top() + newHeight > desktopRect.bottom()) { rect.moveTop(desktopRect.bottom() - newHeight); }
            break;
    }

    if (alignment == Alignment::FullScreen) {  // full screen

        if (windowState() != Qt::WindowFullScreen) {
            setCursor(Qt::BlankCursor);
            setWindowState(Qt::WindowFullScreen);
        }

    } else {  // not full screen

        if (windowState() == Qt::WindowFullScreen) {
            setWindowState(Qt::WindowActive);
            this->unsetCursor();
        }

        this->setGeometry(rect);

        if (windowState() != Qt::WindowFullScreen) {
            if (_lastAlignment == Alignment::Custom) {
                _lastLeft = rect.left();
                _lastTop = rect.top();
            }
            _lastWidth = rect.width();
            _lastHeight = rect.height();
        }
    }

    qDebug().noquote().nospace() << "[Window] Sized: " << geometry().left() << ", " << geometry().top() << ", " << geometry().width() << ", " << geometry().height() << " /" << alignment;

    _lastAlignment = alignment;
    Settings::setLastAlignment((int)_lastAlignment);

    if (alignment != Alignment::FullScreen) {  // store width and height if not full screen
        if (alignment == Alignment::Custom) {  // store left and top if custom
            Settings::setLastLeft(_lastLeft);
            Settings::setLastTop(_lastTop);
        }
        Settings::setLastWidth(_lastWidth);
        Settings::setLastHeight(_lastHeight);
    }

    qDebug().noquote().nospace() << "[Window] Size saved: " << _lastLeft << ", " << _lastTop << ", " << _lastWidth << ", " << _lastHeight << " /" << _lastAlignment;
}

void MainWindow::changeWindowSize(int difference) {
    if (_lastAlignment == Alignment::FullScreen) { return; }  // ignore if full screen

    int width = this->width();
    int height = this->height();
    double ratio = (double)_cameraWidth / _cameraHeight;

    int newWidth, newHeight;
    if (height <= width) {
        int delta = difference * std::max(1, height / 100);  // to have a bit larger steps
        newWidth = (int)round((height + delta) * ratio);
        newHeight = (int)round(newWidth / ratio);
    } else {
        int delta = difference * std::max(1, width / 100);  // to have a bit larger steps
        newHeight = (int)round((width + delta) / ratio);
        newWidth = (int)round(newHeight * ratio);
    }

    if ((newWidth < 200) || (newHeight < 200)) { return; }
    setWindowSize(newWidth, newHeight, _lastAlignment);
}
