#ifndef CLIENT_DAEMON
#define CLIENT_DAEMON

#include <memory>
#include <vector>
#include "rtorrent.h"
#include "torrent_list.h"
#include "torrent_list.cc"


class torrent_list_daemon : public QObject {
    Q_OBJECT

public:
    rtorrent &rtor;
    scheduler &sched;
    torrent_list<torrent_list_daemon> list;

    torrent_list_daemon(rtorrent &r, scheduler &s):
        rtor(r), sched(s), list(r, *this)
    {
        /*bool ok = connect(s.get_timer(), &QTimer::timeout,
                this, &client_daemon::fetch_all);
        Q_ASSERT(ok);*/
    }

    template<class file_model_t>
    std::unique_ptr<files_client<file_model_t> >
    files(const QString &hash){
        return std::make_unique<files_client<file_model_t> >(rtor, hash);
    }

public slots:
    void fetch_all(){
        list.fetch_all();
        sched.reschedule();
    }
    void start_torrents(QStringList hashes){
        list.start_torrents(hashes);
    }
    void stop_torrents(QStringList hs){
        list.stop_torrents(hs);
    }
    void remove_torrents(QStringList hs, bool d){
        list.remove_torrents(hs, d);
    }
    void add_files(QString dest, QStringList fs, bool start = false){
        list.add_files(dest, fs, start);
    }
    void move_downloads(QString dest, QStringList hashes, bool start){
        list.move_downloads(dest, hashes, start);
    }

signals:
    void torrent_changed(int, std::shared_ptr<Torrent> t);
    void torrents_inserted(int, std::vector<std::shared_ptr<Torrent> >);
    void torrents_removed(int, int);

};

#endif // CLIENT_DAEMON
