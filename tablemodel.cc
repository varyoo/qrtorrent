#include "tablemodel.h"
#include <QColor>
#include <QDebug>


TableModel::TableModel(QObject *parent):
    QAbstractListModel(parent)
{}

int TableModel::rowCount(const QModelIndex&) const {
    return torrents.size();
}

int TableModel::columnCount(const QModelIndex&) const {
    return NB_COLUMNS;
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    Q_ASSERT(index.model() == this);
    if(!index.isValid()){
        return QVariant();
    }
    
    const Torrent *t = static_cast<Torrent*>(index.internalPointer());

    switch(role){
    case Qt::ForegroundRole:
        switch(index.column()){
            case TableModel::TR_NAME:
                switch(t->display_state){
                case Torrent::State::Checking:
                    return QColor(0, 128, 128); // Teal
                case Torrent::State::Downloading:
                    return QColor(34, 139, 34); // Forest Green
                case Torrent::State::Seeding:
                    //return QColor(65, 105, 225); // Royal Blue
                    return QVariant();
                case Torrent::State::Stopped:
                    return QColor(250, 128, 114); // Salmon
                default:
                    Q_ASSERT(false);
                    return QVariant();
                }
                break;

            case TableModel::TR_DOWN_SPEED:
            case TableModel::TR_UP_SPEED:
                if(data(index, Qt::DisplayRole).toBool()){
                    break;
                }
                // fallthrough

            default:
                return QColor(Qt::darkGray);
        }

    case Qt::DisplayRole:
        switch(index.column()){
        case TableModel::TR_NAME: return t->name;
        case TableModel::TR_PROGRESS: return t->percent();
        case TableModel::TR_DOWNLOADED: return t->downTotal;
        case TableModel::TR_UPLOADED: return t->upTotal;
        case TableModel::TR_DOWN_SPEED: return t->downRate;
        case TableModel::TR_UP_SPEED: return t->upRate;
        case TableModel::TR_RATIO: return t->reported_ratio();
        case TableModel::TR_ADDED: return t->addtime;
        default:
            qWarning("data: invalid display value column: %d", index.column());
            break;
        }
        break;
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Horizontal){
        if(role == Qt::DisplayRole){
            switch(section){
            case TR_NAME: return "Name";
            case TR_PROGRESS: return "Progress";
            case TR_DOWNLOADED: return "Downloaded";
            case TR_DOWN_SPEED: return "Download speed";
            case TR_UPLOADED: return "Uploaded";
            case TR_UP_SPEED: return "Upload speed";
            case TR_RATIO: return "Ratio";
            case TR_ADDED: return "Added";
            default: return QVariant();
            }
        }
    }
    return QVariant();
}

QModelIndex TableModel::sibling(int row, int column, const QModelIndex &idx) const {
    if(row == idx.row() && column < NB_COLUMNS){
        return createIndex(row, column, idx.internalPointer());
    } else {
        // there is something to do here
        return QAbstractItemModel::sibling(row, column, idx);
    }
}

QModelIndex TableModel::index(int row, int column, const QModelIndex &parent) const {
    if(row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent)){
        return QModelIndex();
    }

    auto p = torrents[row].get();
    return createIndex(row, column, p);
}

void TableModel::changeTorrent(int i, std::shared_ptr<Torrent> t){
    torrents[i] = t;
    void *const p = t.get();
    emit dataChanged(createIndex(i, 0, p), createIndex(i, NB_COLUMNS-1, p) );
}

void TableModel::insertTorrents(int i, std::vector<std::shared_ptr<Torrent>> ts){
    if(ts.size() == 0){
        qWarning() << "model: insert 0 torrents";
        return;
    }
    beginInsertRows(QModelIndex(), i, i+ts.size()-1);
    torrents.insert(torrents.begin()+i, ts.begin(), ts.end());
    endInsertRows();
}

void TableModel::removeTorrents(int from, int to){
    beginRemoveRows(QModelIndex(), from, to-1);
    torrents.erase(torrents.begin()+from, torrents.begin()+to);
    endRemoveRows();
}

Qt::ItemFlags TableModel::flags(const QModelIndex&) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool TableModel::dropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&){
    return true;
}
