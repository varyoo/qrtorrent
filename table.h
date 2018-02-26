#ifndef TABLE2_H
#define TABLE2_H

#include"client.h"
#include<QVariant>
#include<memory>
#include<QItemDelegate>
#include<QTreeView>
#include"tablesortmodel.h"
#include"tablemodel.h"


class TableCell : public QItemDelegate {
private:
    TableSortModel &sortModel;

private:
    static inline QString printRate(long int r, QLocale l = QLocale()){
        return QString("%1/s").arg(printSize(r, l));
    }
    static inline QString printSize(long int s, QLocale l = QLocale()){
        return l.formattedDataSize(s, 2, QLocale::DataSizeSIFormat);
    }

public:
    TableCell(TableSortModel &m, QObject *parent = 0);
    ~TableCell(){}
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
};

class Table : public QTreeView {
Q_OBJECT

private:
    TableModel model;
    TableSortModel sortModel;
    TableCell delegate;

private:
    QStringList selectedHashes();
    QModelIndex mapToSource(const QModelIndex &index) const;
    QModelIndex mapFromSource(const QModelIndex &index) const;

private slots:
    void startTorrents();
    void stopTorrents();
    void aboutToRemoveTorrents();

public:
    Table(QWidget *parent);
    ~Table(){}

public slots:
    inline void changeTorrent(int i, std::shared_ptr<Torrent> t){
        model.changeTorrent(i, t);
    }
    inline void insertTorrents(int i, std::vector<std::shared_ptr<Torrent> > ts){
        model.insertTorrents(i, ts);
    }
    inline void removeTorrents(int from, int to){
        model.removeTorrents(from, to);
    }
    
    void showMenu(QPoint);
    void apply_name_filter(const QString& name);

signals:
    void torrentsStarted(QStringList hashs);
    void torrentsStopped(QStringList hashs);
    void torrentsRemoved(QStringList hs, bool deleteData);
};

#endif // TABLE2_H
