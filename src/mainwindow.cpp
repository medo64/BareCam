#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCameraInfo>

#include <QtWidgets>

MainWindow::MainWindow() : ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    _camera->setViewfinder(ui->viewfinder);
    _camera->start();
}
