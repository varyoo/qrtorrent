#ifndef FILES_MODEL_H
#define FILES_MODEL_H

#include"torrent_file.h"
#include<QList>
#include<QDebug>


class file_model_t {
private:
    torrent_file file;
    file_model_t *parent;
    QList<file_model_t*> childs;
public:
    file_model_t(const torrent_file &tf);
    void set_parent(file_model_t*);
    void add_child(file_model_t*);
    inline file_model_t *child(int row){
        return childs.value(row);
    }
    inline int child_count(){
        return childs.count();
    }
    int row();
    inline file_model_t *get_parent(){
        return parent;
    }
    inline const torrent_file &get_file(){
        return file;
    }
    inline file_model_t *last_child(){
        if(childs.count() == 0){
            return nullptr;
        }
        
        return childs.value(childs.count()-1);
    }
};

#endif // FILES_MODEL_H
