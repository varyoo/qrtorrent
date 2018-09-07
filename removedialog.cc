#include <QDebug>

#include "removedialog.h"
#include "ui_removedialog.h"


RemoveDialog::RemoveDialog(QWidget *parent, const std::vector<std::string> &hs) :
    QDialog(parent),
    ui(new Ui::RemoveDialog),
    hashes(hs)
{
    ui->setupUi(this);

    QString s;
    ui->label->setText(s.sprintf(
                "Are you sure to delete %ld torrent(s)?", hs.size()));
}

RemoveDialog::~RemoveDialog()
{
    delete ui;
}
