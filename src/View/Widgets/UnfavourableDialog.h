#ifndef UNFAVOURABLEDIALOG_H
#define UNFAVOURABLEDIALOG_H

#include <QDialog>
#include <set>

namespace Ui {
class UnfavourableDialog;
}

class UnfavourableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UnfavourableDialog(QWidget *parent = nullptr);
    ~UnfavourableDialog();

private:
    Ui::UnfavourableDialog *ui;
};

#endif // UNFAVOURABLEDIALOG_H
