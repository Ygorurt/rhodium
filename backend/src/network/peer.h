#ifndef PEER_H
#define PEER_H

#include <QObject>
#include <QTcpSocket>
#include "message.h"  // Inclua o arquivo de cabeçalho da Message

class Peer : public QObject {
    Q_OBJECT
public:
    explicit Peer(QObject *parent = nullptr);
    
    // Corrija as declarações adicionando os tipos de retorno
    void sendMessage(const Message &message);
    bool isConnected() const;

signals:
    // Especifique o tipo completo para o parâmetro
    void messageReceived(const Message &message);
    void disconnected();

private:
    QTcpSocket *socket;
};

#endif // PEER_H
