#ifndef PEER_H
#define PEER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QString>

struct PeerInfo {
    QHostAddress address;
    quint16 port;
    QString publicKey;
    QString version;
    qint64 lastSeen;
    int latency; // em ms

    bool isValid() const;
    QString id() const;
    QString toString() const;
};

class Peer : public QTcpSocket {
    Q_OBJECT

public:
    explicit Peer(QObject* parent = nullptr);
    Peer(const PeerInfo& info, QObject* parent = nullptr);

    PeerInfo info()