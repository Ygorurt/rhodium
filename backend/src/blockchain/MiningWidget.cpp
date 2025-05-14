#include "miningwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

MiningWidget::MiningWidget(Blockchain &blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain), miningActive(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Formulário
    QFormLayout *formLayout = new QFormLayout();
    statusLabel = new QLabel("Inativo");
    hashrateLabel = new QLabel("0 H/s");
    addressLineEdit = new QLineEdit();
    addressLineEdit->setPlaceholderText("Endereço para recompensas");
    
    formLayout->addRow("Status:", statusLabel);
    formLayout->addRow("Hashrate:", hashrateLabel);
    formLayout->addRow("Endereço:", addressLineEdit);

    // Controles
    QHBoxLayout *controlLayout = new QHBoxLayout();
    toggleButton = new QPushButton("Iniciar Mineração");
    posCheckBox = new QCheckBox("Usar Proof of Stake");
    
    controlLayout->addWidget(toggleButton);
    controlLayout->addWidget(posCheckBox);

    // Conecta sinais
    connect(toggleButton, &QPushButton::clicked, this, &MiningWidget::toggleMining);
    connect(addressLineEdit, &QLineEdit::editingFinished, this, &MiningWidget::setMiningAddress);

    // Adiciona ao layout
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(controlLayout);

    updateStatus();
}

void MiningWidget::toggleMining()
{
    if (miningActive) {
        // Para mineração
        miningActive = false;
        toggleButton->setText("Iniciar Mineração");
        statusLabel->setText("Inativo");
    } else {
        // Inicia mineração
        QString address = addressLineEdit->text();
        if (address.isEmpty()) {
            QMessageBox::warning(this, "Erro", "Digite um endereço para receber recompensas");
            return;
        }
        
        miningActive = true;
        toggleButton->setText("Parar Mineração");
        statusLabel->setText("Mineração ativa");
        
        // Na implementação real: blockchain.startMining(address, posCheckBox->isChecked())
    }
}

void MiningWidget::setMiningAddress()
{
    // Valida o endereço
    QString address = addressLineEdit->text();
    if (!address.isEmpty()) {
        // Na implementação real: validar formato do endereço
    }
}

void MiningWidget::updateStatus()
{
    // Atualiza status da mineração
    if (miningActive) {
        statusLabel->setText("Mineração ativa");
        // Simula hashrate
        hashrateLabel->setText(QString::number(rand() % 1000) + " H/s");
    } else {
        statusLabel->setText("Inativo");
        hashrateLabel->setText("0 H/s");
    }
}