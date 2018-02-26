#include"removedialog.h"
#include"ui_removedialog.h"
#include<QDebug>

RemoveDialog::RemoveDialog(QWidget *parent, QStringList hs) :
    QDialog(parent),
    ui(new Ui::RemoveDialog),
    hs(hs)
{
    ui->setupUi(this);

    QString s;
    ui->label->setText(s.sprintf(
                "Are you sure to delete %d torrent(s)?", hs.size()));
}

void RemoveDialog::accept(){
    if(hs.size() == 0){
        qWarning() << "Remove 0 torrents";
        return;
    }

    emit torrentsRemoved(hs, false);
    QDialog::accept();
}

RemoveDialog::~RemoveDialog()
{
    delete ui;
}
