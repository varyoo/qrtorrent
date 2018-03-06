#ifndef CONTENT_VIEW_H
#define CONTENT_VIEW_H

#include<QVariant>
#include<memory>
#include<QItemDelegate>
#include<QTreeView>
#include"torrent_files_model.h"


class content_view : public QTreeView {
Q_OBJECT

private:
    torrent_files_model model;

public:
    content_view(QWidget *parent);
    ~content_view(){}
    inline torrent_files_model &content_model(){
        return model;
    }
};

#endif // CONTENT_VIEW_H
