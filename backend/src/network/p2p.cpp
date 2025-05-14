#include "p2p.h"
#include <QNetworkInterface>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDataStream>
#include "crypto/sha256.h"

P2PNetwork::P2PNetwork(BlockchainCore* blockchain, QObject* parent)
    : QObject(parent), blockchain(blockchain) {
    
    // Gerar identidade do nó
    std::string privKey, pubKey;
    ECDSA::generateKeyPair(privKey, pubKey);
    nodePrivateKey = QString::fromStdString(privKey);
    nodePublicKey = QString::fromStdString(pubKey);
    nodeId = QString::fromStdString(ECDSA::publicKeyToAddress(pubKey));
    
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
    
    broadcastTimer = new QTimer(this);
    broadcastTimer->setInterval(BROADCAST_INTERVAL);
    
    discoveryTimer = new QTimer(this);
    discoveryTimer->setInterval(DISCOVERY_INTERVAL);
    
    healthCheckTimer = new QTimer(this);
    healthCheckTimer->setInterval(HEALTH_CHECK_INTERVAL);
    
    connect(tcpServer, &QTcpServer::newConnection, this, &P2PNetwork::onNewConnection);
    connect(broadcastTimer, &QTimer::timeout, this, &P2PNetwork::onBroadcastTimer);
    connect(discoveryTimer, &QTimer::timeout, this, &P2PNetwork::discoverPeers);
    connect(healthCheckTimer, &QTimer::timeout, this, &P2PNetwork::checkPeerHealth);
}

P2PNetwork::~P2PNetwork() {
    stop();
}

void P2PNetwork::start(int port) {
    serverPort = port;
    
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        emit networkActivity(tr("Failed to start server: %1").arg(tcpServer->errorString()));
        return;
    }
    
    if (!udpSocket->bind(port, QUdpSocket::ShareAddress)) {
        emit networkActivity(tr("Failed to bind UDP socket: %1").arg(udpSocket->errorString()));
        return;
    }
    
    broadcastTimer->start();
    discoveryTimer->start();
    healthCheckTimer->start();
    
    emit networkActivity(tr("P2P network started on port %1. Node ID: %2").arg(port).arg(nodeId));
}

void P2PNetwork::stop() {
    broadcastTimer->stop();
    discoveryTimer->stop();
    healthCheckTimer->stop();
    
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        it->socket->disconnectFromHost();
        it->socket->deleteLater();
    }
    peers.clear();
    
    tcpServer->close();
    udpSocket->close();
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
            PeerInfo info;
            info.socket = socket;
            info.lastActivity = QDateTime::currentMSecsSinceEpoch();
            info.version = "1.0";
            peers.insert(socket, info);
            
            connect(socket, &QTcpSocket::readyRead, this, &P2PNetwork::onReadyRead);
            connect(socket, &QTcpSocket::disconnected, this, &P2PNetwork::onPeerDisconnected);
            
            // Iniciar handshake
            performHandshake(socket);
            
            emit networkActivity(tr("Connected to seed node %1:%2").arg(host).arg(port));
            emit peerCountChanged(peers.size());
        } else {
            emit networkActivity(tr("Failed to connect to seed node %1:%2").arg(host).arg(port));
            socket->deleteLater();
        }
    }
}

void P2PNetwork::onNewConnection() {
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket* socket = tcpServer->nextPendingConnection();
        QString peerAddress = QString("%1:%2").arg(socket->peerAddress().toString())
                                             .arg(socket->peerPort());
        
        // Verificar se o peer está banido
        if (bannedPeers.contains(peerAddress) && 
            bannedPeers[peerAddress] > QDateTime::currentMSecsSinceEpoch()) {
            socket->disconnectFromHost();
            socket->deleteLater();
            continue;
        }
        
        PeerInfo info;
        info.socket = socket;
        info.lastActivity = QDateTime::currentMSecsSinceEpoch();
        peers.insert(socket, info);
        
        connect(socket, &QTcpSocket::readyRead, this, &P2PNetwork::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &P2PNetwork::onPeerDisconnected);
        
        // Configurar timeout para handshake
        QTimer::singleShot(HANDSHAKE_TIMEOUT, [this, socket]() {
            if (peers.contains(socket) && peers[socket].publicKey.isEmpty()) {
                emit networkActivity(tr("Handshake timeout with peer %1").arg(
                    socket->peerAddress().toString()));
                socket->disconnectFromHost();
            }
        });
        
        emit networkActivity(tr("New connection from %1").arg(peerAddress));
        emit peerCountChanged(peers.size());
    }
}

