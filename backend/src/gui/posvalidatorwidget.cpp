#include "posvalidatorwidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

POSValidatorWidget::POSValidatorWidget(Blockchain* blockchain, QWidget *parent)
    : QWidget(parent), blockchain(blockchain), isStaking(false) {
    setupUI();
    setupValidatorTable();
    
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &POSValidatorWidget::updateStakingInfo);
    updateTimer->start(3000);
}

void POSValidatorWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Informações de staking
    stakeAmountLabel = new QLabel("Stake atual: 0 RHOD");
    expectedRewardLabel = new QLabel("Recompensa estimada: 0 RHOD/bloco");
    stakingStatusLabel = new QLabel("Status: Não participando");
    validatorStatusLabel = new QLabel("Validador: Inativo");
    
    // Controles
    stakingToggle = new QPushButton("Participar do PoS");
    stakeButton = new QPushButton("Adicionar Stake");
    stakeAmountInput = new QDoubleSpinBox();
    stakeAmountInput->setRange(0.1, 1000000);
    stakeAmountInput->setDecimals(8);
    validatorAddressInput = new QLineEdit();
    
    QFormLayout *form = new QFormLayout();
    form->addRow("Quantidade para stake:", stakeAmountInput);
    form->addRow("Endereço do validador:", validatorAddressInput);
    
    // Tabela de validadores
    validatorTable = new QTableView();
    
    layout->addWidget(stakeAmountLabel);
    layout->addWidget(expectedRewardLabel);
    layout->addWidget(stakingStatusLabel);
    layout->addWidget(validatorStatusLabel);
    layout->addLayout(form);
    layout->addWidget(stakeButton);
    layout->addWidget(stakingToggle);
    layout->addWidget(new QLabel("Validadores Ativos:"));
    layout->addWidget(validatorTable);
    
    connect(stakingToggle, &QPushButton
