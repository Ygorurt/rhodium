#pragma once

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include "blockchain.h"

class TransactionWidget : public QWidget {
    Q_OBJECT

public:
    explicit TransactionWidget(Blockchain* blockchain, QWidget *parent = nullptr);

private slots:
    void sendTransaction();
    void updateBalanceDisplay();

private:
    Blockchain* blockchain;
    
    QLineEdit *senderInput;
    QLineEdit *recipientInput;
    QDoubleSpinBox *amountInput;
    QLineEdit *signatureInput;
    QPushButton *sendButton;
    QTextEdit *txHistory;
    QLabel *balanceLabel;
    
    void setupUI();
};
