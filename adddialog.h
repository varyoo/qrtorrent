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
    explicit AddDialog(QWidget *parent, QStringList fs);
    ~AddDialog();

private:
    Ui::AddDialog *ui;
    QStringList files;
    Config conf{};

private slots:
    void accept();

signals:
    void filesAdded(QString, QStringList, bool);
};

#endif // ADDDIALOG_H
