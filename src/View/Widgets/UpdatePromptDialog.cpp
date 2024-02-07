#include "UpdatePromptDialog.h"
#include <qpainter.h>
#include <QScrollBar>
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

	ui.changeLogText->setText(QString::fromStdString(changeLog));

    connect(ui.yesButton, &QPushButton::clicked,  this, [&]{ done(QDialog::Accepted);});
    connect(ui.noButton, &QPushButton::clicked, this, [&]{ done(QDialog::Rejected); });
	connect(ui.link_label, &QLabel::linkActivated, this, [&] { done(2); });
}

UpdatePromptDialog::~UpdatePromptDialog()
{}
