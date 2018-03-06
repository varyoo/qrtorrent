#ifndef FILES_DAEMON_H
#define FILES_DAEMON_H

#include"file_model.h"
#include<vector>
#include"files_client.h"


typedef files_client<file_model_t> client_t;

class files_daemon_t : public QObject
{
Q_OBJECT
public:
    files_daemon_t(scheduler &sched, client_t &client);
private:
    client_t &client;
public:
    scheduler &sched;
public slots:
    void fetch();
signals:
    void files_loaded(std::vector<std::shared_ptr<file_model_t> > files,
            file_model_t *root);
};

#endif // FILES_DAEMON_H
