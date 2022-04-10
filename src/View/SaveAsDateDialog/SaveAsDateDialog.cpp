#include "SaveAsDateDialog.h"


SaveAsDateDialog::SaveAsDateDialog(const Date& d, const std::string& docName, QWidget* parent)
	: QDialog(parent)
{
    ui.setupUi(this);

	setWindowTitle(u8"Запази като");

	ui.dateEdit->setDate(QDate(d.year, d.month, d.day));

	ui.label->setText(QString::fromStdString(docName) + u8" от дата:");

    connect(ui.okButton, &QPushButton::clicked, this,
        [=] {

            auto d = ui.dateEdit->date();

            m_date.emplace(d.day(), d.month(), d.year());

            accept();

        });
    connect(ui.cancelButton, &QPushButton::clicked, this, [=] { done(0); close(); });
}

SaveAsDateDialog::~SaveAsDateDialog()
{
}
