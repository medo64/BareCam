#pragma once

#include <QCamera>
#include <QScopedPointer>


#include <QMainWindow>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        MainWindow();

    private:
        Ui::MainWindow* ui;
        QScopedPointer<QCamera> _camera;

};
