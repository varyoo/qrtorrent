#include"torrent_client.h"

torrent_client::torrent_client(QObject *parent, rtorrent &rtor, const QString &hash):
    QObject(parent),
    hash(hash),
    rtor(rtor)
{}
