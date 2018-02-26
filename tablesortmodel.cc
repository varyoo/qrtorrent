#include "tablesortmodel.h"
#include<QDebug>
#include<QCollator>
#include"table.h"
#include<cmath>

TableSortModel::TableSortModel(QObject *parent):
    QSortFilterProxyModel(parent),
    model(this)
{}

bool TableSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool TableSortModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const {
    Q_UNUSED(source_column);
    Q_UNUSED(source_parent);
    return true;    
}

bool TableSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    switch(sortColumn()){
    case TableModel::TR_NAME:
        {
            QCollator qc;
            return qc.compare(left.data().toString(), right.data().toString()) < 0;
        }
    case TableModel::TR_PROGRESS:
    case TableModel::TR_DOWNLOADED:
    case TableModel::TR_DOWN_SPEED:
    case TableModel::TR_UPLOADED:
    case TableModel::TR_UP_SPEED:
    case TableModel::TR_ADDED:
        return left.data().toLongLong() < right.data().toLongLong();
    case TableModel::TR_RATIO:
        {
            float l = left.data().toFloat();
            return !(std::isnan(l) || std::isinf(l)) && l < right.data().toFloat();
        }
    default:
        qWarning() << "sort on invalid column";
        return QSortFilterProxyModel::lessThan(left, right);
    }
}
