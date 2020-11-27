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
        virtual void keyPressEvent(QKeyEvent* event);

    private:
        void startNextCamera();

    private:
        Ui::MainWindow* ui;
        QString _cameraName;
        QScopedPointer<QCamera> _camera;
        QTimer* _statusUpdateTimer = nullptr;

    private slots:
        void onStatusUpdate();

};
