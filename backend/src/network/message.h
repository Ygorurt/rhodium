#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>
#include <QDateTime>
#include "network/peer.h"

class Message {
public:
    enum Type {
        TRANSACTION = 0x01,
        BLOCK = 0x02,
        GET_PEERS = 0x03,
        PEERS = 0x04,
        PING = 0x05,
        PONG = 0x06,
        HANDSHAKE = 0x07
    };

    Message(Type type, const QByteArray& payload);
    Message(const QByteArray& rawData);

    bool isValid() const;
    Type type() const;
    QByteArray payload() const;
    QByteArray rawData() const;
    QDateTime timestamp() const;

    // Cria mensagens específicas
    static Message createHandshake(const PeerInfo& peerInfo);
    static Message createTransaction(const QByteArray& txData);
    static Message createBlock(const QByteArray& blockData);
    static Message createGetPeers();
    static Message createPeers(const QList<PeerInfo>& peers);
    static Message createPing();
    static Message createPong();

private:
    Type m_type;
    QByteArray m_payload;
    QDateTime m_timestamp;
    bool m_valid;

    QByteArray serialize() const;
    void deserialize(const QByteArray& data);
};

#endif // MESSAGE_H