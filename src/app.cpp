#include <QtWidgets>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowState(Qt::WindowFullScreen);
    w.show();

    return app.exec();
};
