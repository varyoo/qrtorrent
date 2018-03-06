#ifndef TORRENT_CLIENT_H
#define TORRENT_CLIENT_H


class torrent_client : public QObject
{ Q_OBJECT
public:
    torrent_client(QObject *parent, rtorrent&, const QString &hash);
private:
    const QString hash;
    Config conf{};
    rtorrent &rtor;
};

#endif // TORRENT_CLIENT_H
