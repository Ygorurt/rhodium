#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
    
    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout, this, &MainWindow::updateChainStatus);
    updateTimer_->start(5000); // Update every 5 seconds
}

void MainWindow::onMineButtonClicked() {
    if (blockchain_.mineBlock("miner_address")) {
        log("New block mined successfully!");
    } else {
        log("No transactions to mine");
    }
    updateChainStatus();
}

void MainWindow::updateChainStatus() {
    ui->chainLengthLabel->setText(QString("Blockchain length: %1").arg(blockchain_.getChainLength()));
    ui->pendingTxsLabel->setText(QString("Pending transactions: %1").arg(blockchain_.getPendingTransactions().size()));
}