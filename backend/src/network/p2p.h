#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSet>
#include <QMap>
#include <QTimer>
#include <QElapsedTimer>
#include "blockchain/core.h"
#include "crypto/ecdsa.h"

class P2PNetwork : public QObject {
    Q_OBJECT

public:
    explicit P2PNetwork(BlockchainCore* blockchain, QObject* parent = nullptr);
    ~P2PNetwork();

    void start(int port = 8333);
    void stop();
    void connectToSeedNodes(const QStringList& nodes);
    void broadcastTransaction(const QByteArray& txData);
    void broadcastBlock(const QByteArray& blockData);

    QList<QString> getPeerList() const;
    int getPeerCount() const;
    QString getNodeId() const;

    // Métodos de verificação de assinatura
    bool verifyNetworkMessage(const QByteArray& message, const QByteArray& signature);
    QByteArray getNodePublicKey() const;

signals:
    void newTransactionReceived(const QByteArray& txData);
    void newBlockReceived(const QByteArray& blockData);
    void peerCountChanged(int count);
    void networkActivity(const QString& message);

private slots:
    void onNewConnection();
    void onPeerDisconnected();
    void onReadyRead();
    void onBroadcastTimer();
    void discoverPeers();
    void checkPeerHealth();

private:
    struct PeerInfo {
        QTcpSocket* socket;
        qint64 lastActivity;
        QString version;
        int latency;
        QString publicKey;
    };

    enum MessageType {
        MSG_TRANSACTION = 'T',
        MSG_BLOCK = 'B',
        MSG_PEER_LIST = 'P',
        MSG_DISCOVERY = 'D',
        MSG_HANDSHAKE = 'H'
    };

    void handleMessage(const QByteArray& message, QTcpSocket* sender);
    void sendToAll(const QByteArray& message, QTcpSocket* exclude = nullptr);
    void sendToPeer(QTcpSocket* peer, const QByteArray& message);
    QByteArray signMessage(const QByteArray& message);
    bool verifyPeer(const QString& peerId, const QByteArray& signature);
    void banPeer(const QString& address, int minutes = 60);
    void performHandshake(QTcpSocket* socket);
    QByteArray serializeMessage(MessageType type, const QByteArray& payload);
    void processHandshake(const QByteArray& data, QTcpSocket* socket);

    QTcpServer* tcpServer;
    QUdpSocket* udpSocket;
    QMap<QTcpSocket*, PeerInfo> peers;
    QMap<QString, QByteArray> pendingTransactions;
    QMap<QString, QByteArray> pendingBlocks;
    QMap<QString, qint64> bannedPeers;

    QTimer* broadcastTimer;
    QTimer* discoveryTimer;
    QTimer* healthCheckTimer;

    BlockchainCore* blockchain;
    int serverPort;
    QString nodeId;
    QString nodePrivateKey;
    QString nodePublicKey;

    // Constantes
    const QByteArray MAGIC_NUMBER = "RHODIUM";
    const int PROTOCOL_VERSION = 1;
    const int MESSAGE_MAX_SIZE = 10 * 1024 * 1024; // 10MB
    const int BROADCAST_INTERVAL = 5000; // 5 segundos
    const int DISCOVERY_INTERVAL = 30000; // 30 segundos
    const int HEALTH_CHECK_INTERVAL = 30000; // 30 segundos
    const int HANDSHAKE_TIMEOUT = 5000; // 5 segundos
};
