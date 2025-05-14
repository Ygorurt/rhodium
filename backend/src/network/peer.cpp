#include "peer.h"
#include <QIODevice>

Peer::Peer(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)) {
    // Conexões de signals/slots
}

void Peer::sendMessage(const Message &message) {
    if (!socket || !isConnected()) {
        return;
    }
    
    QByteArray data = message.serialize();
    if (socket->write(data) == -1) {
        qDebug() << "Error sending message:" << socket->errorString();
    }
    socket->flush();
}

bool Peer::isConnected() const {
    return socket && socket->state() == QTcpSocket::ConnectedState;
}
