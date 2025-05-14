#include "transactionwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDateTime>

TransactionWidget::TransactionWidget(Blockchain* blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain) {
    setupUI();
    loadTransactionHistory();
}

void TransactionWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Formulário de transação
    QFormLayout *form = new QFormLayout();
    
    senderInput = new QLineEdit();
    recipientInput = new QLineEdit();
    amountInput = new QDoubleSpinBox();
    amountInput->setRange(0.01, 1000000);
    amountInput->setDecimals(8);
    signatureInput = new QLineEdit();
    assetSelector = new QComboBox();
    assetSelector->addItems({"RHOD", "BTC", "ETH"});
    
    form->addRow("Remetente:", senderInput);
    form->addRow("Destinatário:", recipientInput);
    form->addRow("Quantidade:", amountInput);
    form->addRow("Ativo:", assetSelector);
    form->addRow("Assinatura:", signatureInput);
    
    // Botões
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    sendButton = new QPushButton("Enviar Transação");
    generateKeysButton = new QPushButton("Gerar Chaves");
    
    buttonLayout->addWidget(generateKeysButton);
    buttonLayout->addWidget(sendButton);
    
    // Área de informações
    balanceLabel = new QLabel("Saldo: 0 RHOD");
    txHistory = new QTextEdit();
    txHistory->setReadOnly(true);
    
    layout->addLayout(form);
    layout->addLayout(buttonLayout);
    layout->addWidget(balanceLabel);
    layout->addWidget(new QLabel("Histórico:"));
    layout->addWidget(txHistory);
    
    // Conexões
    connect(sendButton, &QPushButton::clicked, this, &TransactionWidget::sendTransaction);
    connect(generateKeysButton, &QPushButton::clicked, this, &TransactionWidget::generateKeyPair);
    
    // Atualizar saldo periodicamente
    QTimer *balanceTimer = new QTimer(this);
    connect(balanceTimer, &QTimer::timeout, this, &TransactionWidget::updateBalanceDisplay);
    balanceTimer->start(5000);
}

void TransactionWidget::sendTransaction() {
    QString from = senderInput->text();
    QString to = recipientInput->text();
    double amount = amountInput->value();
    
    if(from.isEmpty() || to.isEmpty() || amount <= 0) {
        QMessageBox::warning(this, "Erro", "Preencha todos os campos corretamente");
        return;
    }
    
    blockchain->addTransaction(from.toStdString(), to.toStdString(), amount, signatureInput->text().toStdString());
    txHistory->append(QString("[%1] Transação enviada: %2 RHOD de %3 para %4")
                     .arg(QDateTime::currentDateTime().toString())
                     .arg(amount)
                     .arg(from)
                     .arg(to));
}

void TransactionWidget::generateKeyPair() {
    // Implementação da geração de chaves (usando sua biblioteca de criptografia)
    // currentPrivateKey = ECDSA::generatePrivateKey();
    // currentPublicKey = ECDSA::getPublicKey(currentPrivateKey);
    
    senderInput->setText(QString::fromStdString(currentPublicKey));
    signatureInput->setText(QString::fromStdString(currentPrivateKey));
    
    QMessageBox::information(this, "Chaves Geradas", 
        "Chave Pública: " + QString::fromStdString(currentPublicKey) + 
        "\nChave Privada: " + QString::fromStdString(currentPrivateKey));
}

void TransactionWidget::updateBalanceDisplay() {
    if(!senderInput->text().isEmpty()) {
        double balance = blockchain->getBalance(senderInput->text().toStdString());
        balanceLabel->setText(QString("Saldo: %1 RHOD").arg(balance, 0, 'f', 8));
    }
}

void TransactionWidget::loadTransactionHistory() {
    // Implementar carregamento do histórico de transações
    txHistory->clear();
    txHistory->append("Histórico de transações carregado...");
}
