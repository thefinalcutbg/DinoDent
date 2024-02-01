#include "PracticeListDialog.h"
#include <QPainter>



PracticeListDialog::PracticeListDialog(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Изберете практика");

    connect(ui.okButton, &QPushButton::clicked, this, [&] { done(ui.comboBox->currentIndex()); close(); });
}

void PracticeListDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
}

int PracticeListDialog::exec(const std::vector<std::string>& practiceList)
{
	for (auto& practice : practiceList)
		ui.comboBox->addItem(QString::fromStdString(practice));

	return QDialog::exec();
}

PracticeListDialog::~PracticeListDialog()
{
}
