#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSettings>
#include <QLineEdit>
#include <QMetaObject>

#include "config.h"
#include "rtorrent.h"
#include "focus.h"
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
    std::shared_ptr<rtor::client> rtor_client;
    scheduler sched;
    torrents_daemon torrents;
    QThread worker;
    QLineEdit *search;
    QMetaObject::Connection run_fetch_all;
    focus_t focus;

private:
    void schedule_client();

signals:
    void aboutToQuit();
    void finished();
    void add_torrents(QString dest_path, std::vector<std::string> filenames, bool start);
    void update_client(std::shared_ptr<rtor::client>);

private slots:
    void on_actionConnect_triggered();
    void on_actionOpen_triggered();
    void show_details(QString hash);
};

#endif // MAINWINDOW_H
