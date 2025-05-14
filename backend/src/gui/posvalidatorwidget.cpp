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
    
    connect(stakingToggle, &QPushButton::clicked, this, &POSValidatorWidget::toggleStaking);
    connect(stakeButton, &QPushButton::clicked, this, &POSValidatorWidget::stakeCoins);
}

void POSValidatorWidget::setupValidatorTable() {
    validatorModel = new QStandardItemModel(0, 3, this);
    validatorModel->setHorizontalHeaderLabels({"Endereço", "Stake", "Último Bloco"});
    validatorTable->setModel(validatorModel);
    validatorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void POSValidatorWidget::updateStakingInfo() {
    if(!validatorAddressInput->text().isEmpty()) {
        double stake = blockchain->getStake(validatorAddressInput->text().toStdString());
        stakeAmountLabel->setText(QString("Stake atual: %1 RHOD").arg(stake, 0, 'f', 8));
        
        // Estimativa simplificada de recompensa
        double reward = stake * 0.05; // 5% de recompensa anual
        expectedRewardLabel->setText(QString("Recompensa estimada: %1 RHOD/dia").arg(reward/365, 0, 'f', 8));
    }
    
    updateValidatorStatus();
    updateValidatorTable();
}

void POSValidatorWidget::toggleStaking() {
    isStaking = !isStaking;
    
    if(isStaking) {
        stakingStatusLabel->setText("Status: Participando como validador");
        stakingToggle->setText("Parar Validação");
    } else {
        stakingStatusLabel->setText("Status: Não participando");
        stakingToggle->setText("Participar do PoS");
    }
}

void POSValidatorWidget::stakeCoins() {
    QString address = validatorAddressInput->text();
    double amount = stakeAmountInput->value();
    
    if(address.isEmpty() || amount <= 0) {
        QMessageBox::warning(this, "Erro", "Digite um endereço e valor válidos");
        return;
    }
    
    // Implementar lógica de stake na blockchain
    // blockchain->addStake(address.toStdString(), amount);
    
    QMessageBox::information(this, "Stake Adicionado", 
        QString("%1 RHOD adicionados ao stake do endereço %2").arg(amount).arg(address));
    
    updateStakingInfo();
}

void POSValidatorWidget::updateValidatorStatus() {
    if(isStaking) {
        validatorStatusLabel->setText("Validador: Ativo (produzindo blocos)");
    } else {
        validatorStatusLabel->setText("Validador: Inativo");
    }
}

void POSValidatorWidget::updateValidatorTable() {
    validatorModel->removeRows(0, validatorModel->rowCount());
    
    // Obter lista de validadores da blockchain
    auto validators = blockchain->getValidators();
    
    for(const auto& val : validators) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QString::fromStdString(val)));
        items.append(new QStandardItem(QString::number(blockchain->getStake(val), 'f', 8));
        items.append(new QStandardItem("N/A")); // Substituir pelo último bloco
        
        validatorModel->appendRow(items);
    }
}
