#include <QTimer>
#include <QThread>
#include <QFileDialog>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connect.h"
#include "adddialog.h"
#include "details_dialog.h"
#include "files_daemon.h"
#include "move_dialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    torrents(conf.mk_rtorrent()),
    worker(),
    search(new QLineEdit(this)),
    torrents_schedule(1000)
{
    ui->setupUi(this);
    qDebug() << "Using configuration file" << conf.fileName();

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->addWidget(spacer);
    search->setPlaceholderText("Filter torrent list...");
    search->setFixedWidth(200);
    ui->mainToolBar->addWidget(search);
    connect(search, &QLineEdit::textChanged,
            ui->tableView, &Table::apply_name_filter);
    ui->actionConnect->setIcon(style()->standardIcon(QStyle::SP_DriveNetIcon));
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    
    torrents.moveToThread(&worker);
    torrents_schedule.moveToThread(&worker);

    // stop the worker thread
    connect(this, &MainWindow::about_to_quit,
            &torrents_schedule, &scheduler::finish);
    connect(&torrents_schedule, &scheduler::finished,
            &worker, &QThread::quit, Qt::DirectConnection);

    qRegisterMetaType<std::vector<std::string> >("std::vector<std::string>");
    qRegisterMetaType<std::string>("std::string");

    // Add torrents
    connect(this, &MainWindow::add_torrents,
            &torrents, &torrents_daemon::add_torrents);

    qRegisterMetaType<std::shared_ptr<rtor::client> >("std::shared_ptr<rtor::client>");
    connect(this, &MainWindow::update_client,
            &torrents, &torrents_daemon::swap_client);

    qDebug() << "Main thread id =" << QThread::currentThreadId();
    
    connect(ui->tableView, &Table::torrentsStarted,
            &torrents, &torrents_daemon::start_torrents);
    connect(ui->tableView, &Table::torrentsStopped,
            &torrents, &torrents_daemon::stop_torrents);
    connect(ui->tableView, &Table::torrentsRemoved,
            &torrents, &torrents_daemon::remove_torrents);
    connect(ui->tableView, &Table::details_requested,
            this, &MainWindow::show_details);

    // Set downloads location
    connect(ui->tableView, &Table::move_downloads,
            &torrents, &torrents_daemon::move_downloads);

    qRegisterMetaType<std::vector<std::shared_ptr<Torrent> > >
        ("std::vector<std::shared_ptr<Torrent> >");
    qRegisterMetaType<std::shared_ptr<Torrent> >("std::shared_ptr<Torrent>");
    connect(&torrents, &torrents_daemon::torrent_changed,
            ui->tableView, &Table::changeTorrent);
    connect(&torrents, &torrents_daemon::torrents_inserted,
            ui->tableView, &Table::insertTorrents);
    connect(&torrents, &torrents_daemon::torrents_removed,
            ui->tableView, &Table::removeTorrents);

    // fetch schedule
    connect(&torrents_schedule, &scheduler::run_fetch,
            &torrents, &torrents_daemon::fetch_all,
            Qt::DirectConnection);
    connect(this, &MainWindow::fetch_start,
            &torrents_schedule, &scheduler::start);
    connect(this, &MainWindow::fetch_stop,
            &torrents_schedule, &scheduler::stop);

    worker.start();
}

MainWindow::~MainWindow()
{
    emit about_to_quit();
    worker.wait();
    delete ui;
}

void MainWindow::on_actionConnect_triggered(){
    Connect dialog(this);
    dialog.exec();

    if(dialog.result() == QDialog::Accepted){
        emit update_client(conf.mk_rtorrent());
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    
    if(event->type() == QEvent::ActivationChange){
        if(this->isActiveWindow()){
            emit fetch_start();
        } else {
            emit fetch_stop();
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,
            "Open torrents",
            QDir::currentPath(),
            "Torrent files (*.torrent)");
    if(filenames.size() == 0){
        qDebug() << "Add torrents: no files were selected";
        return;
    }

    AddDialog dialog(this);

    dialog.exec();

    if(dialog.result() == QDialog::Accepted){
        std::vector<std::string> vec;
        for(const auto &f : filenames){
            vec.push_back(f.toStdString());
        }

        emit add_torrents(dialog.dest_path, vec, dialog.start_torrents);
    }
}

void MainWindow::show_details(QString hash)
{
    // stop the torrents timer
    emit fetch_stop();
    
    files_client<file_model_t> client(conf.mk_rtorrent(), hash);

    files_daemon fd(client);
    fd.moveToThread(&worker);

    details_dialog d(this, fd);
    d.exec();

    // the timer will start again when focus is reached
}
