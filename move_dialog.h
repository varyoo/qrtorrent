#ifndef MOVE_DIALOG_H
#define MOVE_DIALOG_H

#include <QDialog>

namespace Ui {
class move_dialog;
}

class move_dialog : public QDialog
{
    Q_OBJECT

public:
    bool move_data;
    QString dest_path;

public:
    explicit move_dialog(QWidget *parent = nullptr);
    ~move_dialog();

private:
    Ui::move_dialog *ui;

private slots:
    void accept();
};

#endif // MOVE_DIALOG_H
