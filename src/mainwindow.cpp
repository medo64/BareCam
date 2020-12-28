#include <math.h>
#include <QCameraInfo>
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
            if (_lastAlignment != 0) {
                setWindowSize(_lastWidth, _lastHeight, 0); break;
            } else {
                setWindowSize(_lastWidth, _lastHeight, -1); break;
            }

        case Qt::Key_1: setWindowSize(_lastWidth, _lastHeight, 1); break;
        case Qt::Key_2: setWindowSize(_lastWidth, _lastHeight, 2); break;
        case Qt::Key_3: setWindowSize(_lastWidth, _lastHeight, 3); break;
        case Qt::Key_4: setWindowSize(_lastWidth, _lastHeight, 4); break;
        case Qt::Key_5: setWindowSize(_lastWidth, _lastHeight, 5); break;
        case Qt::Key_6: setWindowSize(_lastWidth, _lastHeight, 6); break;
        case Qt::Key_7: setWindowSize(_lastWidth, _lastHeight, 7); break;
        case Qt::Key_8: setWindowSize(_lastWidth, _lastHeight, 8); break;
        case Qt::Key_9: setWindowSize(_lastWidth, _lastHeight, 9); break;

        case Qt::Key_Escape:
            if (Settings::useEscapeToExit()) { close(); }
            break;

        case Qt::Key_Return:
            showMenu();
            break;

        case Qt::Key_Space:
            startNextCamera();
            break;

    }

    QMainWindow::keyPressEvent(e);
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
    setWindowSize(this->width(), this->height(), 0);

    connect(_camera.data(), &QCamera::stateChanged, this, &MainWindow::onStatusUpdate);

    qDebug().noquote().nospace() << "[Camera] Starting " << _cameraName;
    _camera->start();

    QApplication::restoreOverrideCursor();
}

void MainWindow::showMenu() {
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

    auto rect = this->contentsRect();
    auto point = rect.center();
    point.rx() -= menu.sizeHint().width() / 2; //move it left a bit
    point.ry() -= menu.sizeHint().height() / 2; //move it up a bit
    menu.exec(point);
}


void MainWindow::onMenuCameraSelected() {
    QAction* action = qobject_cast<QAction*>(sender());
    QString deviceName = action->data().toString();
    startNextCamera(deviceName);
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

void MainWindow::setWindowSize(int width, int height, int alignment) {
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
        case 1:  // bottom-left
            rect.moveLeft(desktopRect.left());
            rect.moveTop(desktopRect.bottom() - newHeight + 1);
            break;

        case 2:  // bottom-center
            rect.moveLeft(desktopRect.left() + (desktopRect.width() - newWidth) / 2);
            rect.moveTop(desktopRect.bottom() - newHeight + 1);
            break;

        case 3:  // bottom-right
            rect.moveLeft(desktopRect.right() - newWidth + 1);
            rect.moveTop(desktopRect.bottom() - newHeight + 1);
            break;

        case 4:  // middle-left
            rect.moveLeft(desktopRect.left());
            rect.moveTop(desktopRect.top() + (desktopRect.height() - newHeight) / 2);
            break;

        case 5:  // middle-center
            rect.moveLeft(desktopRect.left() + (desktopRect.width() - newWidth) / 2);
            rect.moveTop(desktopRect.top() + (desktopRect.height() - newHeight) / 2);
            break;

        case 6:  // middle-right
            rect.moveLeft(desktopRect.right() - newWidth + 1);
            rect.moveTop(desktopRect.top() + (desktopRect.height() - newHeight) / 2);
            break;

        case 7:  // top-left
            rect.moveLeft(desktopRect.left());
            rect.moveTop(desktopRect.top());
            break;

        case 8:  // top-center
            rect.moveLeft(desktopRect.left() + (desktopRect.width() - newWidth) / 2);
            rect.moveTop(desktopRect.top());
            break;

        case 9:  // top-right
            rect.moveLeft(desktopRect.right() - newWidth + 1);
            rect.moveTop(desktopRect.top());
            break;

        case 0:  // full-screen
            break;

        default: // free flowing
            rect.moveLeft(_lastLeft);
            rect.moveTop(_lastTop);
            if (rect.left() + newWidth > desktopRect.right()) { rect.moveLeft(desktopRect.right() - newWidth); }
            if (rect.top() + newHeight > desktopRect.bottom()) { rect.moveTop(desktopRect.bottom() - newHeight); }
            break;
    }

    if (alignment == 0) {  // full screen

        if (_lastAlignment != 0) {
            setCursor(Qt::BlankCursor);
            setWindowState(Qt::WindowFullScreen);
        }

    } else {  // not full screen

        if (_lastAlignment == 0) {
            setWindowState(Qt::WindowActive);
            this->unsetCursor();
        }

        this->setGeometry(rect);

        if (_lastAlignment != 0) {
            if (_lastAlignment == -1) {
                _lastLeft = rect.left();
                _lastTop = rect.top();
            }
            _lastWidth = rect.width();
            _lastHeight = rect.height();
        }
    }

    _lastAlignment = alignment;
}
