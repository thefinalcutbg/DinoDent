#ifndef SIGNATUREVIEWDIALOG_H
#define SIGNATUREVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class SignatureViewDialog;
}

class SignatureViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignatureViewDialog(const std::vector<unsigned char>& bitmap, const std::string& signature);
    ~SignatureViewDialog();

private:
    Ui::SignatureViewDialog *ui;
};

#endif // SIGNATUREVIEWDIALOG_H
