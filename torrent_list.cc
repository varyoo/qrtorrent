#include "torrent_list.h"
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <fstream>
#include <QDateTime>
#include <sstream>


template<typename listener_t>
torrent_list<listener_t>::torrent_list(std::shared_ptr<rtor::client> rtor, listener_t &listener):
    client(rtor),
    listener(listener)
{
    const std::string columns[Torrent::NB_COLUMNS+2]{"", "name",
        "d.hash=",
        "d.name=",
        "d.bytes_done=",
        "d.size_bytes=",
        "d.up.rate=",
        "d.up.total=",
        "d.down.rate=",
        "d.down.total=",
        "d.creation_date=",
        "d.custom1=",
        "d.custom=addtime",
        "d.ratio=",
        "d.is_active=",
        "d.is_hash_checking=",
        "d.is_open=",
        "d.complete=",
        "d.state="
    };
    for(const std::string &s : columns){
        fetch_all_params.add(xmlrpc_c::value_string(s));
    }
}

template<typename listener_t>
inline void torrent_list<listener_t>::removeLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    size_t d = j;
    
    while(d < old_size){
        if(old[d]->hash < ts[i]->hash){
            d++;
        } else {
            if(j != d){
                listener.torrents_removed(j, d);
                j = d;
            }

            return equalLoop(ts, old_size, new_size, j, i);
        }
    }
}

template<typename listener_t>
void torrent_list<listener_t>::insertLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    size_t d = i;
    std::vector<std::shared_ptr<Torrent> > delta;
    
    while(d < new_size){
        if(j == old_size || old[j]->hash > ts[d]->hash){
            delta.push_back(ts[d]);
            d++;
        } else {
            if(i != d){
                listener.torrents_inserted(i, delta);
                i = d;
            }

            return removeLoop(ts, old_size, new_size, j, i);
        }
    }
}

template<typename listener_t>
inline void torrent_list<listener_t>::equalLoop(std::vector<std::shared_ptr<Torrent> > &ts,
        const size_t &old_size, const size_t &new_size, size_t &j, size_t &i)
{
    while(j < old_size && i < new_size){
        if(old[j]->hash == ts[i]->hash){
            if(ts[i]->hasChanged(*old[j])){
                listener.torrent_changed(i, ts[i]);
            }
            i++;
            j++;
        } else {
            return insertLoop(ts, old_size, new_size, j, i);
        }
    }
}

template<typename listener_t>
void torrent_list<listener_t>::fetch_all(){

    xmlrpc_c::rpc req("d.multicall2", fetch_all_params);
    client->call(req);

    xmlrpc_c::value_array v(xmlrpc_c::value_array(req.getResult()));
    std::vector<xmlrpc_c::value> vv = v.vectorValueValue();
    size_t j = 0;
    size_t i = 0;
    const size_t old_size = old.size();
    const size_t new_size = vv.size();

    std::vector<std::shared_ptr<Torrent> > ts;
    ts.reserve(vv.size());
    for(const xmlrpc_c::value &tv : vv){
        ts.push_back(parse_torrent(tv));
    }

    // the list must be ordered by hash for the comparison with the previous list to work
    std::sort(ts.begin(), ts.end(), [](const std::shared_ptr<Torrent> &a,
                const std::shared_ptr<Torrent> &b){
            return a->hash < b->hash;
            });
    
    insertLoop(ts, old_size, new_size, j, i);

    if(j != old_size){
        listener.torrents_removed(j, old_size);
    } else if(i != new_size){
        listener.torrents_inserted(i, {ts.begin()+i, ts.end()});
    }

    old = ts;
    //sched.reschedule();
}
