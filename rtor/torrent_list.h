#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <memory>

#include "rtorrent.h"

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client.hpp>


namespace rtor {

template<typename subscriber, typename torrent>
class torrent_list {

private:
    std::vector<std::shared_ptr<torrent> > prev_list;
    xmlrpc_c::paramList fetch_all_params;
    rtor::client_ptr client;
    subscriber &subscription;

public:
    torrent_list(rtor::client_ptr client, subscriber &sub):
        fetch_all_params(torrent::params()),
        client(client),
        subscription(sub)
    {}

    void fetch_all();

    void swap_client(const rtor::client_ptr &new_client){
        // 1. remove all torrents from GUI
        subscription.torrents_removed(0, prev_list.size());

        // 2. remove all torrents
        prev_list.clear();

        // 3. connect to the new server
        client = std::move(new_client);
    }
};

} // rtor

#endif // CLIENT_H