void P2PNetwork::onPeerDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket || !peers.contains(socket)) return;
    
    QString peerAddress = QString("%1:%2").arg(socket->peerAddress().toString())
                                         .arg(socket->peerPort());
    
    emit networkActivity(tr("Peer disconnected: %1").arg(peerAddress));
    peers.remove(socket);
    socket->deleteLater();
    
    emit peerCountChanged(peers.size());
}

void P2PNetwork::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket || !peers.contains(socket)) return;
    
    QByteArray data = socket->readAll();
    peers[socket].lastActivity = QDateTime::currentMSecsSinceEpoch();
    
    if (peers[socket].publicKey.isEmpty()) {
        // Se ainda não completou handshake, processa como handshake
        processHandshake(data, socket);
    } else {
        // Mensagem normal
        handleMessage(data, socket);
    }
}

void P2PNetwork::handleMessage(const QByteArray& message, QTcpSocket* sender) {
    if (message.size() < MAGIC_NUMBER.size() + 1 || !message.startsWith(MAGIC_NUMBER)) {
        emit networkActivity(tr("Invalid message received from %1").arg(
            sender->peerAddress().toString()));
        return;
    }
    
    char command = message[MAGIC_NUMBER.size()];
    QByteArray payload = message.mid(MAGIC_NUMBER.size() + 1);
    
    switch (command) {
        case MSG_TRANSACTION: {
            if (payload.size() > MESSAGE_MAX_SIZE) {
                emit networkActivity(tr("Oversized transaction from %1").arg(
                    sender->peerAddress().toString()));
                banPeer(sender->peerAddress().toString(), 5); // Ban por 5 minutos
                return;
            }
            
            // Verificar assinatura da mensagem
            QByteArray signature = payload.left(ECDSA_SIGNATURE_SIZE);
            QByteArray txData = payload.mid(ECDSA_SIGNATURE_SIZE);
            
            if (!verifyNetworkMessage(txData, signature)) {
                emit networkActivity(tr("Invalid transaction signature from %1").arg(
                    sender->peerAddress().toString()));
                return;
            }
            
            emit newTransactionReceived(txData);
            sendToAll(serializeMessage(MSG_TRANSACTION, signature + txData), sender);
            break;
        }
            
        case MSG_BLOCK: {
            if (payload.size() > MESSAGE_MAX_SIZE) {
                emit networkActivity(tr("Oversized block from %1").arg(
                    sender->peerAddress().toString()));
                banPeer(sender->peerAddress().toString(), 10); // Ban por 10 minutos
                return;
            }
            
            QByteArray signature = payload.left(ECDSA_SIGNATURE_SIZE);
            QByteArray blockData = payload.mid(ECDSA_SIGNATURE_SIZE);
            
            if (!verifyNetworkMessage(blockData, signature)) {
                emit networkActivity(tr("Invalid block signature from %1").arg(
                    sender->peerAddress().toString()));
                return;
            }
            
            emit newBlockReceived(blockData);
            sendToAll(serializeMessage(MSG_BLOCK, signature + blockData), sender);
            break;
        }
            
        case MSG_PEER_LIST: {
            // Processar lista de peers
            QDataStream stream(payload);
            QStringList peerList;
            stream >> peerList;
            
            for (const QString& peer : peerList) {
                if (!peer.contains(":")) continue;
                
                QStringList parts = peer.split(":");
                QString host = parts[0];
                int port = parts[1].toInt();
                
                bool alreadyConnected = false;
                for (const auto& peerInfo : peers) {
                    if (peerInfo.socket->peerAddress().toString() == host && 
                        peerInfo.socket->peerPort() == port) {
                        alreadyConnected = true;
                        break;
                    }
                }
                
                if (!alreadyConnected) {
                    connectToSeedNodes({peer});
                }
            }
            break;
        }
            
        default:
            emit networkActivity(tr("Unknown message type %1 from %2").arg(command).arg(
                sender->peerAddress().toString()));
    }
}

