#ifndef CLIENT_H
#define CLIENT_H

#include<libssh/libsshpp.hpp>
#include<xmlrpc-c/girerr.hpp>
#include<xmlrpc-c/base.hpp>
#include<xmlrpc-c/client.hpp>
#include<iostream>
#include<QDebug>
#include<memory>
#include<QTimer>
#include<config.h>
#include"rtorrent.h"
#include"scheduler.h"

/*typedef const std::array<const std::string> torrent_path;

class torrent_file {
public:
    const QString basename;
    torrent_file(const QString &basename):
        basename(basename){}
};*/

class Torrent {
public:
    enum {
        HASH,
        NAME,
        BYTES_DONE,
        SIZE_BYTES,
        UP_RATE,
        UP_TOTAL,
        DOWN_RATE,
        DOWN_TOTAL,
        CREATION_DATE,
        TAGS,
        ADDTIME,
        RATIO,
        IS_ACTIVE,
        IS_HASH_CHECKING,
        IS_OPEN,
        COMPLETE,
        STATE,

        NB_COLUMNS
    };
    enum State {
        Checking,
        Downloading,
        Stopped,
        Seeding
    };
    const QString hash;
    const QString name;
    const qint64 bytes_done;
    const qint64 size_bytes;
    const qint64 upRate;
    const qint64 upTotal;
    const qint64 downRate;
    const qint64 downTotal;
    const qint64 creation_date;
    const qint64 addtime;
    const qint64 ratio;
    const bool is_active;
    const bool is_hash_checking;
    const bool is_open;
    const bool complete;
    const bool state;
    State display_state;

public:
    Torrent(xmlrpc_c::carray);
    friend QDebug &operator<<(QDebug&, Torrent const&);
    inline bool hasChanged(const Torrent &o) const {
        Q_ASSERT(o.hash == hash);

        return o.name != name ||
            o.bytes_done != bytes_done ||
            size_bytes != o.size_bytes ||
            upRate != o.upRate || upTotal != o.upTotal ||
            downRate != o.downRate || downTotal != o.downTotal ||
            is_active != o.is_active ||
            is_hash_checking != o.is_hash_checking ||
            is_open != o.is_open ||
            complete != o.complete ||
            state != o.state;
    }
    inline int percent() const {
        return (bytes_done/(double) size_bytes) * 100;
    }
    inline float calc_ratio() const {
        return upTotal/(float)downTotal;
    }
    inline float reported_ratio() const {
        return ratio/1000.;
    }
    inline bool operator<(const Torrent &other) const {
        return hash < other.hash;
    }
    inline bool operator>(const Torrent &other) const {
        return hash > other.hash;
    }
    inline bool operator==(const Torrent &other) const {
        return hash == other.hash;
    }
};

typedef std::vector<std::shared_ptr<Torrent> > Torrents;


class Client : public QObject
{
    Q_OBJECT

private:
    static const int interval = 1000;
    std::vector<std::shared_ptr<Torrent> > old;
    xmlrpc_c::paramList fetchAllParams;
    Config conf{};
    rtorrent &rtor;

public:
    scheduler sched;

private:
    static void loadFileInto(xmlrpc_c::cbytestring&, QString);
    static std::shared_ptr<Torrent> parse_torrent(const xmlrpc_c::value&);
    //void sshConnect(); 

    // needs heavy tests as i don't understand half of these
    void removeLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);
    void insertLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);
    void equalLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);

public:
    Client(rtorrent &rtor);
    ~Client();

public slots:
    void fetchAll();
    void startTorrents(QStringList hashes);
    void stopTorrents(QStringList hashes);
    void removeTorrents(QStringList hashes, bool deleteData);
    void addFiles(QString dest, QStringList fs, bool start = false);
    //void sshReconnect();

signals:
    void torrentChanged(int, std::shared_ptr<Torrent>);
    void torrentsInserted(int, std::vector<std::shared_ptr<Torrent> >);
    void torrentsRemoved(int, int);

    void finished();
    //void sshError(ssh::SshException e);
    //void sshConnected();
};

#endif // CLIENT_H
