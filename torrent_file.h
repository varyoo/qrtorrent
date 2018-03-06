#ifndef TORRENT_FILE_H
#define TORRENT_FILE_H

#include<vector>
#include<xmlrpc-c/base.hpp>
#include<QString>


typedef std::vector<std::string> filepath_t;

struct torrent_file {
    const filepath_t path;
    const QString basename;
    torrent_file(filepath_t &&path, QString &&basename);
};

#endif // TORRENT_FILE_H
