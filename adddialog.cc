#include "adddialog.h"
#include "ui_adddialog.h"
#include<QDebug>

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);

    ui->destination->setText(conf.defaultDest());
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::accept(){
    dest_path = ui->destination->text();
    if(ui->setDefault->checkState() == Qt::Checked){
        conf.setDefaultDest(dest_path);
    }

    if(dest_path.isEmpty()){
        qDebug() << "Add torrents: destination path is empty";
    }

    start_torrents = ui->start->checkState() == Qt::Checked;
    
    QDialog::accept();
}
