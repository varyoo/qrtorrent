#include"file_model.h"

#include"client_tpl.cc"


template std::unique_ptr<files_client<file_model_t> >
Client::files<file_model_t>(rtorrent&, const QString&);
