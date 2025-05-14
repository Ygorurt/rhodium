#include "networkwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

NetworkWidget::NetworkWidget(Blockchain* blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain) {
    setupUI();
    setupPeerTree();
    
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &NetworkWidget::updatePeerList);
    refreshTimer->start(5000);
}

void NetworkWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Informações do nó
    nodeIdLabel = new QLabel("ID do Nó: Carregando...");
    connectionStatusLabel = new QLabel("Status: Desconectado");
    
    // Controles de rede
    peerAddressInput = new QLineEdit();
    peerAddressInput->setPlaceholderText("endereço:porta");
    
    connectButton = new QPushButton("Conectar");
    refreshButton = new QPushButton("Atualizar Lista");
    
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(peerAddressInput);
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(refreshButton);
    
    // Tabela de peers
    peerTree = new QTreeWidget();
    peerTree->setHeaderLabels({"Peer", "Endereço", "Ping", "Altura"});
    
    layout->addWidget(nodeIdLabel);
    layout->addWidget(connectionStatusLabel);
    layout->addLayout(controlLayout);
    layout->addWidget(new QLabel("Peers Conectados:"));
    layout->addWidget(peerTree);
    
    connect(connectButton, &QPushButton::clicked, this, &NetworkWidget::connectToPeer);
    connect(refreshButton, &QPushButton::clicked, this, &NetworkWidget::refreshPeers);
}

void NetworkWidget::setupPeerTree() {
    peerTree->setColumnCount(4);
    peerTree->setHeaderLabels({"Peer", "Endereço", "Ping", "Altura"});
    peerTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    peerTree->setSortingEnabled(true);
}

void NetworkWidget::refreshPeers() {
    updatePeerList();
}

void NetworkWidget::connectToPeer() {
    QString peerAddress = peerAddressInput->text();
    if(peerAddress.isEmpty()) {
        QMessageBox::warning(this, "Erro", "Digite um endereço válido");
        return;
    }
    
    // Implementar conexão P2P
    // blockchain->connectToPeer(peerAddress.toStdString());
    
    QMessageBox::information(this, "Conexão", 
        QString("Conectando ao peer: %1").arg(peerAddress));
    
    updatePeerList();
}

void NetworkWidget::updatePeerList() {
    peerTree->clear();
    
    auto peers = blockchain->getPeerList();
    connectionStatusLabel->setText(QString("Status: Conectado (%1 peers)").arg(peers.size()));
    
    for(const auto& peer : peers) {
        QTreeWidgetItem *item = new QTreeWidgetItem(peerTree);
        item->setText(0, QString::fromStdString(peer.substr(0, 8))); // ID abreviado
        item->setText(1, QString::fromStdString(peer));
        item->setText(2, "50ms"); // Ping simulado
        item->setText(3, "12345"); // Altura simulado
    }
}
