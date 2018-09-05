#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include"torrent_list_daemon.h"
#include<QThread>
#include<QSettings>
#include"config.h"
#include<QLineEdit>
#include"rtorrent.h"
#include"focus.h"
#include<QMetaObject>


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
    rtorrent rtor;
    scheduler sched;
    torrent_list_daemon client;
    QThread worker;
    Config conf{};
    QLineEdit *search;
    QMetaObject::Connection run_fetch_all;
    focus_t focus;

private:
    void schedule_client();

signals:
    void aboutToQuit();
    void finished();
    void add_torrents(QString dest_path, QStringList filenames, bool start);
    void move_downloads(QString dest_path, QStringList hashes, bool move_data);

private slots:
    void on_actionConnect_triggered();
    void on_actionOpen_triggered();
    void show_details(QString hash);
    void update_torrents(QStringList hashes);
};

#endif // MAINWINDOW_H
