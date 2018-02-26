#ifndef TORRENTSMODEL_H
#define TORRENTSMODEL_H

#include<QAbstractTableModel>
#include<memory>
#include"client.h"

class TableModel : public QAbstractListModel {
    public:
        enum Column
        {
            TR_NAME,
            TR_PROGRESS,
            TR_DOWNLOADED,
            TR_DOWN_SPEED,
            TR_UPLOADED,
            TR_UP_SPEED,
            TR_RATIO,
            TR_ADDED,

            NB_COLUMNS
        };
    private:
        std::vector<std::shared_ptr<Torrent>> torrents;
    public:
        TableModel(QObject *parent);
        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        QModelIndex index(int row, int column = 0,
                const QModelIndex &parent = QModelIndex()) const;
        QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
        Qt::ItemFlags flags(const QModelIndex&) const;
        bool dropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&);

        void changeTorrent(int, std::shared_ptr<Torrent>);
        void insertTorrents(int, std::vector<std::shared_ptr<Torrent> >);
        void removeTorrents(int, int);
};

#endif // TORRENTSMODEL_H
