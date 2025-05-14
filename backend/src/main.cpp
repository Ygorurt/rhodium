#include "gui/mainwindow.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    // Configura o caminho dos plugins
    QCoreApplication::addLibraryPath("./platforms");
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", "platforms");
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
