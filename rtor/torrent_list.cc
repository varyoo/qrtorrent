#include <fstream>
#include <sstream>

#include "torrent_list.h"


namespace rtor {

template<typename subscriber, typename torrent>
void torrent_list<subscriber, torrent>::fetch_all(){

    xmlrpc_c::rpc req("d.multicall2", fetch_all_params);
    client->call(req);

    xmlrpc_c::carray torrents_vec = xmlrpc_c::value_array(req.getResult()).
        vectorValueValue();
    size_t i = 0, j = 0;
    const size_t old_size = prev_list.size(),
          new_size = torrents_vec.size();

    std::vector<std::shared_ptr<torrent> > ts;
    ts.reserve(new_size);
    for(const auto &tor_vec : torrents_vec){
        ts.emplace_back(new torrent(tor_vec));
    }

    // the list must be ordered by hash for the comparison with the previous list to work
    std::sort(ts.begin(), ts.end(), [](const std::shared_ptr<torrent> &a,
                const std::shared_ptr<torrent> &b){
            return a->hash < b->hash;
            });

    while(j < old_size && i < new_size){
        if(ts[i]->hash == prev_list[j]->hash){
            // compare two versions of the same torrent,
            // this is the most common scenario
            if(ts[i]->hasChanged(*prev_list[j])){
                subscription.torrent_changed(i, ts[i]);
            }
            i++;
            j++;

        } else if(ts[i]->hash < prev_list[j]->hash){
            // ts[i] is a new torrent
            subscription.torrents_inserted(i, {ts[i]});
            i++;

        } else {
            // prev_list[j] has been removed
            subscription.torrents_removed(i, i+1);
            j++;
        }
    }

    if(j != old_size){
        subscription.torrents_removed(j, old_size);
    } else if(i != new_size){
        subscription.torrents_inserted(i, {ts.begin()+i, ts.end()});
    }

    prev_list = ts;
}

} // rtor
