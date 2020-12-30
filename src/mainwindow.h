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
        virtual void mouseDoubleClickEvent(QMouseEvent* event);
        virtual void mouseMoveEvent(QMouseEvent* event);
        virtual void mousePressEvent(QMouseEvent* event);
        virtual void mouseReleaseEvent(QMouseEvent* event);

    private:
        enum Alignment {
            Custom = -1,
            FullScreen = 0,
            LowerLeft = 1,
            LowerCenter = 2,
            LowerRight = 3,
            MiddleLeft = 4,
            MiddleCenter = 5,
            MiddleRight = 6,
            UpperLeft = 7,
            UpperCenter = 8,
            UpperRight = 9,
        };

    private:
        void startNextCamera(QString deviceName = QString());
        void showMenu();
        void setWindowSize(int width, int height, Alignment alignment);
        void changeWindowSize(int difference);
        int _cameraWidth = 0;
        int _cameraHeight = 0;
        int _lastLeft = 0;
        int _lastTop = 0;
        int _lastWidth = 0;
        int _lastHeight = 0;
        Alignment _lastAlignment;
        QPoint _lastClickLocation;

    private:
        Ui::MainWindow* ui;
        QString _cameraName;
        QScopedPointer<QCamera> _camera;
        QTimer* _statusUpdateTimer = nullptr;

    private slots:
        void onMenuCameraSelected();
        void onMenuAbout();
        void onStatusUpdate();

};
