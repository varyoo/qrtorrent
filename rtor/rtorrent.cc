#include <sstream>
#include <fstream>
#include <iterator>

#include "rtorrent.h"


namespace rtor {

void client::cmd_for_hashes(std::vector<std::string> hashes, std::string cmd)
{
    xmlrpc_c::carray cmds;
    for(const auto &h : hashes){
        xmlrpc_c::carray ps = {xmlrpc_c::value_string(h)};
        append_cmd(cmds, cmd, ps);
    }

    xmlrpc_c::paramList ps;
    xmlrpc_c::rpc req("system.multicall", ps.addc(cmds));
    call(req);
}

void client::append_cmd(xmlrpc_c::carray &cmds, const std::string &cmd, const xmlrpc_c::carray &ps){
    cmds.push_back(xmlrpc_c::value_struct({
        {"methodName", xmlrpc_c::value_string(cmd)},
        {"params", xmlrpc_c::value_array(ps)}
    }));
}

// https://stackoverflow.com/a/5342138
std::string client::escape_shell_arg(std::string const &s)
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

void client::move_data(std::vector<std::string> hashes, const std::string &dest_path)
{
    xmlrpc_c::carray cmds;

    for(const auto &hash : hashes){
        xmlrpc_c::carray params = {
            xmlrpc_c::value_string(hash)
        };

        append_cmd(cmds, "d.directory", params);
    }

    xmlrpc_c::paramList call_params;
    xmlrpc_c::rpc req("system.multicall", call_params.addc(cmds));
    call(req);

    auto res = xmlrpc_c::value_array(req.getResult()).vectorValueValue();

    // build shell script
    std::stringstream sh;

    for(xmlrpc_c::value &line_val : res){
        auto torrent_val = xmlrpc_c::value_array(line_val).vectorValueValue();

        xmlrpc_c::value_string src_dir(torrent_val[0]);

        sh << "mv '" << escape_shell_arg(src_dir) << "' '" <<
            escape_shell_arg(dest_path) << "'&&";
    }

    sh << "exit 0";

    xmlrpc_c::paramList mv_params;
    mv_params.addc("sh").addc("-c").addc(sh.str());

    // execute script on remote computer
    xmlrpc_c::rpc sh_req("execute", mv_params);
    call(sh_req);
}

void client::move_downloads(std::vector<std::string> hashes, std::string dest, bool also_move_data)
{
    // 1: close torrents
    cmd_for_hashes(hashes, "d.close");
    
    // 2: move downloads or abort
    if(also_move_data){
        move_data(hashes, dest);
    }
    
    xmlrpc_c::carray cmds;
    const xmlrpc_c::value_string path_value(dest);
    
    for(const auto &hash : hashes){
        xmlrpc_c::value_string hash_value(hash);
        xmlrpc_c::carray params = {hash_value, path_value};

        append_cmd(cmds, "d.directory.set", params);
    }

    xmlrpc_c::paramList call_params;
    call_params.addc(cmds);
    
    xmlrpc_c::rpc req("system.multicall", call_params);
    call(req);

    // 3: open torrents
    cmd_for_hashes(hashes, "d.open");
}

// from https://stackoverflow.com/questions/15138353
void client::load_file(xmlrpc_c::cbytestring &bytes, const std::string &filename){
    // open the file:
    std::ifstream file(filename, std::ios::binary);

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


void client::add_files(std::vector<std::string> files, std::string dest, bool start)
{
    const xmlrpc_c::value_string target("");
    xmlrpc_c::carray cmds;

    // used by ruTorrent and Flood
    std::time_t epoch = std::time(nullptr);
    std::stringstream ss;
    ss << "d.custom.set=addtime," << epoch;
    const xmlrpc_c::value_string addtime(ss.str());

    int size = 0;
    
    for(const auto &filename : files){
        xmlrpc_c::cbytestring bytes;
        load_file(bytes, filename);

        xmlrpc_c::value_bytestring v(bytes);
        xmlrpc_c::carray load_params = {target, v, addtime};

        if(!dest.empty()){
            load_params.push_back(xmlrpc_c::value_string("d.directory.set="+dest) );
        }

        std::string cmd;
        if(start){
            cmd = "load.raw_start";
        } else {
            cmd = "load.raw";
        }

        size += bytes.size();
        if(size > 300*1000){ // 524 kilobytes supposedly max
            //qDebug() << "Add files: breaking down big multicall";
            
            xmlrpc_c::paramList call_params;
            xmlrpc_c::rpc req("system.multicall", call_params.addc(cmds));
            call(req);
            
            cmds.clear();
            size = 0;
        }
        
        append_cmd(cmds, cmd, load_params);
    }

    xmlrpc_c::paramList call_params;
    xmlrpc_c::rpc req("system.multicall", call_params.addc(cmds));
    call(req);
}

} // rtor
