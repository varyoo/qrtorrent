#include"files_daemon.h"
#include<xmlrpc-c/girerr.hpp>
#include<QDebug>
#include<QApplication>


files_daemon_t::files_daemon_t(scheduler &sched, client_t &client):
    client(client),
    sched(sched)
{
}

void
files_daemon_t::fetch()
{
    Q_ASSERT(this->thread() != QApplication::instance()->thread());

    std::vector<std::shared_ptr<file_model_t> > files;
    file_model_t *root;

    try {
        root = client.fetch_into(files);
    } catch(girerr::error &e){
        qWarning() << e.what();
        sched.reschedule();
        return;
    }

    emit files_loaded(files, root);
    //sched.reschedule();
}
