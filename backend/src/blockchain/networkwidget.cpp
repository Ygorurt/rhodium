#include "networkwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>

NetworkWidget::NetworkWidget(Blockchain &blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    peerCountLabel = new QLabel("Nós conectados: 0", this);
    peerListWidget = new QListWidget(this);

    layout->addWidget(peerCountLabel);
    layout->addWidget(peerListWidget);

    updatePeerList();
}

void NetworkWidget::updatePeerList()
{
    peerListWidget->clear();
    auto peers = blockchain.getPeerList();
    peerCountLabel->setText(QString("Nós conectados: %1").arg(peers.size()));
    
    for (const auto& peer : peers) {
        peerListWidget->addItem(QString::fromStdString(peer));
    }
}