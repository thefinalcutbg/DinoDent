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
		m_result = ui.textBox->toPlainText().toStdString();
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

	QTextCursor newCursor(ui.textBox->document());
	newCursor.movePosition(QTextCursor::End);
	ui.textBox->setTextCursor(newCursor);
}

MultilineDialog::~MultilineDialog()
{
}
