#ifndef RTORRENT_H
#define RTORRENT_H

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client.hpp>


namespace rtor {

class client {

public:
    xmlrpc_c::clientXmlTransport_curl transport;
    xmlrpc_c::client_xml xml_client;
    xmlrpc_c::carriageParm_curl0Ptr carriage;

public:
    client(xmlrpc_c::carriageParm_curl0Ptr carriage):
        transport(),
        xml_client(&transport),
        carriage(carriage)
    {
        xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 5e6);
    }

    void call(xmlrpc_c::rpc &req){
        req.call(&xml_client, carriage.get());
    }

    void cmd_for_hashes(std::vector<std::string> hashes, std::string cmd);
    
    void start_torrents(std::vector<std::string> hashes){
        cmd_for_hashes(hashes, "d.start");
    }
    
    void stop_torrents(std::vector<std::string> hashes){
        cmd_for_hashes(hashes, "d.stop");
    }
    
    void delete_torrents(std::vector<std::string> hashes){
        cmd_for_hashes(hashes, "d.erase");
    }

    void add_files(std::vector<std::string> files, std::string path, bool start); 
    void move_downloads(std::vector<std::string> hashes, std::string dest, bool move_data);

    static void append_cmd(xmlrpc_c::carray &commands_dest,
            const std::string &command,
            const xmlrpc_c::carray &command_params);
    
    static std::string escape_shell_arg(std::string const &shell_arg);

private:
    void move_data(std::vector<std::string> hashes, const std::string &to);
    void load_file(xmlrpc_c::cbytestring &bytes, const std::string &filename);
};

typedef std::shared_ptr<client> client_ptr;

} // rtor

#endif // RTORRENT_H
