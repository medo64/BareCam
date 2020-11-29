#pragma once

#include <QCamera>
#include <QScopedPointer>
#include <QTimer>


#include <QMainWindow>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        MainWindow();

    protected:
        virtual void closeEvent(QCloseEvent* event);
        virtual void keyPressEvent(QKeyEvent* event);

    private:
        void startNextCamera(QString deviceName = QString());
        void showMenu();

    private:
        Ui::MainWindow* ui;
        QString _cameraName;
        QScopedPointer<QCamera> _camera;
        QTimer* _statusUpdateTimer = nullptr;

    private slots:
        void onMenuCameraSelected();
        void onStatusUpdate();

};
