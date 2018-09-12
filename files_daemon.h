#ifndef FILES_DAEMON_H
#define FILES_DAEMON_H

#include <vector>
#include <QDebug>

#include "file_model.h"
#include "files_client.h"


typedef files_client<file_model_t> fd_client;

class files_daemon : public QObject {
    Q_OBJECT

private:
    fd_client &client;

public:
    files_daemon(fd_client &client):
        client(client)
    {}

public slots:
    void fetch(){
        std::vector<std::shared_ptr<file_model_t> > files;
        file_model_t *root;

        root = client.fetch_into(files);
        emit files_loaded(files, root);
    }

signals:
    void files_loaded(std::vector<std::shared_ptr<file_model_t> > files,
            file_model_t *root);
};

#endif // FILES_DAEMON_H
