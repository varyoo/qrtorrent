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
    static constexpr int interval = 1000;
    std::vector<std::shared_ptr<torrent> > old;
    xmlrpc_c::paramList fetch_all_params;

public:
    rtor::client_ptr client;

private:
    subscriber &subscription;

private:
    // needs heavy tests as i don't understand half of these
    void remove_loop(std::vector<std::shared_ptr<torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);
    void insert_loop(std::vector<std::shared_ptr<torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);
    void equal_loop(std::vector<std::shared_ptr<torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i);

public:
    torrent_list(rtor::client_ptr client, subscriber &sub):
        fetch_all_params(torrent::params()),
        client(client),
        subscription(sub)
    {}

    void fetch_all();
};

} // rtor

#endif // CLIENT_H
