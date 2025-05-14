#include "mainwindow.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Rhodium Blockchain Node");
    resize(1024, 768);

    // Inicializa blockchain
    blockchain.initP2PNetwork(8333);

    setupUi();
    setupMenu();
    setupTrayIcon();
    connectSignals();

    // Atualizador de status
    QTimer *statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
    statusTimer->start(5000);
}

void MainWindow::setupUi()
{
    // Widget principal
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Cria as abas
    walletWidget = new WalletWidget(blockchain, this);
    networkWidget = new NetworkWidget(blockchain, this);
    miningWidget = new MiningWidget(blockchain, this);
    transactionWidget = new TransactionWidget(blockchain, this);

    tabWidget->addTab(walletWidget, "Carteira");
    tabWidget->addTab(networkWidget, "Rede");
    tabWidget->addTab(miningWidget, "Mineração");
    tabWidget->addTab(transactionWidget, "Transações");

    // Barra de status
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    updateStatus();
}

void MainWindow::setupMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("Arquivo");
    fileMenu->addAction("Salvar Blockchain", [this]() {
        blockchain.saveToFile("blockchain.json");
        showNotification("Blockchain Salva", "Blockchain salva com sucesso no arquivo blockchain.json");
    });
    fileMenu->addSeparator();
    fileMenu->addAction("Sair", qApp, &QApplication::quit);

    QMenu *helpMenu = menuBar()->addMenu("Ajuda");
    helpMenu->addAction("Sobre", []() {
        QMessageBox::about(nullptr, "Sobre", "Rhodium Blockchain\nVersão 1.0");
    });
}

void MainWindow::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/rhodium.png"), this);
    
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction("Abrir", this, &QWidget::showNormal);
    trayMenu->addAction("Sair", qApp, &QApplication::quit);
    
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    
    connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            showNormal();
        }
    });
}

void MainWindow::connectSignals()
{
    // Conecta sinais da blockchain à interface
    connect(&blockchain, &Blockchain::blockMined, this, &MainWindow::onBlockMined);
    connect(&blockchain, &Blockchain::transactionAdded, this, &MainWindow::onTransactionProcessed);
    connect(&blockchain, &Blockchain::peerConnected, this, &MainWindow::onPeerConnected);
    connect(&blockchain, &Blockchain::peerDisconnected, this, &MainWindow::onPeerDisconnected);
}

void MainWindow::updateStatus()
{
    QString status = QString("Blocos: %1 | Transações Pendentes: %2 | Conexões: %3")
                    .arg(blockchain.getChainLength())
                    .arg(blockchain.getPendingTransactions())
                    .arg(blockchain.getPeerList().size());
    
    statusBar->showMessage(status);
}

void MainWindow::showNotification(const QString &title, const QString &message)
{
    if (isMinimized() || !isActiveWindow()) {
        trayIcon->showMessage(title, message);
    } else {
        statusBar->showMessage(message, 5000);
    }
}

void MainWindow::onBlockMined(const QString &blockHash)
{
    showNotification("Novo Bloco Minerado", QString("Bloco %1 adicionado à blockchain").arg(blockHash));
    walletWidget->refreshBalance();
    miningWidget->updateStatus();
}

void MainWindow::onTransactionProcessed(const QString &txId)
{
    showNotification("Nova Transação", QString("Transação %1 processada").arg(txId));
    transactionWidget->updateTransactions();
    walletWidget->refreshBalance();
}

void MainWindow::onPeerConnected(const QString &peer)
{
    showNotification("Nova Conexão", QString("Conectado ao nó %1").arg(peer));
    networkWidget->updatePeerList();
}

void MainWindow::onPeerDisconnected(const QString &peer)
{
    showNotification("Conexão Encerrada", QString("Desconectado do nó %1").arg(peer));
    networkWidget->updatePeerList();
}

MainWindow::~MainWindow()
{
    // Salva blockchain ao sair
    blockchain.saveToFile("blockchain_autosave.json");
}
