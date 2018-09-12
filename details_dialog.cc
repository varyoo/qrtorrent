#include "details_dialog.h"
#include "ui_details_dialog.h"
#include "file_model.h"


details_dialog::details_dialog(QWidget *parent, files_daemon &fd) :
    QDialog(parent),
    ui(new Ui::details_dialog),
    daemon(fd)
{
    ui->setupUi(this);

    qRegisterMetaType<std::vector<std::shared_ptr<file_model_t> > >(
            "std::vector<std::shared_ptr<file_model_t> >");
    connect(&daemon, &files_daemon::files_loaded,
            &ui->files->content_model(), &torrent_files_model::set_files);

    connect(this, &details_dialog::fetch_run_once,
            &daemon, &files_daemon::fetch);

    emit fetch_run_once();
}

details_dialog::~details_dialog()
{
    delete ui;
}
