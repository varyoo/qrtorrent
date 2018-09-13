#ifndef RTORRENT_WORKER
#define RTORRENT_WORKER

#include <vector>
#include <QObject>
#include <QDebug>
#include <QTimerEvent>

#include "rtor/rtorrent.h"
#include "rtor/torrent_list.h"
#include "rtor/torrent_list.cc"
#include "torrent.h"


class torrents_daemon : public QObject {
    Q_OBJECT

public:
    rtor::client_ptr client;
    rtor::torrent_list<torrents_daemon, Torrent> list;

private:
    int timer_id;

public:
    torrents_daemon(rtor::client_ptr rtor):
        client(rtor),
        list(client, *this)
    {}
    
public slots:
    void fetch_all(){
        list.fetch_all();
    }

    void swap_client(rtor::client_ptr new_client){
        list.swap_client(new_client);
    }

    void move_downloads(std::vector<std::string> hashes, std::string dest, bool start){
        client->move_downloads(hashes, dest, start);
    }

    void start_torrents(std::vector<std::string> hashes){
        client->start_torrents(hashes);
    }

    void stop_torrents(std::vector<std::string> hashes){
        client->stop_torrents(hashes);
    }

    void remove_torrents(std::vector<std::string> hashes){
        client->delete_torrents(hashes);
    }

    void add_torrents(QString dest_path, std::vector<std::string> filenames, bool start){
        client->add_files(filenames, dest_path.toStdString(), start);
    }

    void timerEvent(QTimerEvent *e) override {
        Q_ASSERT(e->timerId() == timer_id);
        fetch_all();
    }

    void start(int interval){
        timer_id = startTimer(interval);
        Q_ASSERT(timer_id != 0);
    }

    void stop(){
        killTimer(timer_id);
    }

signals:
    void torrent_changed(int, std::shared_ptr<Torrent> t);
    void torrents_inserted(int, std::vector<std::shared_ptr<Torrent> >);
    void torrents_removed(int, int);
};

#endif // RTORRENT_WORKER
