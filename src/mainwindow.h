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
        void setWindowSize(int width, int height, int alignment);
        int _cameraWidth = 0;
        int _cameraHeight = 0;
        int _lastLeft = 0;
        int _lastTop = 0;
        int _lastWidth = 0;
        int _lastHeight = 0;
        int _lastAlignment = -1;  // -1 is floating

    private:
        Ui::MainWindow* ui;
        QString _cameraName;
        QScopedPointer<QCamera> _camera;
        QTimer* _statusUpdateTimer = nullptr;

    private slots:
        void onMenuCameraSelected();
        void onStatusUpdate();

};
