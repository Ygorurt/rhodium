#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , blockchain()
    , updateTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Configuração inicial dos labels
    ui->chainLengthLabel->setText(QString("Comprimento da cadeia: %1")
                                .arg(blockchain.getChainLength()));
    ui->pendingTxsLabel->setText(QString("Transações pendentes: %1")
                               .arg(blockchain.getPendingTransactions()));

    // Configuração do timer
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateChainStatus);
    updateTimer->start(5000); // Atualiza a cada 5 segundos

    QPushButton* mineButton = new QPushButton("Minar", this);
mineButton->setObjectName("mineButton");
mineButton->setGeometry(100, 100, 100, 30);  // Ajuste posição/tamanho
connect(mineButton, &QPushButton::clicked,
        this, &MainWindow::onMineButtonClicked);

    // Conexão dos botões
    connect(ui->mineButton, &QPushButton::clicked, 
            this, &MainWindow::onMineButtonClicked);
}

// Implementações Qt obrigatórias
const QMetaObject* MainWindow::metaObject() const {
    return QObject::d_ptr->metaObject ? 
           QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void* MainWindow::qt_metacast(const char* className) {
    if (!strcmp(className, "MainWindow"))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(className);
}

int MainWindow::qt_metacall(QMetaObject::Call call, int id, void** args) {
    id = QMainWindow::qt_metacall(call, id, args);
    if (id < 0)
        return id;
    return id;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateChainStatus()
{
    ui->chainLengthLabel->setText(QString("Comprimento da cadeia: %1")
                                .arg(blockchain.getChainLength()));
    ui->pendingTxsLabel->setText(QString("Transações pendentes: %1")
                               .arg(blockchain.getPendingTransactions()));
}

void MainWindow::onMineButtonClicked()
{
    blockchain.mineBlock("miner_address");
    updateChainStatus();
}
