#include "torrent_list.h"
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <fstream>
#include <QDateTime>
#include <sstream>


template<typename listener_t>
torrent_list<listener_t>::torrent_list(rtorrent &rtor, listener_t &listener):
    rtor(rtor),
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
        fetchAllParams.add(xmlrpc_c::value_string(s));
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

    xmlrpc_c::rpc req("d.multicall2", fetchAllParams);
    rtor.call(req);

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

// from https://stackoverflow.com/questions/15138353
template<typename listener_t>
void torrent_list<listener_t>::loadFileInto(xmlrpc_c::cbytestring &bytes, QString filename){
    // open the file:
    std::ifstream file(filename.toStdString(), std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    bytes.reserve(fileSize);

    // read the data:
    bytes.insert(bytes.begin(),
           std::istream_iterator<unsigned char>(file),
           std::istream_iterator<unsigned char>());
}


template<typename listener_t>
void torrent_list<listener_t>::add_files(QString dest, QStringList files, bool start)
{
    const std::string std_dest = dest.toStdString();
    const xmlrpc_c::value_string target("");
    xmlrpc_c::carray cmds;
    const xmlrpc_c::value_string addtime(
        QString("d.custom.set=addtime,%1"). // used by ruTorrent and Flood
        arg(QDateTime::currentSecsSinceEpoch() ).
        toStdString()
        );
    int size = 0;
    
    for(const QString &filename : files){
        xmlrpc_c::cbytestring bytes;
        torrent_list::loadFileInto(bytes, filename);

        xmlrpc_c::value_bytestring v(bytes);
        xmlrpc_c::carray load_params = {target, v, addtime};

        if(!dest.isEmpty()){
            load_params.push_back(xmlrpc_c::value_string("d.directory.set="+std_dest) );
        }

        std::string cmd;
        if(start){
            cmd = "load.raw_start";
        } else {
            cmd = "load.raw";
        }

        size += bytes.size();
        if(size > 300*1000){ // 524 kilobytes supposedly max
            qDebug() << "Add files: breaking down big multicall";
            
            xmlrpc_c::paramList call_params;
            call_params.add(xmlrpc_c::value_array(cmds));
            xmlrpc_c::rpc req("system.multicall", call_params);
            rtor.call(req);
            
            cmds.clear();
            size = 0;
        }
        
        rtor.addCmd(cmds, cmd, load_params);
    }

    xmlrpc_c::paramList call_params;
    call_params.add(xmlrpc_c::value_array(cmds));
    xmlrpc_c::rpc req("system.multicall", call_params);
    rtor.call(req);
}

// https://stackoverflow.com/a/5342138
template<typename listener_t>
std::string torrent_list<listener_t>::escape_shell_args(std::string const &s)
{
    std::size_t n = s.length();
    std::string escaped;
    escaped.reserve(n * 2);        // pessimistic preallocation

    for (std::size_t i = 0; i < n; ++i) {
        if (s[i] == '\\' || s[i] == '\'')
            escaped += '\\';
        escaped += s[i];
    }
    return escaped;
}

template<typename listener_t>
void torrent_list<listener_t>::move_data(QStringList hashes, QString dest_path){
    xmlrpc_c::carray cmds;

    for(QString &hash : hashes){
        xmlrpc_c::carray params = {
            xmlrpc_c::value_string(hash.toStdString())
        };

        rtor.addCmd(cmds, "d.directory", params);
    }

    xmlrpc_c::paramList callParams;
    callParams.add(xmlrpc_c::value_array(cmds));
    
    xmlrpc_c::rpc req("system.multicall", callParams);
    req.call(&rtor.client, rtor.cp);

    auto res = xmlrpc_c::value_array(req.getResult()).vectorValueValue();

    // build shell script
    std::stringstream sh;

    for(xmlrpc_c::value &line_val : res){
        auto torrent_val = xmlrpc_c::value_array(line_val).vectorValueValue();

        xmlrpc_c::value_string src_dir(torrent_val[0]);

        sh << "mv '" << escape_shell_args(src_dir) << "' '" <<
            escape_shell_args(dest_path.toStdString()) << "'&&";
    }

    sh << "exit 0";

    xmlrpc_c::paramList mvParams;
    mvParams.add(xmlrpc_c::value_string("sh"));
    mvParams.add(xmlrpc_c::value_string("-c"));
    mvParams.add(xmlrpc_c::value_string(sh.str()));

    // execute script on remote computer
    xmlrpc_c::rpc sh_req("execute", mvParams);
    rtor.call(sh_req);
}

template<typename listener_t>
void torrent_list<listener_t>::move_downloads(QString dest, QStringList hashes, bool also_move_data)
{
    // 1: close torrents
    rtor.cmdForHashes("d.close", hashes);
    
    // 2: move downloads or abort
    if(also_move_data){
        torrent_list::move_data(hashes, dest);
    }
    
    xmlrpc_c::carray cmds;
    const xmlrpc_c::value_string path_value(dest.toStdString());
    
    for(const QString &hash : hashes){
        xmlrpc_c::value_string hash_value(hash.toStdString());
        xmlrpc_c::carray params = {hash_value, path_value};

        rtor.addCmd(cmds, "d.directory.set", params);
    }

    xmlrpc_c::paramList callParams;
    callParams.add(xmlrpc_c::value_array(cmds));
    xmlrpc_c::rpc req("system.multicall", callParams);
    rtor.call(req);

    // 3: open torrents
    rtor.cmdForHashes("d.open", hashes);
}
