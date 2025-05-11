#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "blockchain.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateChainStatus();
    void onMineButtonClicked();

private:
    Ui::MainWindow *ui;
    Blockchain blockchain;
    QTimer *updateTimer;
};
#endif // MAINWINDOW_H
