#include"details_dialog.h"
#include"ui_details_dialog.h"
#include"file_model.h"


details_dialog::details_dialog(QWidget *parent, files_daemon_t &fd) :
    QDialog(parent),
    ui(new Ui::details_dialog),
    daemon(fd),
    focus(daemon.sched)
{
    ui->setupUi(this);

    bool ok;
    qRegisterMetaType<std::vector<std::shared_ptr<file_model_t> > >(
            "std::vector<std::shared_ptr<file_model_t> >");
    ok = connect(&daemon, &files_daemon_t::files_loaded,
            &ui->files->content_model(), &torrent_files_model::set_files);
    Q_ASSERT(ok);

    // clock tick
    run_fetch = connect(daemon.sched.get_timer(), &QTimer::timeout,
            &daemon, &files_daemon_t::fetch, Qt::DirectConnection);
    Q_ASSERT(run_fetch);
}

details_dialog::~details_dialog()
{
    delete ui;
}

void details_dialog::focusInEvent(QFocusEvent*)
{
    focus.focus_in();
}

void details_dialog::focusOutEvent(QFocusEvent*)
{
    focus.focus_out();
}

void details_dialog::done(int r)
{
    focus.disconnect();
    QObject::disconnect(run_fetch);

    return QDialog::done(r);
}
