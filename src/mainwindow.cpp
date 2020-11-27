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

    //camera
    auto defaultCamera = QCameraInfo::defaultCamera();

    //debug
#ifdef QT_DEBUG
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

    _camera.reset(new QCamera(defaultCamera));
    _camera->setViewfinder(ui->viewfinder);
    _camera->start();
}
