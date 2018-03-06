#include"file_model.h"

file_model_t::file_model_t(const torrent_file &tf):
    file(tf)
{}

void file_model_t::set_parent(file_model_t* p)
{
    parent = p;
}

void file_model_t::add_child(file_model_t* c)
{
    childs.append(c);
}

int file_model_t::row()
{
    return parent->childs.indexOf(const_cast<file_model_t*>(this));
}
