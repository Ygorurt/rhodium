#ifndef TRANSACTIONWIDGET_H
#define TRANSACTIONWIDGET_H

#include <QWidget>
#include "blockchain.h"

class QLineEdit;
class QPushButton;
class QTextEdit;

class TransactionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionWidget(Blockchain &blockchain, QWidget *parent = nullptr);
    void updateTransactions();

public slots:
    void createTransaction();

private:
    Blockchain &blockchain;
    QLineEdit *senderEdit;
    QLineEdit *receiverEdit;
    QLineEdit *amountEdit;
    QPushButton *sendButton;
    QTextEdit *transactionLog;
};

#endif // TRANSACTIONWIDGET_H