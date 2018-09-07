#ifndef REMOVEDIALOG_H
#define REMOVEDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveDialog;
}

class RemoveDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::RemoveDialog *ui;
    const std::vector<std::string> &hashes;

public:
    explicit RemoveDialog(QWidget *parent, const std::vector<std::string> &hs);
    ~RemoveDialog();
};

#endif // REMOVEDIALOG_H
