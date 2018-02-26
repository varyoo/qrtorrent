#include"mainwindow.h"
#include<QApplication>
#include"table.h"
#include"client.h"

int main(int argc, char *argv[])
{
    /*QLocale l;
    l.setNumberOptions(QLocale::DefaultNumberOptions);
    QLocale::setDefault(l);*/

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
