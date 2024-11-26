#include "MultilineDialog.h"
#include <QClipboard>
#include <QApplication>
#include <QTextCursor>

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

    connect(ui.okButton, &QPushButton::clicked, this, [&] {

		m_result = ui.textBox->isReadOnly() ?
			ui.textBox->toPlainText().toStdString()
			:
			ui.textBox->getText();
		
		close();
	});
}

std::optional<std::string> MultilineDialog::getResult()
{
	return m_result;
}

void MultilineDialog::enableEditing()
{
	ui.textBox->setReadOnly(false);
	ui.copyButton->hide();

	ui.textBox->setText(ui.textBox->toPlainText().toStdString());
}

MultilineDialog::~MultilineDialog()
{
}
