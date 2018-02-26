#ifndef REMOVEDIALOG_H
#define REMOVEDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveDialog;
}

class RemoveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveDialog(QWidget *parent, QStringList hs);
    ~RemoveDialog();

private:
    Ui::RemoveDialog *ui;
    QStringList hs;

private slots:
    void accept();

signals:
    void torrentsRemoved(QStringList, bool);
};

#endif // REMOVEDIALOG_H
