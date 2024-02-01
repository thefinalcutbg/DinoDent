#include "UpdatePromptDialog.h"
#include <qpainter.h>

void UpdatePromptDialog::paintEvent(QPaintEvent*)
{   
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

UpdatePromptDialog::UpdatePromptDialog(const std::string& changeLog, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("DinoDent");

	ui.changeLogText->setPlainText(QString::fromStdString(changeLog));

    connect(ui.yesButton, &QPushButton::clicked,  this, [&]{ accept();});
    connect(ui.noButton, &QPushButton::clicked, this, [&]{ reject(); });

}

UpdatePromptDialog::~UpdatePromptDialog()
{}
