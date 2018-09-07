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
    rtor_client(conf.mk_rtorrent()),
    sched(1000),
    torrents(rtor_client, sched),
    worker(),
    search(new QLineEdit(this)),
    focus(sched)
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
    
    sched.moveToThread(&worker);
    torrents.moveToThread(&worker);

    schedule_client();

    bool ok;
    ok = connect(&sched, &scheduler::finished, &worker, &QThread::quit,
            // very important somehow or the program never exits
            Qt::DirectConnection
            );
    Q_ASSERT(ok);
    ok = connect(this, &MainWindow::aboutToQuit,
            &sched, &scheduler::stop);
    Q_ASSERT(ok);

    qRegisterMetaType<std::vector<std::string> >("std::vector<std::string>");
    qRegisterMetaType<std::string>("std::string");

    // Add torrents
    connect(this, &MainWindow::add_torrents,
            &torrents, &torrents_daemon::add_torrents);

    qRegisterMetaType<std::shared_ptr<rtor::client> >("std::shared_ptr<rtor::client>");
    connect(this, &MainWindow::update_client,
            &torrents, &torrents_daemon::swap_client);

    qDebug() << "Main thread id =" << QThread::currentThreadId();
    
    ok = connect(ui->tableView, &Table::torrentsStarted,
            &torrents, &torrents_daemon::start_torrents);
    Q_ASSERT(ok);
    ok = connect(ui->tableView, &Table::torrentsStopped,
            &torrents, &torrents_daemon::stop_torrents);
    Q_ASSERT(ok);
    ok = connect(ui->tableView, &Table::torrentsRemoved,
            &torrents, &torrents_daemon::remove_torrents);
    Q_ASSERT(ok);
    ok = connect(ui->tableView, &Table::details_requested,
            this, &MainWindow::show_details);
    Q_ASSERT(ok);

    // Set downloads location
    connect(ui->tableView, &Table::move_downloads,
            &torrents, &torrents_daemon::move_downloads);

    qRegisterMetaType<std::vector<std::shared_ptr<Torrent> > >
        ("std::vector<std::shared_ptr<Torrent> >");
    qRegisterMetaType<std::shared_ptr<Torrent> >("std::shared_ptr<Torrent>");
    ok = connect(&torrents, &torrents_daemon::torrent_changed,
            ui->tableView, &Table::changeTorrent);
    Q_ASSERT(ok);
    ok = connect(&torrents, &torrents_daemon::torrents_inserted,
            ui->tableView, &Table::insertTorrents);
    Q_ASSERT(ok);
    ok = connect(&torrents, &torrents_daemon::torrents_removed,
            ui->tableView, &Table::removeTorrents);
    Q_ASSERT(ok);

    ok = connect(&worker, SIGNAL(started()), &sched, SLOT(resume()));
    Q_ASSERT(ok);

    worker.start();
}

MainWindow::~MainWindow()
{
    emit aboutToQuit();
    worker.wait();
    delete ui;
}

void MainWindow::on_actionConnect_triggered(){
    Connect dialog(this);
    dialog.exec();

    if(dialog.result() == QDialog::Accepted){
        auto new_client = conf.mk_rtorrent();

        // this client may be used by the torrent files dialog
        rtor_client = new_client;

        emit update_client(conf.mk_rtorrent());
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange){
        if(this->isActiveWindow()){
            focus.focus_in();
        } else {
            focus.focus_out();
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
    // stop the client daemon
    focus.disconnect();
    disconnect(run_fetch_all);
    
    files_client<file_model_t> fc(rtor_client, hash);

    files_daemon_t fd(sched, fc);
    fd.moveToThread(&worker);

    details_dialog d(this, fd);
    d.exec();

    schedule_client();
    focus.reconnect();
}

void MainWindow::schedule_client()
{
    run_fetch_all = connect(sched.get_timer(), &QTimer::timeout,
            &torrents, &torrents_daemon::fetch_all, Qt::DirectConnection);
    Q_ASSERT(run_fetch_all);
}
