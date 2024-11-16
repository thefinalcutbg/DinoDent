#pragma once

#include <QDialog>
#include "ui_CertificateListDialog.h"
#include "Network/PKCS11.h"

class CertificateListDialog : public QDialog
{
	Q_OBJECT

	int m_index = -1;

	void paintEvent(QPaintEvent* e) override;

public:
	CertificateListDialog(const std::vector<X509Details>& certList, QWidget *parent = nullptr);
	int getIndex() const { return m_index; }
	~CertificateListDialog();

private:
	Ui::CertificateListDialogClass ui;
};
