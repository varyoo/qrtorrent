#include <QThread>
#include <QProgressBar>
#include <QItemDelegate>
#include <QStyleOptionProgressBar>
#include <QApplication>
#include <QPainter>
#include <cmath>
#include <QStyleOptionViewItem>
#include <QDateTime>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>

#include "table.h"
#include "move_dialog.h"
#include "removedialog.h"


TableCell::TableCell(TableSortModel &m, QObject *parent):
    QItemDelegate(parent),
    sortModel(m)
{
}

QWidget *TableCell::createEditor(QWidget*, const QStyleOptionViewItem&,
         const QModelIndex&) const{
    return nullptr;
}

void TableCell::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    // get the correct internal pointer from source model
    const QModelIndex &idx = sortModel.mapToSource(index);
    const Torrent* t = static_cast<Torrent*>(idx.internalPointer());

    QStyleOptionViewItem opt = QItemDelegate::setOptions(index, option);
    
    QItemDelegate::drawBackground(painter, opt, index);

    switch(index.column()){
    case TableModel::TR_NAME:
        {
            opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
            
            /*QString prefix;
            if(t->display_state != Torrent::State::Stopped){
                prefix = "✔ ";
            } else {
                prefix = "✘ ";
            }*/

            QItemDelegate::drawDisplay(painter, opt, option.rect, t->name);
        }
        break;
    case TableModel::TR_PROGRESS:
        {
            QStyleOptionProgressBar pbo;
            pbo.state = opt.state;
            pbo.rect = opt.rect;
            pbo.minimum = 0;
            pbo.maximum = 100;
            pbo.progress = t->percent();
            pbo.state |= QStyle::State_Enabled;
            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &pbo, painter);
        }
        break;
    case TableModel::TR_DOWNLOADED:
    case TableModel::TR_UPLOADED:
        opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        QItemDelegate::drawDisplay(painter, opt, option.rect,
                printSize(index.data().toLongLong()));
        break;
    case TableModel::TR_DOWN_SPEED:
    case TableModel::TR_UP_SPEED:
        opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        QItemDelegate::drawDisplay(painter, opt, option.rect,
                printRate(index.data().toLongLong()));
        break;
    case TableModel::TR_RATIO:
        {
            opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
            float r = t->reported_ratio();
            QString display;
            if(std::isnan(r) || std::isinf(r)){
                display = "∞";
            } else {
                display = QString::number(r, 'f', 1);
            }
            QItemDelegate::drawDisplay(painter, opt, option.rect, display);
        }
        break;
    case TableModel::TR_ADDED:
        {
            opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
            if(t->addtime == 0){
                QItemDelegate::drawDisplay(painter, opt, option.rect, "Unknown");
            } else {
                QDateTime dt;
                dt.setTime_t(t->addtime);
                QString display = dt.date().toString(Qt::DefaultLocaleShortDate);
                QItemDelegate::drawDisplay(painter, opt, option.rect, display);
            }
        }
        break;
    default:
        QItemDelegate::paint(painter, option, index);
    }

}

Table::Table(QWidget *parent):
    QTreeView(parent),
    model(this),
    sortModel(this),
    delegate(sortModel, this)
{
    setUniformRowHeights(true);
    setRootIsDecorated(false);
    setAllColumnsShowFocus(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setItemsExpandable(false);
    setAutoScroll(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegate(&delegate);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSortingEnabled(true);

    // TODO: save and restore the last column sorted
    sortByColumn(TableModel::TR_ADDED, Qt::DescendingOrder);

    sortModel.setDynamicSortFilter(true);
    sortModel.setFilterKeyColumn(TableModel::TR_NAME);
    sortModel.setFilterRole(Qt::DisplayRole);
    sortModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    sortModel.setSourceModel(&model);
    
    setModel(&sortModel); 

    // TODO: save & restore column sizes
    header()->resizeSection(TableModel::TR_NAME, 500);
    header()->setStretchLastSection(false);
    
    connect(this, &Table::customContextMenuRequested,
            this, &Table::showMenu);

    bool ok = connect(this, &QAbstractItemView::doubleClicked,
            this, &Table::show_details);
    Q_ASSERT(ok);
}

void Table::apply_name_filter(const QString& name){
    sortModel.setFilterRegExp(QRegExp(name, Qt::CaseInsensitive, QRegExp::WildcardUnix));
}

inline QModelIndex Table::mapToSource(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());
    if(index.model() == &sortModel)
        return sortModel.mapToSource(index);
    return index;
}

inline QModelIndex Table::mapFromSource(const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.model() == &sortModel);
    return sortModel.mapFromSource(index);
}

void Table::showMenu(QPoint p)
{
    QModelIndex idx = indexAt(p);
    if(!idx.isValid()){
        return;
    }

    QMenu m(this);
    
    QAction start("Start");
    m.connect(&start, &QAction::triggered, this, &Table::startTorrents);

    QAction stop("Stop");
    m.connect(&stop, &QAction::triggered, this, &Table::stopTorrents);

    QAction remove("Delete");
    m.connect(&remove, &QAction::triggered, this, &Table::aboutToRemoveTorrents);

    QAction move("Set download location");
    m.connect(&move, &QAction::triggered, this, &Table::show_move_dialog);

    m.addAction(&start); // does not take ownership
    m.addAction(&stop);
    m.addSeparator();

    m.addAction(&move);
    m.addSeparator();

    m.addAction(&remove);
    m.exec(QCursor::pos());
}

std::vector<std::string> Table::selectedHashes(){
    std::vector<std::string> hs;

    for(const QModelIndex &idx : selectionModel()->selectedRows()){
        const QModelIndex &src_idx = model.index(mapToSource(idx).row());
        hs.push_back(static_cast<Torrent*>(src_idx.internalPointer())->hash.toStdString());
    }

    return hs;
}

void Table::startTorrents(){
    emit torrentsStarted(selectedHashes());
}

void Table::stopTorrents(){
    emit torrentsStopped(selectedHashes());
}

void Table::aboutToRemoveTorrents(){
    auto hs = selectedHashes();
    RemoveDialog dialog(this, hs);
    
    dialog.exec();

    if(dialog.result() == QDialog::Accepted){
        emit torrentsRemoved(hs, false);
    }
}

void Table::show_details(const QModelIndex &idx)
{
    const QModelIndex &src_idx = model.index(mapToSource(idx).row());
    Torrent* t = static_cast<Torrent*>(src_idx.internalPointer());
    emit details_requested(t->hash);
}

void Table::show_move_dialog(){
    auto hashes = selectedHashes();
    move_dialog dialog(this);

    dialog.exec();

    if(dialog.result() == QDialog::Accepted){
        emit move_downloads(hashes, dialog.dest_path.toStdString(), dialog.move_data);
    }
}
