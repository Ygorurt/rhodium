#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QList>
#include <QNetworkInterface>
#include <QCryptographicHash>
#include "peer.h"

/**
 * @class Discovery
 * @brief Responsável pela descoberta de nós pares na rede P2P
 * 
 * Esta classe implementa o protocolo de descoberta de peers usando UDP broadcast
 * e mantém uma lista atualizada de nós conhecidos na rede.
 */
class Discovery : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construtor da classe Discovery
     * @param port Porta UDP para escutar as mensagens de descoberta
     * @param parent Objeto pai (opcional)
     */
    explicit Discovery(quint16 port, QObject *parent = nullptr);

    /**
     * @brief Destrutor
     */
    ~Discovery();

    /**
     * @brief Inicia o serviço de descoberta
     */
    void start();

    /**
     * @brief Para o serviço de descoberta
     */
    void stop();

    /**
     * @brief Realiza uma descoberta ativa de peers
     */
    void discover();

    /**
     * @brief Retorna a lista de peers conhecidos
     * @return Lista de PeerInfo
     */
    QList<PeerInfo> knownPeers() const;

    /**
     * @brief Adiciona um peer manualmente à lista de peers conhecidos
     * @param peer Informações do peer a ser adicionado
     */
    void addKnownPeer(const PeerInfo &peer);

signals:
    /**
     * @brief Sinal emitido quando um novo peer é descoberto
     * @param peerInfo Informações do peer descoberto
     */
    void newPeerDiscovered(const PeerInfo &peerInfo);

    /**
     * @brief Sinal emitido quando ocorre um erro na descoberta
     * @param errorMessage Mensagem de erro descritiva
     */
    void discoveryError(const QString &errorMessage);

private slots:
    /**
     * @brief Slot chamado quando dados estão disponíveis para leitura no socket UDP
     */
    void onReadyRead();

    /**
     * @brief Slot chamado periodicamente para realizar descoberta de peers
     */
    void onDiscoveryTimeout();

private:
    QUdpSocket *m_udpSocket;       ///< Socket UDP para comunicação
    QTimer *m_discoveryTimer;      ///< Timer para descoberta periódica
    quint16 m_port;                ///< Porta de escuta
    QList<PeerInfo> m_knownPeers;  ///< Lista de peers conhecidos
    QString m_nodeId;              ///< Identificador único deste nó

    /**
     * @brief Cria a mensagem de descoberta
     * @return QByteArray com a mensagem serializada
     */
    QByteArray createDiscoveryMessage() const;

    /**
     * @brief Processa uma resposta à mensagem de descoberta
     * @param data Dados recebidos
     * @param sender Endereço do remetente
     */
    void processDiscoveryResponse(const QByteArray &data, const QHostAddress &sender);

    /**
     * @brief Gera um ID único para este nó
     * @return QString com o ID do nó
     */
    QString generateNodeId() const;

    /**
     * @brief Verifica se um peer já está na lista de conhecidos
     * @param peer Peer a ser verificado
     * @return true se o peer já é conhecido, false caso contrário
     */
    bool isPeerKnown(const PeerInfo &peer) const;
};

#endif // DISCOVERY_H