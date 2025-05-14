#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QLabel>
#include "blockchain.h"
#include "powminerwidget.h"
#include "posvalidatorwidget.h"
#include "transactionwidget.h"
#include "networkwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateStatusBar();
    void onBlockMined(const Block& newBlock);
    void onTransactionAdded(const Transaction& tx);

private:
    void setupUI();
    void setupConnections();
    
    Blockchain blockchain;
    
    // Widgets
    QTabWidget *mainTabs;
    TransactionWidget *txWidget;
    POWMinerWidget *powMiner;
    POSValidatorWidget *posValidator;
    NetworkWidget *networkWidget;
    
    // Status Bar
    QLabel *chainStatus;
    QLabel *peerStatus;
    QLabel *miningStatus;
};
