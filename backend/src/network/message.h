#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>
#include <QDataStream>

class Message {
public:
    enum Type {
        TRANSACTION,
        BLOCK,
        PEER_LIST
    };

    static const int MIN_MESSAGE_SIZE = 10;  // Defina o tamanho mínimo

    Message(Type type, const QByteArray &data);
    
    Type type() const;
    QByteArray data() const;
    QByteArray serialize() const;
    static Message deserialize(const QByteArray &data);

private:
    Type m_type;
    QByteArray m_data;
};

#endif // MESSAGE_H
