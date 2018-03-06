#include"client.h"

template<class file_model_t>
std::unique_ptr<files_client<file_model_t> >
Client::files(rtorrent &rtor, const QString &hash)
{
    return std::make_unique<files_client<file_model_t> >(rtor, hash);
}
