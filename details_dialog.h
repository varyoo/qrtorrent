#ifndef DETAILS_DIALOG_H
#define DETAILS_DIALOG_H

#include <QDialog>
#include <QMetaObject>

#include "files_daemon.h"


namespace Ui {
class details_dialog;
}

class details_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit details_dialog(QWidget *parent, files_daemon&);
    ~details_dialog();

private:
    Ui::details_dialog *ui;
    files_daemon &daemon;

signals:
    void fetch_run_once();
};

#endif // DETAILS_DIALOG_H
