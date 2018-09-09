#ifndef TORRENT_H
#define TORRENT_H

#include <QString>
#include <memory>
#include <QDebug>

#include <xmlrpc-c/base.hpp>


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

private:
    Torrent(const xmlrpc_c::carray &);

public:
    static xmlrpc_c::paramList params(){
        xmlrpc_c::paramList params;
        params.addc("").addc("name").
            addc("d.hash=").
            addc("d.name=").
            addc("d.bytes_done=").
            addc("d.size_bytes=").
            addc("d.up.rate=").
            addc("d.up.total=").
            addc("d.down.rate=").
            addc("d.down.total=").
            addc("d.creation_date=").
            addc("d.custom1=").
            addc("d.custom=addtime").
            addc("d.ratio=").
            addc("d.is_active=").
            addc("d.is_hash_checking=").
            addc("d.is_open=").
            addc("d.complete=").
            addc("d.state=");

        Q_ASSERT(params.size() == NB_COLUMNS+2);
        return params;
    }
    
    Torrent(const xmlrpc_c::value &v):
        Torrent(xmlrpc_c::value_array(v).vectorValueValue())
    {}

    friend QDebug &operator<<(QDebug &d, const Torrent &t){
        d << t.hash << ":" << t.name;
        return d;
    }
    
    bool hasChanged(const Torrent &o) const {
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
    
    int percent() const {
        return (bytes_done/(double) size_bytes) * 100;
    }
    
    float calc_ratio() const {
        return upTotal/(float)downTotal;
    }
    
    float reported_ratio() const {
        return ratio/1000.;
    }
    
    bool operator<(const Torrent &other) const {
        return hash < other.hash;
    }
    
    bool operator>(const Torrent &other) const {
        return hash > other.hash;
    }
    
    bool operator==(const Torrent &other) const {
        return hash == other.hash;
    }
};

typedef std::vector<std::shared_ptr<Torrent> > Torrents;

#endif // TORRENT_H
