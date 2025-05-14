#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSet>
#include <QMap>
#include <QTimer>
#include "blockchain/core.h"

class P2PNetwork : public QObject {
    Q_OBJECT
    
public:
    explicit P2PNetwork(BlockchainCore* blockchain, QObject* parent = nullptr);
    ~P2PNetwork();
    
    void start(int port = 8333);
    void connectToSeedNodes(const QStringList& nodes);
    void broadcastTransaction(const QByteArray& txData);
    void broadcastBlock(const QByteArray& blockData);
    
    QList<QString> getPeerList() const;
    int getPeerCount() const;
    
signals:
    void newTransactionReceived(const QByteArray& txData);
    void newBlockReceived(const QByteArray& blockData);
    void peerCountChanged(int count);
    
private slots:
    void onNewConnection();
    void onPeerDisconnected();
    void onReadyRead();
    void onBroadcastTimer();
    void discoverPeers();
    
private:
    void handleMessage(const QByteArray& message, QTcpSocket* sender);
    void sendToAll(const QByteArray& message, QTcpSocket* exclude = nullptr);
    
    QTcpServer* tcpServer;
    QUdpSocket* udpSocket;
    QSet<QTcpSocket*> peers;
    QMap<QString, QByteArray> pendingTransactions;
    QTimer* broadcastTimer;
    QTimer* discoveryTimer;
    BlockchainCore* blockchain;
    int serverPort;
};