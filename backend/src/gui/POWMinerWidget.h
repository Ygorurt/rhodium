#pragma once

#include <QWidget>
#include <QThread>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>

class POWMiner : public QThread {
    Q_OBJECT
public:
    POWMiner(Blockchain* chain, const std::string& address) 
        : blockchain(chain), minerAddress(address) {}
    
signals:
    void blockMined(const Block& block);
    void hashRateCalculated(double hashesPerSec);

protected:
    void run() override;

private:
    Blockchain* blockchain;
    std::string minerAddress;
};

class POWMinerWidget : public QWidget {
    Q_OBJECT

public:
    POWMinerWidget(Blockchain* blockchain, QWidget *parent = nullptr);
    ~POWMinerWidget();

private slots:
    void toggleMining();
    void updateBlockInfo(const Block& block);
    void updateHashRate(double hashesPerSec);

private:
    Blockchain* blockchain;
    POWMiner* minerThread;
    
    QLineEdit *minerAddressInput;
    QPushButton *toggleButton;
    QProgressBar *hashProgress;
    QLabel *hashRateLabel;
    QLabel *lastBlockLabel;
    QTextEdit *miningLog;
    
    bool isMining = false;
};
