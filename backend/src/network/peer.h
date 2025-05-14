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
    QString id;  // Identificador único

    bool isValid() const;
    QString toString() const;
};

class Peer : public QTcpSocket {
    Q_OBJECT

public:
    explicit Peer(QObject* parent = nullptr);
    explicit Peer(const PeerInfo& info, QObject* parent = nullptr);

    PeerInfo info() const;
    void setInfo(const PeerInfo& info);
    bool isConnected() const;
    void sendMessage(const Message& message);

signals:
    void messageReceived(const Message& message);
    void disconnected(Peer* peer);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    PeerInfo m_info;
    QByteArray m_buffer;
};

#endif // PEER_H
