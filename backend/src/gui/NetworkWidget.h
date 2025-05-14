#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>

class NetworkWidget : public QWidget {
    Q_OBJECT

public:
    NetworkWidget(Blockchain* blockchain, QWidget *parent = nullptr);

private slots:
    void refreshPeers();
    void connectToPeer();

private:
    Blockchain* blockchain;
    
    QTreeWidget *peerTree;
    QPushButton *refreshButton;
    QPushButton *connectButton;
    QLineEdit *peerAddressInput;
    
    void setupPeerTree();
};
