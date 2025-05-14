#include "powminerwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDateTime>

POWMinerWidget::POWMinerWidget(Blockchain* blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain), minerThread(nullptr) {
    setupUI();
}

POWMinerWidget::~POWMinerWidget() {
    if(minerThread && minerThread->isRunning()) {
        minerThread->stop();
        minerThread->wait();
    }
}

void POWMinerWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Configuração do minerador
    QFormLayout *form = new QFormLayout();
    minerAddressInput = new QLineEdit();
    form->addRow("Endereço do Minerador:", minerAddressInput);
    
    // Controles
    toggleButton = new QPushButton("Iniciar Mineração");
    hashProgress = new QProgressBar();
    hashRateLabel = new QLabel("Hashrate: 0 H/s");
    lastBlockLabel = new QLabel("Último bloco: Nenhum");
    statusLabel = new QLabel("Status: Inativo");
    
    // Área de log
    miningLog = new QTextEdit();
    miningLog->setReadOnly(true);
    
    layout->addLayout(form);
    layout->addWidget(toggleButton);
    layout->addWidget(hashProgress);
    layout->addWidget(hashRateLabel);
    layout->addWidget(lastBlockLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(new QLabel("Log de Mineração:"));
    layout->addWidget(miningLog);
    
    connect(toggleButton, &QPushButton::clicked, this, &POWMinerWidget::toggleMining);
}

void POWMinerWidget::toggleMining() {
    if(isMining) {
        // Parar mineração
        minerThread->stop();
        minerThread->wait();
        delete minerThread;
        minerThread = nullptr;
        
        toggleButton->setText("Iniciar Mineração");
        statusLabel->setText("Status: Mineração parada");
        isMining = false;
    } else {
        // Iniciar mineração
        std::string address = minerAddressInput->text().toStdString();
        if(address.empty()) {
            QMessageBox::warning(this, "Erro", "Digite um endereço válido");
            return;
        }
        
        minerThread = new POWMiner(blockchain, address);
        connect(minerThread, &POWMiner::blockMined, this, &POWMinerWidget::updateBlockInfo);
        connect(minerThread, &POWMiner::hashRateCalculated, this, &POWMinerWidget::updateHashRate);
        connect(minerThread, &POWMiner::miningStatusChanged, this, &POWMinerWidget::updateMiningStatus);
        
        minerThread->start();
        toggleButton->setText("Parar Mineração");
        statusLabel->setText("Status: Mineração em progresso");
        isMining = true;
    }
}

void POWMinerWidget::updateBlockInfo(const Block& block) {
    lastBlockLabel->setText(QString("Último bloco: %1").arg(block.height));
    miningLog->append(QString("[%1] Bloco %2 minerado com sucesso! Hash: %3")
                     .arg(QDateTime::currentDateTime().toString())
                     .arg(block.height)
                     .arg(QString::fromStdString(block.hash)));
}

void POWMinerWidget::updateHashRate(double hashesPerSec) {
    QString unit;
    double rate = hashesPerSec;
    
    if(hashesPerSec > 1e12) {
        rate = hashesPerSec / 1e12;
        unit = "TH/s";
    } else if(hashesPerSec > 1e9) {
        rate = hashesPerSec / 1e9;
        unit = "GH/s";
    } else if(hashesPerSec > 1e6) {
        rate = hashesPerSec / 1e6;
        unit = "MH/s";
    } else if(hashesPerSec > 1e3) {
        rate = hashesPerSec / 1e3;
        unit = "KH/s";
    } else {
        unit = "H/s";
    }
    
    hashRateLabel->setText(QString("Hashrate: %1 %2").arg(rate, 0, 'f', 2).arg(
