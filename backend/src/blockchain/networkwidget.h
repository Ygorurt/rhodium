#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>
#include "blockchain.h"

class QLabel;
class QListWidget;

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(Blockchain &blockchain, QWidget *parent = nullptr);
    void updatePeerList();

private:
    Blockchain &blockchain;
    QLabel *peerCountLabel;
    QListWidget *peerListWidget;
};

#endif // NETWORKWIDGET_H