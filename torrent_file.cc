#include"torrent_file.h"
#include<QDebug>


torrent_file::torrent_file(filepath_t &&path, QString &&basename):
    path(path),
    basename(basename)
{
}
