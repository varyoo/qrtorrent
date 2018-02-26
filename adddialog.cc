#include "adddialog.h"
#include "ui_adddialog.h"
#include<QDebug>

AddDialog::AddDialog(QWidget *parent, QStringList fs) :
    QDialog(parent),
    ui(new Ui::AddDialog),
    files(fs)
{
    ui->setupUi(this);

    ui->destination->setText(conf.defaultDest());
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::accept(){
    QString path = ui->destination->text();
    if(ui->setDefault->checkState() == Qt::Checked){
        conf.setDefaultDest(path);
    }

    if(path.isEmpty()){
        qDebug() << "Add torrents: destination path is empty";
    }

    bool start = ui->start->checkState() == Qt::Checked;
    emit filesAdded(path, files, start);
    QDialog::accept();
}
