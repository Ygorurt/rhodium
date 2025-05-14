#ifndef WALLETWIDGET_H
#define WALLETWIDGET_H

#include <QWidget>
#include "blockchain.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class WalletWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WalletWidget(Blockchain &blockchain, QWidget *parent = nullptr);
    void refreshBalance();

public slots:
    void generateNewWallet();
    void importWallet();
    void showWalletDetails();

private:
    Blockchain &blockchain;
    QLabel *balanceLabel;
    QLineEdit *addressLineEdit;
    QPushButton *generateButton;
    QPushButton *importButton;
    QPushButton *detailsButton;
    QTextEdit *infoTextEdit;
};

#endif // WALLETWIDGET_H