void P2PNetwork::broadcastTransaction(const QByteArray& txData) {
    if (txData.size() > MESSAGE_MAX_SIZE) {
        emit networkActivity(tr("Transaction too large to broadcast"));
        return;
    }
    
    QByteArray signature = signMessage(txData);
    QByteArray message = serializeMessage(MSG_TRANSACTION, signature + txData);
    
    QString txHash = QString::fromStdString(sha256(txData.toStdString()));
    pendingTransactions.insert(txHash, message);
    
    sendToAll(message);
}

void P2PNetwork::broadcastBlock(const QByteArray& blockData) {
    if (blockData.size() > MESSAGE_MAX_SIZE) {
        emit networkActivity(tr("Block too large to broadcast"));
        return;
    }
    
    QByteArray signature = signMessage(blockData);
    QByteArray message = serializeMessage(MSG_BLOCK, signature + blockData);
    
    QString blockHash = QString::fromStdString(sha256(blockData.toStdString()));
    pendingBlocks.insert(blockHash, message);
    
    sendToAll(message);
}

void P2PNetwork::onBroadcastTimer() {
    // Reenvia transações pendentes periodicamente
    for (const QByteArray& txMessage : pendingTransactions) {
        sendToAll(txMessage);
    }
    
    // Reenvia blocos pendentes periodicamente
    for (const QByteArray& blockMessage : pendingBlocks) {
        sendToAll(blockMessage);
    }
}

void P2PNetwork::discoverPeers() {
    QByteArray discoverMsg = serializeMessage(MSG_DISCOVERY, QByteArray::number(serverPort));
    udpSocket->writeDatagram(discoverMsg, QHostAddress::Broadcast, serverPort);
    
    // Processa respostas
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        
        if (datagram.startsWith(MAGIC_NUMBER + QByteArray(1, MSG_DISCOVERY))) {
            QString peerAddress = sender.toString() + ":" + QString::number(senderPort);
            
            bool alreadyConnected = false;
            for (const auto& peerInfo : peers) {
                if (peerInfo.socket->peerAddress() == sender && 
                    peerInfo.socket->peerPort() == senderPort) {
                    alreadyConnected = true;
                    break;
                }
            }
            
            if (!alreadyConnected && !bannedPeers.contains(peerAddress)) {
                connectToSeedNodes({peerAddress});
            }
        }
    }
}

void P2PNetwork::checkPeerHealth() {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QList<QTcpSocket*> toRemove;
    
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        if (now - it->lastActivity > HEALTH_CHECK_INTERVAL * 2) {
            emit networkActivity(tr("Peer %1 inactive, disconnecting").arg(
                it->socket->peerAddress().toString()));
            toRemove.append(it->socket);
        }
    }
    
    for (QTcpSocket* socket : toRemove) {
        socket->disconnectFromHost();
    }
}

QByteArray P2PNetwork::serializeMessage(MessageType type, const QByteArray& payload) {
    return MAGIC_NUMBER + QByteArray(1, static_cast<char>(type)) + payload;
}

void P2PNetwork::sendToAll(const QByteArray& message, QTcpSocket* exclude) {
    for (auto it = peers.begin(); it != peers.end(); ++it) {
        if (it->socket != exclude && it->socket->state() == QAbstractSocket::ConnectedState) {
            it->socket->write(message);
        }
    }
}

