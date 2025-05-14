#include "network/peer.h"
#include <QDataStream>
#include <QTimer>
#include <QCryptographicHash>
#include "network/message.h"

Peer::Peer(QObject* parent) 
    : QTcpSocket(parent), m_info({QHostAddress(), 0, "", "", 0, 0}) {
    
    // Configura timeout de inatividade
    QTimer* inactivityTimer = new QTimer(this);
    inactivityTimer->setInterval(120000); // 2 minutos
    
    connect(this, &Peer::readyRead, this, [inactivityTimer]() {
        inactivityTimer->start(); // Reinicia timer a cada atividade
    });
    
    connect(inactivityTimer, &QTimer::timeout, this, [this]() {
        disconnectFromHost(); // Desconecta se inativo
    });
    
    connect(this, &Peer::connected, inactivityTimer, [inactivityTimer]() {
        inactivityTimer->start(); // Inicia timer quando conectado
    });
    
    connect(this, &Peer::disconnected, inactivityTimer, &QTimer::stop);
}

Peer::Peer(const PeerInfo& info, QObject* parent)
    : Peer(parent) {
    setInfo(info);
}

PeerInfo Peer::info() const {
    return m_info;
}

void Peer::setInfo(const PeerInfo& info) {
    m_info = info;
    
    // Gera ID único baseado nas informações do peer
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(info.address.toString().toUtf8());
    hash.addData(QByteArray::number(info.port));
    hash.addData(info.publicKey.toUtf8());
    
    m_info.id = hash.result().toHex().left(16); // ID de 16 caracteres
}

bool Peer::isConnected() const {
    return state() == ConnectedState;
}

void Peer::sendMessage(const Message& message) {
    if (!isConnected()) {
        qWarning() << "Tentativa de enviar mensagem para peer desconectado";
        return;
    }
    
    QByteArray data = message.rawData();
    
    // Registra métricas antes do envio
    qint64 bytesWritten = write(data);
    
    if (bytesWritten == -1) {
        qWarning() << "Falha ao enviar mensagem para" << m_info.id << ":" << errorString();
    } else if (!flush()) {
        qWarning() << "Falha ao liberar buffer para" << m_info.id;
    } else {
        m_info.lastSeen = QDateTime::currentSecsSinceEpoch();
    }
}

void Peer::onReadyRead() {
    while (bytesAvailable() > 0) {
        m_buffer.append(readAll());
        
        // Processa mensagens completas no buffer
        while (m_buffer.size() >= Message::MIN_MESSAGE_SIZE) {
            // Verifica magic number
            if (m_buffer.left(4) != "RHOD") {
                qWarning() << "Magic number inválido de" << m_info.id;
                m_buffer.clear();
                disconnectFromHost();
                return;
            }
            
            // Extrai tamanho do payload (bytes 13-16, após magic(4) + type(1) + timestamp(8))
            if (m_buffer.size() < 17) break; // Espera mais dados
            
            quint32 payloadSize;
            QDataStream stream(m_buffer.mid(13, 4));
            stream >> payloadSize;
            
            // Verifica tamanho máximo
            if (payloadSize > Message::MAX_MESSAGE_SIZE) {
                qWarning() << "Mensagem muito grande de" << m_info.id;
                m_buffer.clear();
                disconnectFromHost();
                return;
            }
            
            // Verifica se mensagem completa está disponível
            quint32 totalSize = 17 + payloadSize + 4; // Cabeçalho + payload + checksum
            if (m_buffer.size() < totalSize) break;
            
            // Extrai mensagem completa
            QByteArray messageData = m_buffer.left(totalSize);
            m_buffer.remove(0, totalSize);
            
            Message message(messageData);
            if (message.isValid()) {
                m_info.lastSeen = QDateTime::currentSecsSinceEpoch();
                emit messageReceived(message);
            } else {
                qWarning() << "Mensagem inválida de" << m_info.id;
            }
        }
    }
}

void Peer::onDisconnected() {
    qInfo() << "Peer desconectado:" << m_info.id;
    emit disconnected(this);
}

// Implementação de PeerInfo
bool PeerInfo::isValid() const {
    return !address.isNull() && port > 0 && !publicKey.isEmpty();
}

QString PeerInfo::toString() const {
    return QString("%1:%2").arg(address.toString()).arg(port);
}