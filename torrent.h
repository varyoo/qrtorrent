#ifndef TORRENT_H
#define TORRENT_H

#include <QString>
#include <xmlrpc-c/base.hpp>
#include <memory>


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
    friend QDebug &operator<<(QDebug&, const Torrent&);
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

#endif // TORRENT_H
