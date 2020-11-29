#include "medo/singleinstance.h"
#include "icons.h"
#include "screensaver.h"
#include "settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCameraInfo>

#include <QtWidgets>

MainWindow::MainWindow() : ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowIcon(Icons::app());

    //single instance
    connect(SingleInstance::instance(), &SingleInstance::newInstanceDetected, this, [&]() {
        this->hide(); //workaround for Ubuntu
        this->show();
        this->raise(); //workaround for MacOS
        this->activateWindow(); //workaround for Windows
    });

    if (Settings::disableScreensaver()) { Screensaver::Suspend(); }

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
