#include "transactionwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

TransactionWidget::TransactionWidget(Blockchain &blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Formulário de transação
    QFormLayout *formLayout = new QFormLayout();
    senderEdit = new QLineEdit(this);
    receiverEdit = new QLineEdit(this);
    amountEdit = new QLineEdit(this);
    
    formLayout->addRow("Remetente:", senderEdit);
    formLayout->addRow("Destinatário:", receiverEdit);
    formLayout->addRow("Quantidade:", amountEdit);

    // Botão de envio
    sendButton = new QPushButton("Enviar Transação", this);
    connect(sendButton, &QPushButton::clicked, this, &TransactionWidget::createTransaction);

    // Área de log
    transactionLog = new QTextEdit(this);
    transactionLog->setReadOnly(true);

    // Adiciona ao layout
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(sendButton);
    mainLayout->addWidget(transactionLog);

    updateTransactions();
}

void TransactionWidget::createTransaction()
{
    QString from = senderEdit->text();
    QString to = receiverEdit->text();
    QString amountStr = amountEdit->text();
    
    if (from.isEmpty() || to.isEmpty() || amountStr.isEmpty()) {
        QMessageBox::warning(this, "Erro", "Preencha todos os campos");
        return;
    }
    
    bool ok;
    double amount = amountStr.toDouble(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Erro", "Quantidade inválida");
        return;
    }
    
    try {
        blockchain.addTransaction(from.toStdString(), to.toStdString(), amount);
        transactionLog->append(QString("Transação enviada: %1 RHD de %2 para %3")
                             .arg(amount).arg(from).arg(to));
        senderEdit->clear();
        receiverEdit->clear();
        amountEdit->clear();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro", QString("Falha na transação: %1")
                            .arg(e.what()));
    }
}

void TransactionWidget::updateTransactions()
{
    // Implemente a atualização do histórico de transações
    transactionLog->append("Atualizando transações...");
}