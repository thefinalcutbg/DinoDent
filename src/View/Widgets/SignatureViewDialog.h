#ifndef SIGNATUREVIEWDIALOG_H
#define SIGNATUREVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class SignatureViewDialog;
}

class SignatureViewDialog : public QDialog
{
    Q_OBJECT

	bool m_for_removal = false;

public:
    explicit SignatureViewDialog(const std::vector<unsigned char>& bitmap, const std::string& signature);
	bool removeSignature() const { return m_for_removal; }
    ~SignatureViewDialog();

private:
    Ui::SignatureViewDialog *ui;
};

#endif // SIGNATUREVIEWDIALOG_H
