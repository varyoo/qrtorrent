#include "mainwindow.h"
#include <QApplication>
#include "table.h"
#include "torrent_list.h"
#include "app.h"

int main(int argc, char *argv[])
{
    /*QLocale l;
    l.setNumberOptions(QLocale::DefaultNumberOptions);
    QLocale::setDefault(l);*/

    App a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
