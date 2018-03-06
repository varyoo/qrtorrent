#include"torrent_files_model.h"
#include<QDebug>


torrent_files_model::torrent_files_model():
    files(),
    placeholder_root(torrent_file({}, ".")),
    root(&placeholder_root)
{
}

QModelIndex torrent_files_model::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent)){
        return QModelIndex();
    }

    file_model_t *folder;

    if(!parent.isValid()){
        folder = root;
    } else {
        folder = static_cast<file_model_t*>(parent.internalPointer());
    }

    file_model_t *file = folder->child(row);
    if(file){
        return createIndex(row, column, file);
    } else {
        return QModelIndex();
    }
}

QModelIndex torrent_files_model::parent(const QModelIndex &child) const
{
    if(!child.isValid()){
        return QModelIndex();
    }

    file_model_t *file = static_cast<file_model_t*>(child.internalPointer());
    file_model_t *folder = file->get_parent();

    if(folder == root){
        return QModelIndex();
    }

    return createIndex(folder->row(), 0, folder);
}

/*QModelIndex torrent_files_model::sibling(int row, int column, const QModelIndex &idx) const
{

}*/

int torrent_files_model::rowCount(const QModelIndex &parent) const
{
    file_model_t *folder;

    if(!parent.isValid()){
        folder = root;
    } else {
        folder = static_cast<file_model_t*>(parent.internalPointer());
    }

    return folder->child_count();
}

int torrent_files_model::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return num_columns;
}

bool torrent_files_model::hasChildren(const QModelIndex &parent) const
{
    if(!hasIndex(parent.row(), parent.column(), parent)){
        return QAbstractItemModel::hasChildren(parent);
    }

    file_model_t *folder;

    if(!parent.isValid()){
        folder = root;
    } else {
        folder = static_cast<file_model_t*>(parent.internalPointer());
    }

    return folder->child_count() > 0;
}

QVariant torrent_files_model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    file_model_t& f = *static_cast<file_model_t*>(index.internalPointer());

    switch(role){
    case Qt::DisplayRole:
        switch(index.column()){
        case col_basename: return f.get_file().basename;
        }
    }

    return QVariant();
}

QVariant torrent_files_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole){
        switch(section){
        case col_basename: return "Filename";
        }
    }

    return QVariant();
}

void torrent_files_model::set_files(std::vector<std::shared_ptr<file_model_t> > fs,
            file_model_t *r)
{
    Q_ASSERT(files.size() == 0);
    Q_ASSERT(root == &placeholder_root);

    beginInsertRows(QModelIndex(), 0, fs.size()-1);
    files = fs;
    root = r;
    endInsertRows();
}