void P2PNetwork::sendToPeer(QTcpSocket* peer, const QByteArray& message) {
    if (!peer || !peers.contains(peer) || peer->state() != QAbstractSocket::ConnectedState) {
        return;
    }
    
    peer->write(message);
    peers[peer].lastActivity = QDateTime::currentMSecsSinceEpoch();
}

QByteArray P2PNetwork::signMessage(const QByteArray& message) {
    std::string signature = ECDSA::signMessage(
        nodePrivateKey.toStdString(),
        message.toStdString()
    );
    return QByteArray::fromStdString(signature);
}

bool P2PNetwork::verifyNetworkMessage(const QByteArray& message, const QByteArray& signature) {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket || !peers.contains(socket)) return false;
    
    std::string pubKey = peers[socket].publicKey.toStdString();
    std::string msg = message.toStdString();
    std::string sig = signature.toStdString();
    
    return ECDSA::verifySignature(pubKey, msg, sig);
}

void P2PNetwork::performHandshake(QTcpSocket* socket) {
    QByteArray handshakeData;
    QDataStream stream(&handshakeData, QIODevice::WriteOnly);
    stream << PROTOCOL_VERSION << nodePublicKey;
    
    QByteArray signature = signMessage(handshakeData);
    QByteArray message = serializeMessage(MSG_HANDSHAKE, signature + handshakeData);
    
    socket->write(message);
}

void P2PNetwork::processHandshake(const QByteArray& data, QTcpSocket* socket) {
    if (!data.startsWith(MAGIC_NUMBER + QByteArray(1, MSG_HANDSHAKE))) {
        emit networkActivity(tr("Invalid handshake from %1").arg(
            socket->peerAddress().toString()));
        socket->disconnectFromHost();
        return;
    }
    
    QByteArray payload = data.mid(MAGIC_NUMBER.size() + 1);
    if (payload.size() < ECDSA_SIGNATURE_SIZE) {
        emit networkActivity(tr("Invalid handshake size from %1").arg(
            socket->peerAddress().toString()));
        socket->disconnectFromHost();
        return;
    }
    
    QByteArray signature = payload.left(ECDSA_SIGNATURE_SIZE);
    QByteArray handshakeData = payload.mid(ECDSA_SIGNATURE_SIZE);
    
    QDataStream stream(handshakeData);
    int version;
    QString peerPublicKey;
    stream >> version >> peerPublicKey;
    
    if (version != PROTOCOL_VERSION) {
        emit networkActivity(tr("Incompatible protocol version %1 from %2").arg(version).arg(
            socket->peerAddress().toString()));
        socket->disconnectFromHost();
        return;
    }
    
    if (!ECDSA::verifySignature(peerPublicKey.toStdString(), 
                               handshakeData.toStdString(), 
                               signature.toStdString())) {
        emit networkActivity(tr("Invalid handshake signature from %1").arg(
            socket->peerAddress().toString()));
        socket->disconnectFromHost();
        return;
    }
    
    // Handshake válido
    peers[socket].publicKey = peerPublicKey;
    peers[socket].version = QString::number(version);
    
    emit networkActivity(tr("Handshake completed with %1").arg(
        socket->peerAddress().toString()));
}

void P2PNetwork::banPeer(const QString& address, int minutes) {
    bannedPeers[address] = QDateTime::currentMSecsSinceEpoch() + (minutes * 60 * 1000);
    emit networkActivity(tr("Banned peer %1 for %2 minutes").arg(address).arg(minutes));
}

QList<QString> P2PNetwork::getPeerList() const {
    QList<QString> list;
    for (const auto& peerInfo : peers) {
        list.append(QString("%1:%2").arg(peerInfo.socket->peerAddress().toString())
                                   .arg(peerInfo.socket->peerPort()));
    }
    return list;
}

int P2PNetwork::getPeerCount() const {
    return peers.size();
}

QString P2PNetwork::getNodeId() const {
    return nodeId;
}

QByteArray P2PNetwork::getNodePublicKey() const {
    return nodePublicKey.toUtf8();
}
