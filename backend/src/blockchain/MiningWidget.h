#ifndef MININGWIDGET_H
#define MININGWIDGET_H

#include <QWidget>
#include "blockchain.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;

class MiningWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MiningWidget(Blockchain &blockchain, QWidget *parent = nullptr);
    void updateStatus();

public slots:
    void toggleMining();
    void setMiningAddress();

private:
    Blockchain &blockchain;
    QLabel *statusLabel;
    QLabel *hashrateLabel;
    QLineEdit *addressLineEdit;
    QPushButton *toggleButton;
    QCheckBox *posCheckBox;
    bool miningActive;
};

#endif // MININGWIDGET_H