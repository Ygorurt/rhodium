#include "network/message.h"
#include <QDataStream>
#include <QDateTime>
#include <QCryptographicHash>
#include "crypto/sha256.h"
#include "crypto/ecdsa.h"

Message::Message(Type type, const QByteArray& payload)
    : m_type(type), m_payload(payload), m_timestamp(QDateTime::currentDateTime()), m_valid(true) {
    // Verifica se o tipo é válido
    if (type < TRANSACTION || type > HANDSHAKE) {
        m_valid = false;
    }
}

Message::Message(const QByteArray& rawData) 
    : m_valid(false) {
    deserialize(rawData);
}

bool Message::isValid() const {
    return m_valid;
}

Message::Type Message::type() const {
    return m_type;
}

QByteArray Message::payload() const {
    return m_payload;
}

QByteArray Message::rawData() const {
    return serialize();
}

QDateTime Message::timestamp() const {
    return m_timestamp;
}

Message Message::createHandshake(const PeerInfo& peerInfo) {
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream << peerInfo.publicKey << peerInfo.version << peerInfo.latency;
    return Message(HANDSHAKE, payload);
}

Message Message::createTransaction(const QByteArray& txData) {
    return Message(TRANSACTION, txData);
}

Message Message::createBlock(const QByteArray& blockData) {
    return Message(BLOCK, blockData);
}

Message Message::createGetPeers() {
    return Message(GET_PEERS, QByteArray());
}

Message Message::createPeers(const QList<PeerInfo>& peers) {
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream << peers;
    return Message(PEERS, payload);
}

Message Message::createPing() {
    return Message(PING, QByteArray());
}

Message Message::createPong() {
    return Message(PONG, QByteArray());
}

QByteArray Message::serialize() const {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    
    // Cabeçalho da mensagem (4 bytes magic + 1 byte tipo + 8 bytes timestamp)
    stream.writeRawData("RHOD", 4); // Magic number
    stream << static_cast<quint8>(m_type);
    stream << m_timestamp.toSecsSinceEpoch();
    
    // Tamanho e payload
    stream << static_cast<quint32>(m_payload.size());
    stream.writeRawData(m_payload.constData(), m_payload.size());
    
    // Checksum (SHA256 dos primeiros 1024 bytes do payload)
    QByteArray checksum = QCryptographicHash::hash(
        m_payload.left(1024), 
        QCryptographicHash::Sha256
    ).left(4);
    
    stream.writeRawData(checksum.constData(), checksum.size());
    
    return data;
}

void Message::deserialize(const QByteArray& data) {
    QDataStream stream(data);
    
    // Verifica magic number
    char magic[4];
    stream.readRawData(magic, 4);
    if (strncmp(magic, "RHOD", 4) != 0) {
        m_valid = false;
        return;
    }
    
    // Lê tipo e timestamp
    quint8 type;
    qint64 timestamp;
    stream >> type >> timestamp;
    
    m_type = static_cast<Type>(type);
    m_timestamp = QDateTime::fromSecsSinceEpoch(timestamp);
    
    // Lê payload
    quint32 payloadSize;
    stream >> payloadSize;
    
    if (payloadSize > MAX_MESSAGE_SIZE) {
        m_valid = false;
        return;
    }
    
    m_payload.resize(payloadSize);
    stream.readRawData(m_payload.data(), payloadSize);
    
    // Verifica checksum
    if (data.size() < stream.device()->pos() + 4) {
        m_valid = false;
        return;
    }
    
    QByteArray receivedChecksum(4, '\0');
    stream.readRawData(receivedChecksum.data(), 4);
    
    QByteArray calculatedChecksum = QCryptographicHash::hash(
        m_payload.left(1024), 
        QCryptographicHash::Sha256
    ).left(4);
    
    m_valid = (receivedChecksum == calculatedChecksum);
}

// Implementação dos operadores para PeerInfo
QDataStream& operator<<(QDataStream& out, const PeerInfo& peer) {
    out << peer.address.toString() << peer.port << peer.publicKey 
        << peer.version << peer.lastSeen << peer.latency;
    return out;
}

QDataStream& operator>>(QDataStream& in, PeerInfo& peer) {
    QString addressStr;
    in >> addressStr >> peer.port >> peer.publicKey 
       >> peer.version >> peer.lastSeen >> peer.latency;
    peer.address = QHostAddress(addressStr);
    return in;
}