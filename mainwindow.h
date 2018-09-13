#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSettings>
#include <QLineEdit>
#include <QMetaObject>

#include "config.h"
#include "rtor/rtorrent.h"
#include "torrents_daemon.h"


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
    Config conf{};
    torrents_daemon torrents;
    QThread worker;
    QLineEdit *search;
    
signals:
    void about_to_quit();
    
    void add_torrents(QString dest_path, std::vector<std::string> filenames, bool start);
    void update_client(std::shared_ptr<rtor::client>);

    void fetch_start(int interval);
    void fetch_stop();

    void fetch_now();

private slots:
    void on_actionConnect_triggered();
    void on_actionOpen_triggered();
    
    void show_details(QString hash);
};

#endif // MAINWINDOW_H
