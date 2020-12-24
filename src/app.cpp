#include <QtWidgets>
#include "medo/appsetupmutex.h"
#include "medo/singleinstance.h"
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QCoreApplication::setApplicationName(APP_PRODUCT);
    QCoreApplication::setOrganizationName(APP_COMPANY);
    QCoreApplication::setApplicationVersion(APP_VERSION);

    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    AppSetupMutex appMutex("Medo64_BareCam");
    QApplication app(argc, argv);

    if (!SingleInstance::attach()) {
        return static_cast<int>(0x80004004); //exit immediately if another instance is running
    }

    MainWindow w;
    w.show();

    return app.exec();
};
