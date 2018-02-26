#include"connect.h"
#include"ui_connect.h"
#include<QDebug>

Connect::Connect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connect)
{
    ui->setupUi(this);
    
    ui->address->insert(conf.url());
    ui->user->insert(conf.user());
    ui->password->insert(conf.password());
}

Connect::~Connect()
{
    delete ui;
}

void Connect::accept(){
    conf.setRemote(
            ui->address->text(),
            ui->user->text(),
            ui->password->text()
            );
    emit connectionChanged();
    QDialog::accept();
}
