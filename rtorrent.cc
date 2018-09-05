#include "rtorrent.h"
#include <QDebug>
#include <QApplication>


rtorrent::rtorrent():
    transport(),
    client(&transport)
{
    xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 5e6);
    setConnection();
}

void rtorrent::updateConnection(){
    delete cp;
    setConnection();
}

void rtorrent::call(xmlrpc_c::rpc &req){
    Q_ASSERT(this->thread() != QApplication::instance()->thread());
    
    req.call(&client, cp);
}

void rtorrent::cmdForHashes(std::string cmd, QStringList hashs)
{
    xmlrpc_c::carray cmds;
    for(const QString h : hashs){
        xmlrpc_c::carray ps = {xmlrpc_c::value_string(h.toStdString())};
        addCmd(cmds, cmd, ps);
    }

    xmlrpc_c::paramList ps;
    ps.add(xmlrpc_c::value_array(cmds));

    xmlrpc_c::rpc req("system.multicall", ps);
    call(req);
}

void rtorrent::addCmd(xmlrpc_c::carray &cmds, std::string cmd, xmlrpc_c::carray &ps){
    cmds.push_back(xmlrpc_c::value_struct({
        {"methodName", xmlrpc_c::value_string(cmd)},
        {"params", xmlrpc_c::value_array(ps)}
    }));
}
