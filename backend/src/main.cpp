#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Configurar estilo visual
    app.setStyle("Fusion");
    
    // Criar e exibir janela principal
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
