#include "allergiesdialog.h"

AllergiesDialog::AllergiesDialog(Database* database, QWidget* parent)
	: QDialog(parent), presenter(this, database)
{
	ui.setupUi(this);
	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Алергии, настоящи и минали заболявания");

	connect(ui.okButton, &QPushButton::clicked, this, 
		[=]
		{
			presenter.setAllergies(
				ui.allergiesEdit->toPlainText().toStdString(),
				ui.currentEdit->toPlainText().toStdString(),
				ui.pastEdit->toPlainText().toStdString());
			close();
		});

	connect(ui.cancelButton, &QPushButton::clicked, this, [=] {close(); });

}



void AllergiesDialog::open(std::string allergies, std::string current, std::string past)
{
	ui.allergiesEdit->setText(QString::fromStdString(allergies));
	ui.currentEdit->setText(QString::fromStdString(current));
	ui.pastEdit->setText(QString::fromStdString(past));

	ui.okButton->setFocus();
	exec();
}



void AllergiesDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);
	painter.end();
}


void AllergiesDialog::close()
{
	accept();
}

AllergiesDialogPresenter* AllergiesDialog::getPresenter()
{
	return &presenter;
}

AllergiesDialog::~AllergiesDialog()
{
}
