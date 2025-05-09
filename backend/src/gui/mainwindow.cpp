#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , blockchain()
    , updateTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Configuração inicial dos labels
    ui->chainLengthLabel->setText(QString("Comprimento da cadeia: %1")
                                .arg(blockchain.getChainLength()));
    ui->pendingTxsLabel->setText(QString("Transações pendentes: %1")
                               .arg(blockchain.getPendingTransactions()));

    // Configuração do timer
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateChainStatus);
    updateTimer->start(5000); // Atualiza a cada 5 segundos

    // Conexão dos botões
    connect(ui->mineButton, &QPushButton::clicked, 
            this, &MainWindow::onMineButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateChainStatus()
{
    ui->chainLengthLabel->setText(QString("Comprimento da cadeia: %1")
                                .arg(blockchain.getChainLength()));
    ui->pendingTxsLabel->setText(QString("Transações pendentes: %1")
                               .arg(blockchain.getPendingTransactions()));
}

void MainWindow::onMineButtonClicked()
{
    blockchain.mineBlock("miner_address");
    updateChainStatus();
}
