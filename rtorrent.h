#ifndef RTORRENT_H
#define RTORRENT_H

#include<xmlrpc-c/girerr.hpp>
#include<xmlrpc-c/base.hpp>
#include<xmlrpc-c/client.hpp>
#include"config.h"

class rtorrent : public QObject {
    Q_OBJECT

public:
    xmlrpc_c::clientXmlTransport_curl transport;
    xmlrpc_c::client_xml client;
    xmlrpc_c::carriageParm_curl0 *cp;
    Config conf{};

public:
    rtorrent();
    ~rtorrent(){
        delete cp;
    }
    void cmdForHashes(std::string, QStringList);
    void rescheduleFetchAll();
    inline void setConnection(){
        cp = new xmlrpc_c::carriageParm_curl0(conf.url().toStdString());
        cp->setBasicAuth(conf.user().toStdString(), conf.password().toStdString());
    }
    static void addCmd(xmlrpc_c::carray&, std::string, xmlrpc_c::carray&);
    bool call(std::string, xmlrpc_c::paramList);
    bool multicall(xmlrpc_c::carray);
    inline xmlrpc_c::carriageParm_curl0* carriageParm(){
        return cp;
    }
    inline void call_rpc(xmlrpc_c::rpcPtr c){
        c->call(&client, cp);
    }
    void updateConnection();
};

#endif // RTORRENT_H
