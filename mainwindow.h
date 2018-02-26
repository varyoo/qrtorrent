#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include"client.h"
#include<QThread>
#include<QSettings>
#include"config.h"
#include<QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void changeEvent(QEvent*);

private:
    Ui::MainWindow *ui;
    Client client;
    QThread worker;
    Config conf{};
    QLineEdit *search;

signals:
    void aboutToQuit();
    void finished();
    void focusLost();
    void focusGained();
    void filesAdded(QString dest, QStringList fs, bool start);
private slots:
    void on_actionConnect_triggered();
    void on_actionOpen_triggered();
};

#endif // MAINWINDOW_H
