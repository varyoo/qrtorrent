#ifndef CLIENT_H
#define CLIENT_H

#include<libssh/libsshpp.hpp>
#include<xmlrpc-c/girerr.hpp>
#include<xmlrpc-c/base.hpp>
#include<xmlrpc-c/client.hpp>
#include<iostream>
#include<memory>
#include"rtorrent.h"
#include"scheduler.h"
#include"files_client.h"
#include "torrent.h"


template<typename listener_t>
class torrent_list {
private:
    static constexpr int interval = 1000;
    std::vector<std::shared_ptr<Torrent> > old;
    xmlrpc_c::paramList fetchAllParams;
    rtorrent &rtor;
    listener_t &listener;

private:
    static void loadFileInto(xmlrpc_c::cbytestring&, QString);
    static std::shared_ptr<Torrent> parse_torrent(const xmlrpc_c::value &v){
        xmlrpc_c::value_array va(v);
        auto fs = va.vectorValueValue();
        auto t = std::make_shared<Torrent>(fs);
        return t;
    }

    // needs heavy tests as i don't understand half of these
    void removeLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);
    void insertLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);
    void equalLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);

public:
    torrent_list(rtorrent &rtor, listener_t &listener);
    ~torrent_list(){}

    void fetch_all();
    void start_torrents(QStringList hashes){
        rtor.cmdForHashes("d.start", hashes);
    }
    void stop_torrents(QStringList hashes){
        rtor.cmdForHashes("d.stop", hashes);
    }
    void remove_torrents(QStringList hashes, bool remove_data){
        (void) remove_data;
        rtor.cmdForHashes("d.erase", hashes);
    }
    void add_files(QString dest, QStringList fs, bool start = false);
};

#endif // CLIENT_H
