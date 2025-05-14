#include "gui/mainwindow.h"
#include <QApplication>
#include <QDir>
#include "blockchain/network/p2p.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    BlockchainCore core;
    core.startP2PNetwork(8333); // Porta padrão
    
    // Conecta a nós conhecidos
    QStringList seedNodes = {
        "seed1.rhodium.org:8333",
        "seed2.rhodium.org:8333",
        "54.193.75.32:8333"
    };
    core.getP2PNetwork()->connectToSeedNodes(seedNodes);
    
    MainWindow w(&core);
    w.show();
    
    return a.exec();
}
