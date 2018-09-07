#ifndef CONNECT_H
#define CONNECT_H

#include<QDialog>
#include<QSettings>
#include"config.h"

namespace Ui {
class Connect;
}

class Connect : public QDialog
{
    Q_OBJECT

public:
    Connect(QWidget *parent = 0);
    ~Connect();

public slots:
    void accept();

private:
    Ui::Connect *ui;
    Config conf{};
};

#endif // CONNECT_H
