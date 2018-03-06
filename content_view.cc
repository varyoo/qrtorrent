#include"content_view.h"


content_view::content_view(QWidget *parent):
    QTreeView(parent)
{
    setModel(&model);
    setAllColumnsShowFocus(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //setContextMenuPolicy(Qt::CustomContextMenu);
}


