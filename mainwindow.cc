#include"mainwindow.h"
#include"ui_mainwindow.h"
#include<QTimer>
#include<QThread>
#include"connect.h"
#include<QFileDialog>
#include<QDir>
#include"adddialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    client(),
    worker(),
    search(new QLineEdit(this))
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
    
    client.moveToThread(&worker);

    connect(&client, &Client::finished, &worker, &QThread::quit,
            // very important somehow or the program never exits
            Qt::DirectConnection
            );
    connect(this, SIGNAL(aboutToQuit()), &client, SLOT(stop()));

    qDebug() << "Main thread id =" << QThread::currentThreadId();
    
    connect(ui->tableView, &Table::torrentsStarted,
            &client, &Client::startTorrents);
    connect(ui->tableView, &Table::torrentsStopped,
            &client, &Client::stopTorrents);
    connect(ui->tableView, &Table::torrentsRemoved,
            &client, &Client::removeTorrents);

    qRegisterMetaType<std::vector<std::shared_ptr<Torrent> > >
        ("std::vector<std::shared_ptr<Torrent> >");
    qRegisterMetaType<std::shared_ptr<Torrent> >("std::shared_ptr<Torrent>");
    connect(&client, &Client::torrentChanged,
            ui->tableView, &Table::changeTorrent);
    connect(&client, &Client::torrentsInserted,
            ui->tableView, &Table::insertTorrents);
    connect(&client, &Client::torrentsRemoved,
            ui->tableView, &Table::removeTorrents);

    connect(&worker, SIGNAL(started()), &client, SLOT(start()));

    connect(this, &MainWindow::filesAdded,
            &client, &Client::addFiles);

    // stop this fetch all torrents every second hell when the window doesn't have focus
    connect(this, &MainWindow::focusLost,
            &client, &Client::pause);
    connect(this, &MainWindow::focusGained,
            &client, &Client::resume);

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
           &client, &Client::updateConnection);
   c.exec();
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange){
        if(this->isActiveWindow()){
            emit focusGained();
        } else {
            emit focusLost();
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
            &client, &Client::addFiles);
    ad.exec();
}
