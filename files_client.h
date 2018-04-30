#ifndef FILES_CLIENT_H
#define FILES_CLIENT_H

#include<vector>
#include<memory>
#include<xmlrpc-c/girerr.hpp>
#include<xmlrpc-c/base.hpp>
#include<xmlrpc-c/client.hpp>
#include"config.h"
#include"rtorrent.h"
#include"scheduler.h"
#include"torrent_file.h"


template<class file_model_t>
class files_client
{
public:
    files_client(rtorrent&, const QString &hash);
    file_model_t *fetch_into(std::vector<std::shared_ptr<file_model_t> > &files);
private:
    const QString hash;
    Config conf{};
    rtorrent &rtor;
    std::vector<std::shared_ptr<file_model_t> > image;
};

#endif // FILES_CLIENT_H