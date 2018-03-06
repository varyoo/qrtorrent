#ifndef DETAILS_DIALOG_H
#define DETAILS_DIALOG_H

#include <QDialog>
#include"files_daemon.h"
#include"focus.h"
#include<QMetaObject>


namespace Ui {
class details_dialog;
}

class details_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit details_dialog(QWidget *parent, files_daemon_t&);
    ~details_dialog();

private:
    Ui::details_dialog *ui;
    files_daemon_t &daemon;
    focus_t focus;
    QMetaObject::Connection run_fetch;

    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    void done(int r);

signals:
    void focus_gained();
    void focus_lost();
};

#endif // DETAILS_DIALOG_H
