#include "mainwindow.h"
#include <QVBoxLayout>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), blockchain() {
    
    // Initialize P2P network
    blockchain.initP2PNetwork();
    
    setupUI();
    setupConnections();
}

void MainWindow::setupUI() {
    // Main tabs
    mainTabs = new QTabWidget(this);
    setCentralWidget(mainTabs);
    
    // Create widgets
    txWidget = new TransactionWidget(&blockchain);
    powMiner = new POWMinerWidget(&blockchain);
    posValidator = new POSValidatorWidget(&blockchain);
    networkWidget = new NetworkWidget(&blockchain);
    
    // Add tabs
    mainTabs->addTab(txWidget, "Transações");
    mainTabs->addTab(powMiner, "Mineração PoW");
    mainTabs->addTab(posValidator, "Validação PoS");
    mainTabs->addTab(networkWidget, "Rede P2P");
    
    // Status bar
    chainStatus = new QLabel("Blockchain: Carregando...");
    peerStatus = new QLabel("Peers: 0");
    miningStatus = new QLabel("Mineração: Inativa");
    
    statusBar()->addWidget(chainStatus);
    statusBar()->addWidget(peerStatus);
    statusBar()->addWidget(miningStatus);
    
    setWindowTitle("Node de Blockchain Avançado");
    resize(800, 600);
}

void MainWindow::setupConnections() {
    connect(&blockchain, &Blockchain::blockAdded, this, &MainWindow::onBlockMined);
    connect(&blockchain, &Blockchain::transactionAdded, this, &MainWindow::onTransactionAdded);
    
    // Update status every 5 seconds
    QTimer *statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    statusTimer->start(5000);
}

void MainWindow::updateStatusBar() {
    chainStatus->setText(QString("Blocos: %1").arg(blockchain.getChainLength()));
    peerStatus->setText(QString("Peers: %1").arg(blockchain.getPeerList().size()));
}

void MainWindow::onBlockMined(const Block& newBlock) {
    statusBar()->showMessage("Novo bloco minerado: " + QString::fromStdString(newBlock.hash), 3000);
    updateStatusBar();
}

void MainWindow::onTransactionAdded(const Transaction& tx) {
    statusBar()->showMessage("Nova transação adicionada: " + QString::fromStdString(tx.txHash), 3000);
}
