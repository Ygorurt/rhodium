#include "network/p2p.h"

class BlockchainCore : public QObject {
    Q_OBJECT
public:
    // ... outros métodos
    
    void startP2PNetwork(int port = 8333) {
        p2pNetwork->start(port);
    }
    
    P2PNetwork* getP2PNetwork() const { return p2pNetwork; }
    
private:
    P2PNetwork* p2pNetwork;
};