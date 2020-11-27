#include "medo/singleinstance.h"
#include "icons.h"
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

    startNextCamera();

    _statusUpdateTimer = new QTimer(this);
    connect(_statusUpdateTimer, &QTimer::timeout, this, &MainWindow::onStatusUpdate);
    _statusUpdateTimer->start(1000);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    QMessageBox msgBox;

    if (e->key() == Qt::Key_Space) {
        startNextCamera();
    }

    QMainWindow::keyPressEvent(e);
}


void MainWindow::startNextCamera() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString newDescription;

    if (_cameraName.isNull()) { //start new
        _cameraName = QCameraInfo::defaultCamera().deviceName();
        newDescription = QCameraInfo::defaultCamera().description();
    } else {
        auto cameras = QCameraInfo::availableCameras();
        auto camerasCount = cameras.length();
        int currentIndex = 0;
        for (int i = 0; i < camerasCount; i++) {
            if (cameras[i].deviceName() == _cameraName) {
                currentIndex = i;
                break;
            }
        }
        auto camera = cameras[(currentIndex + 1) % camerasCount];
        _cameraName = camera.deviceName();
        newDescription = camera.description();
    }

    if (!_camera.isNull()) { disconnect(_camera.data(), &QCamera::stateChanged, this, &MainWindow::onStatusUpdate); }

    if (!newDescription.isEmpty()) {
        statusBar()->setVisible(true);
        statusBar()->showMessage("Activating " + newDescription);
        statusBar()->show();
        QCoreApplication::processEvents();
    }

    _camera.reset(new QCamera(_cameraName.toLatin1()));
    _camera->setViewfinder(ui->viewfinder);

    connect(_camera.data(), &QCamera::stateChanged, this, &MainWindow::onStatusUpdate);

    qDebug().noquote().nospace() << "[Camera] Starting " << _cameraName;
    _camera->start();

    QApplication::restoreOverrideCursor();
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
