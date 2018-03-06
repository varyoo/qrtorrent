#ifndef TORRENT_FILES_MODEL
#define TORRENT_FILES_MODEL


#include<QAbstractItemModel>
#include"file_model.h"
#include<memory>


class torrent_files_model : public QAbstractItemModel {
Q_OBJECT
public:
    enum column {
        col_basename,

        num_columns
    };
private:
    std::vector<std::shared_ptr<file_model_t> > files;
    file_model_t placeholder_root;
    file_model_t* root;

public:
    torrent_files_model();

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    //QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void set_files(std::vector<std::shared_ptr<file_model_t> > files,
            file_model_t *root);
};



#endif // TORRENT_FILES_MODEL
