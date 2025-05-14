#pragma once

#include <QWidget>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QTimer>

class POSValidatorWidget : public QWidget {
    Q_OBJECT

public:
    POSValidatorWidget(Blockchain* blockchain, QWidget *parent = nullptr);

private slots:
    void updateStakingInfo();
    void toggleStaking();

private:
    Blockchain* blockchain;
    QTimer *updateTimer;
    
    QLabel *stakeAmountLabel;
    QLabel *expectedRewardLabel;
    QLabel *stakingStatusLabel;
    QPushButton *stakingToggle;
    QTableView *validatorTable;
    QStandardItemModel *validatorModel;
    
    void setupValidatorTable();
};
