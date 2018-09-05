#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include"config.h"

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    QString dest_path;
    bool start_torrents;

public:
    explicit AddDialog(QWidget *parent);
    ~AddDialog();

private:
    Ui::AddDialog *ui;
    Config conf{};

private slots:
    void accept();
};

#endif // ADDDIALOG_H
