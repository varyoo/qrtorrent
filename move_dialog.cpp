#include "move_dialog.h"
#include "ui_move_dialog.h"

move_dialog::move_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::move_dialog)
{
    ui->setupUi(this);
}

move_dialog::~move_dialog()
{
    delete ui;
}

void move_dialog::accept(){
    move_data = ui->move_data->checkState() == Qt::Checked;
    dest_path = ui->dest_path->text();
    
    QDialog::accept();
}
