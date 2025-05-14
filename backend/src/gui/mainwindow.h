#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include "../blockchain/blockchain.h"
#include "../blockchain/walletwidget.h"
#include "../network/networkwidget.h"
#include "../blockchain/miningwidget.h"
#include "../blockchain/transactionwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateStatus();
    void showNotification(const QString &title, const QString &message);
    void onBlockMined(const QString &blockHash);
    void onTransactionProcessed(const QString &txId);
    void onPeerConnected(const QString &peer);
    void onPeerDisconnected(const QString &peer);

private:
    void setupUi();
    void setupMenu();
    void setupTrayIcon();
    void connectSignals();

    Blockchain blockchain;
    QTabWidget *tabWidget;
    WalletWidget *walletWidget;
    NetworkWidget *networkWidget;
    MiningWidget *miningWidget;
    TransactionWidget *transactionWidget;
    QStatusBar *statusBar;
    QSystemTrayIcon *trayIcon;
};

#endif // MAINWINDOW_H
