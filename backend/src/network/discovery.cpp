#include "network/discovery.h"
#include <QNetworkDatagram>
#include <QDateTime>
#include <QCryptographicHash>
#include <QHostInfo>

Discovery::Discovery(quint16 port, QObject *parent) 
    : QObject(parent), m_port(port), m_nodeId(generateNodeId()) {
    
    m_udpSocket = new QUdpSocket(this);
    m_discoveryTimer = new QTimer(this);
    m_discoveryTimer->setInterval(30000); // 30 segundos
    
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &Discovery::onReadyRead);
    connect(m_discoveryTimer, &QTimer::timeout, this, &Discovery::onDiscoveryTimeout);
}

Discovery::~Discovery() {
    stop();
}

void Discovery::start() {
    if (!m_udpSocket->bind(m_port, QUdpSocket::ShareAddress)) {
        emit discoveryError(tr("Failed to bind UDP socket: %1").arg(m_udpSocket->errorString()));
        return;
    }
    
    m_discoveryTimer->start();
    discover(); // Primeira descoberta imediata
}

void Discovery::stop() {
    m_discoveryTimer->stop();
    m_udpSocket->close();
}

void Discovery::discover() {
    QByteArray message = createDiscoveryMessage();
    
    // Envia para broadcast na rede local
    if (m_udpSocket->writeDatagram(message, QHostAddress::Broadcast, m_port) == -1) {
        emit discoveryError(tr("Failed to send discovery packet: %1").arg(m_udpSocket->errorString()));
    }
}

QList<PeerInfo> Discovery::knownPeers() const {
    return m_knownPeers;
}

void Discovery::addKnownPeer(const PeerInfo &peer) {
    if (!isPeerKnown(peer)) {
        m_knownPeers.append(peer);
        emit newPeerDiscovered(peer);
    }
}

void Discovery::onReadyRead() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QByteArray data = datagram.data();
        QHostAddress sender = datagram.senderAddress();
        
        // Ignora pacotes muito pequenos
        if (data.size() < 20) continue;
        
        // Processa pacotes de descoberta
        if (data.startsWith("RHODIUM_DISCOVERY")) {
            processDiscoveryResponse(data.mid(17), sender);
        }
    }
}

void Discovery::onDiscoveryTimeout() {
    discover();
}

QByteArray Discovery::createDiscoveryMessage() const {
    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    
    stream << QString("RHODIUM_DISCOVERY").toUtf8();
    stream << m_nodeId.toUtf8();
    stream << m_port;
    
    return message;
}

void Discovery::processDiscoveryResponse(const QByteArray &data, const QHostAddress &sender) {
    QDataStream stream(data);
    QString nodeId;
    quint16 port;
    
    stream >> nodeId;
    stream >> port;
    
    // Ignora respostas do próprio nó
    if (nodeId == m_nodeId) return;
    
    PeerInfo peer;
    peer.address = sender;
    peer.port = port;
    peer.lastSeen = QDateTime::currentSecsSinceEpoch();
    peer.publicKey = nodeId; // Usando nodeId como placeholder
    
    addKnownPeer(peer);
}

QString Discovery::generateNodeId() const {
    QStringList components;
    
    // Usa endereço MAC, hostname e timestamp para gerar ID único
    components << QHostInfo::localHostName();
    components << QString::number(QDateTime::currentSecsSinceEpoch());
    
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (!(interface.flags() & QNetworkInterface::IsLoopBack)) {
            components << interface.hardwareAddress();
            break;
        }
    }
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(components.join("|").toUtf8());
    
    return hash.result().toHex().left(32); // Retorna os primeiros 32 chars do hash
}

bool Discovery::isPeerKnown(const PeerInfo &peer) const {
    QString peerId = QString("%1:%2").arg(peer.address.toString()).arg(peer.port);
    
    foreach (const PeerInfo &knownPeer, m_knownPeers) {
        QString knownPeerId = QString("%1:%2").arg(knownPeer.address.toString()).arg(knownPeer.port);
        if (peerId == knownPeerId) {
            return true;
        }
    }
    
    return false;
}