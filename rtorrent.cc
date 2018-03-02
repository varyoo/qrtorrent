#include"rtorrent.h"
#include<QDebug>
#include<QApplication>


rtorrent::rtorrent():
    transport(),
    client(&transport)
{
    setConnection();
}

void rtorrent::updateConnection(){
    delete cp;
    setConnection();
}

void rtorrent::cmdForHashes(std::string cmd, QStringList hashs)
{
    xmlrpc_c::carray cmds;
    for(const QString h : hashs){
        xmlrpc_c::carray ps = {xmlrpc_c::value_string(h.toStdString())};
        addCmd(cmds, cmd, ps);
    }
    multicall(cmds);
}

void rtorrent::addCmd(xmlrpc_c::carray &cmds, std::string cmd, xmlrpc_c::carray &ps){
    cmds.push_back(xmlrpc_c::value_struct({
        {"methodName", xmlrpc_c::value_string(cmd)},
        {"params", xmlrpc_c::value_array(ps)}
    }));
}

bool rtorrent::multicall(xmlrpc_c::carray cmds){
    xmlrpc_c::paramList ps;
    ps.add(xmlrpc_c::value_array(cmds));
    return call("system.multicall", ps);
}

bool rtorrent::call(std::string cmd, xmlrpc_c::paramList ps){
    Q_ASSERT(this->thread() != QApplication::instance()->thread());
    
    xmlrpc_c::rpcPtr c(cmd, ps);
    try {
        c->call(&client, cp);
    } catch(std::exception &e){
        qWarning() << e.what();
        return false;
    }
    Q_ASSERT(c->isFinished());
    return true;
}
