#include "MultilineDialog.h"
#include <QClipboard>
#include <QApplication>

MultilineDialog::MultilineDialog(const std::string& text, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Конзола");

	ui.setupUi(this);
	ui.textBox->setReadOnly(true);
	ui.textBox->setPlainText(QString::fromStdString(text));

    connect(ui.copyButton, &QPushButton::clicked, this, [&] {
		ui.textBox->selectAll();
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setText(ui.textBox->toPlainText());
	});

    connect(ui.okButton, &QPushButton::clicked, this, [&] {close();});
}

MultilineDialog::~MultilineDialog()
{
}
