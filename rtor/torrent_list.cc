#include <fstream>
#include <sstream>

#include "torrent_list.h"


namespace rtor {

template<typename subscriber, typename torrent>
inline void torrent_list<subscriber, torrent>::remove_loop(std::vector<std::shared_ptr<torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    size_t d = j;
    
    while(d < old_size){
        if(old[d]->hash < ts[i]->hash){
            d++;
        } else {
            if(j != d){
                subscription.torrents_removed(j, d);
                j = d;
            }

            return equal_loop(ts, old_size, new_size, j, i);
        }
    }
}

template<typename subscriber, typename torrent>
void torrent_list<subscriber, torrent>::insert_loop(std::vector<std::shared_ptr<torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    size_t d = i;
    std::vector<std::shared_ptr<torrent> > delta;
    
    while(d < new_size){
        if(j == old_size || old[j]->hash > ts[d]->hash){
            delta.push_back(ts[d]);
            d++;
        } else {
            if(i != d){
                subscription.torrents_inserted(i, delta);
                i = d;
            }

            return remove_loop(ts, old_size, new_size, j, i);
        }
    }
}

template<typename subscriber, typename torrent>
inline void torrent_list<subscriber, torrent>::equal_loop(std::vector<std::shared_ptr<torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    while(j < old_size && i < new_size){
        if(old[j]->hash == ts[i]->hash){
            if(ts[i]->hasChanged(*old[j])){
                subscription.torrent_changed(i, ts[i]);
            }
            i++;
            j++;
        } else {
            return insert_loop(ts, old_size, new_size, j, i);
        }
    }
}

template<typename subscriber, typename torrent>
void torrent_list<subscriber, torrent>::fetch_all(){

    xmlrpc_c::rpc req("d.multicall2", fetch_all_params);
    client->call(req);

    xmlrpc_c::value_array v(xmlrpc_c::value_array(req.getResult()));
    std::vector<xmlrpc_c::value> vv = v.vectorValueValue();
    size_t j = 0;
    size_t i = 0;
    const size_t old_size = old.size();
    const size_t new_size = vv.size();

    std::vector<std::shared_ptr<torrent> > ts;
    ts.reserve(vv.size());
    for(const xmlrpc_c::value &tv : vv){
        ts.push_back(std::make_shared<torrent>(tv));
    }

    // the list must be ordered by hash for the comparison with the previous list to work
    std::sort(ts.begin(), ts.end(), [](const std::shared_ptr<torrent> &a,
                const std::shared_ptr<torrent> &b){
            return a->hash < b->hash;
            });
    
    insert_loop(ts, old_size, new_size, j, i);

    if(j != old_size){
        subscription.torrents_removed(j, old_size);
    } else if(i != new_size){
        subscription.torrents_inserted(i, {ts.begin()+i, ts.end()});
    }

    old = ts;
    //sched.reschedule();
}

} // rtor
