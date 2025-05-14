#include "walletwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

WalletWidget::WalletWidget(Blockchain &blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Formulário de informações
    QFormLayout *formLayout = new QFormLayout();
    balanceLabel = new QLabel("0.0 RHD");
    addressLineEdit = new QLineEdit();
    addressLineEdit->setPlaceholderText("Endereço da carteira");
    
    formLayout->addRow("Saldo:", balanceLabel);
    formLayout->addRow("Endereço:", addressLineEdit);

    // Botões
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    generateButton = new QPushButton("Nova Carteira");
    importButton = new QPushButton("Importar");
    detailsButton = new QPushButton("Detalhes");
    
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(importButton);
    buttonLayout->addWidget(detailsButton);

    // Área de informações
    infoTextEdit = new QTextEdit();
    infoTextEdit->setReadOnly(true);

    // Conecta sinais
    connect(generateButton, &QPushButton::clicked, this, &WalletWidget::generateNewWallet);
    connect(importButton, &QPushButton::clicked, this, &WalletWidget::importWallet);
    connect(detailsButton, &QPushButton::clicked, this, &WalletWidget::showWalletDetails);

    // Adiciona ao layout principal
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(infoTextEdit);

    // Atualiza dados iniciais
    refreshBalance();
}

void WalletWidget::generateNewWallet()
{
    // Na implementação real, você chamaria blockchain.createWallet()
    QString newAddress = "RH" + QString::number(rand() % 1000000);
    addressLineEdit->setText(newAddress);
    infoTextEdit->setText("Nova carteira gerada:\nEndereço: " + newAddress);
    refreshBalance();
}

void WalletWidget::importWallet()
{
    QString privateKey = QInputDialog::getText(this, "Importar Carteira", 
                                             "Digite a chave privada:");
    if (!privateKey.isEmpty()) {
        // Na implementação real: blockchain.importWallet(privateKey)
        infoTextEdit->setText("Carteira importada com sucesso");
        refreshBalance();
    }
}

void WalletWidget::showWalletDetails()
{
    QString address = addressLineEdit->text();
    if (!address.isEmpty()) {
        // Na implementação real: blockchain.getWalletDetails(address)
        infoTextEdit->setText("Detalhes da carteira " + address + 
                             "\nSaldo: " + balanceLabel->text() +
                             "\nTransações: 42");
    }
}

void WalletWidget::refreshBalance()
{
    QString address = addressLineEdit->text();
    if (!address.isEmpty()) {
        double balance = blockchain.getBalance(address.toStdString());
        balanceLabel->setText(QString::number(balance) + " RHD");
    }
}