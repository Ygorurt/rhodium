#include "p2p.h"
#include <QNetworkInterface>
#include <QDateTime>
#include <QCryptographicHash>
#include "crypto/sha256.h"

const QByteArray MAGIC_NUMBER = "RHODIUM";
const int PROTOCOL_VERSION = 1;
const int BROADCAST_INTERVAL = 5000;
const int DISCOVERY_INTERVAL = 30000;

P2PNetwork::P2PNetwork(BlockchainCore* blockchain, QObject* parent) 
    : QObject(parent), blockchain(blockchain) {
    
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
    
    broadcastTimer = new QTimer(this);
    broadcastTimer->setInterval(BROADCAST_INTERVAL);
    
    discoveryTimer = new QTimer(this);
    discoveryTimer->setInterval(DISCOVERY_INTERVAL);
    
    connect(tcpServer, &QTcpServer::newConnection, this, &P2PNetwork::onNewConnection);
    connect(broadcastTimer, &QTimer::timeout, this, &P2PNetwork::onBroadcastTimer);
    connect(discoveryTimer, &QTimer::timeout, this, &P2PNetwork::discoverPeers);
}

void P2PNetwork::start(int port) {
    serverPort = port;
    
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qWarning() << "Failed to start P2P server:" << tcpServer->errorString();
        return;
    }
    
    if (!udpSocket->bind(port, QUdpSocket::ShareAddress)) {
        qWarning() << "Failed to bind UDP socket:" << udpSocket->errorString();
        return;
    }
    
    broadcastTimer->start();
    discoveryTimer->start();
    
    qInfo() << "P2P network started on port" << port;
}

void P2PNetwork::connectToSeedNodes(const QStringList& nodes) {
    for (const QString& node : nodes) {
        QStringList parts = node.split(":");
        if (parts.size() != 2) continue;
        
        QString host = parts[0];
        int port = parts[1].toInt();
        
        QTcpSocket* socket = new QTcpSocket(this);
        socket->connectToHost(host, port);
        
        if (socket->waitForConnected(3000)) {
            peers.insert(socket);
            connect(socket, &QTcpSocket::readyRead, this, &P2PNetwork::onReadyRead);
            connect(socket, &QTcpSocket::disconnected, this, &P2PNetwork::onPeerDisconnected);
            qInfo() << "Connected to seed node:" << node;
        } else {
            qWarning() << "Failed to connect to seed node:" << node;
            socket->deleteLater();
        }
    }
}

void P2PNetwork::onNewConnection() {
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket* socket = tcpServer->nextPendingConnection();
        peers.insert(socket);
        
        connect(socket, &QTcpSocket::readyRead, this, &P2PNetwork::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &P2PNetwork::onPeerDisconnected);
        
        qInfo() << "New peer connected:" << socket->peerAddress().toString();
        emit peerCountChanged(peers.size());
    }
}

void P2PNetwork::onPeerDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    qInfo() << "Peer disconnected:" << socket->peerAddress().toString();
    peers.remove(socket);
    socket->deleteLater();
    
    emit peerCountChanged(peers.size());
}

void P2PNetwork::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    QByteArray data = socket->readAll();
    handleMessage(data, socket);
}

void P2PNetwork::handleMessage(const QByteArray& message, QTcpSocket* sender) {
    if (message.size() < 8 || !message.startsWith(MAGIC_NUMBER)) {
        qWarning() << "Invalid message received";
        return;
    }
    
    QByteArray payload = message.mid(8);
    char command = message[7];
    
    switch (command) {
        case 'T': // Transaction
            emit newTransactionReceived(payload);
            sendToAll(message, sender); // Propaga para outros nós
            break;
            
        case 'B': // Block
            emit newBlockReceived(payload);
            sendToAll(message, sender); // Propaga para outros nós
            break;
            
        case 'P': // Peer list
            // Processa lista de peers recebida
            break;
            
        default:
            qWarning() << "Unknown message type:" << command;
    }
}

void P2PNetwork::sendToAll(const QByteArray& message, QTcpSocket* exclude) {
    for (QTcpSocket* peer : peers) {
        if (peer != exclude && peer->state() == QAbstractSocket::ConnectedState) {
            peer->write(message);
        }
    }
}

void P2PNetwork::broadcastTransaction(const QByteArray& txData) {
    QByteArray message = MAGIC_NUMBER + 'T' + txData;
    pendingTransactions.insert(QCryptographicHash::hash(txData, QCryptographicHash::Sha256), txData);
    sendToAll(message);
}

void P2PNetwork::broadcastBlock(const QByteArray& blockData) {
    QByteArray message = MAGIC_NUMBER + 'B' + blockData;
    sendToAll(message);
}

void P2PNetwork::onBroadcastTimer() {
    // Envia transações pendentes periodicamente
    for (const QByteArray& tx : pendingTransactions) {
        broadcastTransaction(tx);
    }
}

void P2PNetwork::discoverPeers() {
    // Mensagem de descoberta via UDP
    QByteArray discoverMsg = MAGIC_NUMBER + 'D' + QByteArray::number(serverPort);
    
    // Envia para broadcast
    udpSocket->writeDatagram(discoverMsg, QHostAddress::Broadcast, serverPort);
    
    // Processa respostas
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        
        if (datagram.startsWith(MAGIC_NUMBER + 'D')) {
            // Conecta ao novo peer descoberto
            QTcpSocket* socket = new QTcpSocket(this);
            socket->connectToHost(sender.toString(), senderPort);
            
            if (socket->waitForConnected(3000)) {
                peers.insert(socket);
                connect(socket, &QTcpSocket::readyRead, this, &P2PNetwork::onReadyRead);
                connect(socket, &QTcpSocket::disconnected, this, &P2PNetwork::onPeerDisconnected);
                qInfo() << "Discovered new peer:" << sender.toString();
            } else {
                socket->deleteLater();
            }
        }
    }
}

QList<QString> P2PNetwork::getPeerList() const {
    QList<QString> list;
    for (QTcpSocket* peer : peers) {
        list.append(peer->peerAddress().toString() + ":" + QString::number(peer->peerPort()));
    }
    return list;
}

int P2PNetwork::getPeerCount() const {
    return peers.size();
}

P2PNetwork::~P2PNetwork() {
    broadcastTimer->stop();
    discoveryTimer->stop();
    
    for (QTcpSocket* peer : peers) {
        peer->disconnectFromHost();
        peer->deleteLater();
    }
    
    tcpServer->close();
    udpSocket->close();
}