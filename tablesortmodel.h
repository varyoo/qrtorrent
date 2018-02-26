#ifndef TABLESORTMODEL_H
#define TABLESORTMODEL_H

#include<QSortFilterProxyModel>
#include"tablemodel.h"


class TableSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

private:
    TableModel model;

public:
    TableSortModel(QObject *parent = 0);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // TABLESORTMODEL_H
