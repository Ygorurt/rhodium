#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include "../blockchain/blockchain.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onMineButtonClicked();
    void onSendPixClicked();
    void updateChainStatus();

private:
    Ui::MainWindow *ui;
    Blockchain blockchain_;
    QTimer *updateTimer_;
    
    void setupUI();
    void log(const QString& message);
};
#endif // MAINWINDOW_H
