#include"mainwindow.h"
#include"ui_mainwindow.h"
#include<QTimer>
#include<QThread>
#include"connect.h"
#include<QFileDialog>
#include<QDir>
#include"adddialog.h"
#include"details_dialog.h"
#include"files_daemon.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rtor(),
    sched(1000),
    client(rtor, sched),
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
    rtor.moveToThread(&worker);
    client.moveToThread(&worker);

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

    qDebug() << "Main thread id =" << QThread::currentThreadId();
    
    ok = connect(ui->tableView, &Table::torrentsStarted,
            &client, &torrent_list_daemon::start_torrents);
    Q_ASSERT(ok);
    ok = connect(ui->tableView, &Table::torrentsStopped,
            &client, &torrent_list_daemon::stop_torrents);
    Q_ASSERT(ok);
    ok = connect(ui->tableView, &Table::torrentsRemoved,
            &client, &torrent_list_daemon::remove_torrents);
    Q_ASSERT(ok);
    ok = connect(ui->tableView, &Table::details_requested,
            this, &MainWindow::show_details);
    Q_ASSERT(ok);

    qRegisterMetaType<std::vector<std::shared_ptr<Torrent> > >
        ("std::vector<std::shared_ptr<Torrent> >");
    qRegisterMetaType<std::shared_ptr<Torrent> >("std::shared_ptr<Torrent>");
    ok = connect(&client, &torrent_list_daemon::torrent_changed,
            ui->tableView, &Table::changeTorrent);
    Q_ASSERT(ok);
    ok = connect(&client, &torrent_list_daemon::torrents_inserted,
            ui->tableView, &Table::insertTorrents);
    Q_ASSERT(ok);
    ok = connect(&client, &torrent_list_daemon::torrents_removed,
            ui->tableView, &Table::removeTorrents);
    Q_ASSERT(ok);

    ok = connect(&worker, SIGNAL(started()), &sched, SLOT(resume()));
    Q_ASSERT(ok);

    ok = connect(this, &MainWindow::filesAdded,
            &client, &torrent_list_daemon::add_files);
    Q_ASSERT(ok);

    worker.start();
}

MainWindow::~MainWindow()
{
    emit aboutToQuit();
    worker.wait();
    delete ui;
}

void MainWindow::on_actionConnect_triggered()
{
   Connect c(this);
   c.connect(&c, &Connect::connectionChanged,
           &rtor, &rtorrent::updateConnection);
   c.exec();
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
    QStringList fs = QFileDialog::getOpenFileNames(this,
            "Open torrents",
            QDir::currentPath(),
            "Torrent files (*.torrent)");
    if(fs.size() == 0){
        qDebug() << "Add torrents: no files were selected";
        return;
    }

    AddDialog ad(this, fs);
    ad.connect(&ad, &AddDialog::filesAdded,
            &client, &torrent_list_daemon::add_files);
    ad.exec();
}

void MainWindow::show_details(QString hash)
{
    // stop the client daemon
    focus.disconnect();
    disconnect(run_fetch_all);
    
    auto fc = client.files<file_model_t>(hash);

    files_daemon_t fd(sched, *fc);
    fd.moveToThread(&worker);

    details_dialog d(this, fd);
    d.exec();

    schedule_client();
    focus.reconnect();
}

void MainWindow::schedule_client()
{
    run_fetch_all = connect(sched.get_timer(), &QTimer::timeout,
            &client, &torrent_list_daemon::fetch_all, Qt::DirectConnection);
    Q_ASSERT(run_fetch_all);
}
