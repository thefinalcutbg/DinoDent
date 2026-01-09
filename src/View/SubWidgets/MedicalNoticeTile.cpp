#include "MedicalNoticeTile.h"

#include <QPainter>

#include "View/Theme.h"
#include "Model/MedicalNotice.h"

MedicalNoticeTile::MedicalNoticeTile(const MedicalNotice& notice, int index, QWidget *parent)
    : QWidget(parent), m_index(index)
{
	ui.setupUi(this);

	installEventFilter(this);
	setAttribute(Qt::WA_Hover, true);

	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.hisButton->setIcon(QIcon(":/icons/icon_his.png"));

	ui.deleteButton->setHoverColor(Theme::mainBackgroundColor);

	bool hasNrn = notice.nrn.size();

	if (hasNrn) ui.hisButton->hide();

	QString nrn = hasNrn ? "<b>НРН:</b> " + QString(notice.nrn.c_str())
		: "<b>НРН: </b>Няма";

	ui.numLabel->setText(nrn);

	QString dateText = "<b>Дата:</b> от ";
	dateText += notice.from_date.toBgStandard(true).c_str();
	dateText += " до ";
	dateText += notice.to_date.toBgStandard(true).c_str();

	ui.dateLabel->setText(dateText);

	for (auto& label : findChildren<QLabel*>()) {
		label->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
		label->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));
	}

    connect(ui.deleteButton, &QPushButton::clicked, this, [&]{ emit remove(m_index); });
    connect(ui.hisButton, &QPushButton::clicked, this, [&]{ emit sendToHis(m_index); });
}

void MedicalNoticeTile::paintEvent(QPaintEvent*)
{
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), height(), Theme::radius/2, Theme::radius/2);

	QPainter painter;
	painter.begin(this);

	painter.fillPath(path, Theme::sectionBackground);

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);

	painter.setPen(pen);

	painter.drawPath(path);
}

bool MedicalNoticeTile::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		m_hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));

		for (auto& label : findChildren<QLabel*>()) {

			label->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoiseClicked));
		}

		update();
	}

    if (e->type() == QEvent::Leave) {
		QApplication::restoreOverrideCursor();

		for (auto& label : findChildren<QLabel*>()) {

			label->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));
		}

		m_hover = false;
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		m_clicked = true;
		update();
	}

	if (e->type() == QEvent::MouseButtonRelease)
	{
		m_clicked = false;

		emit clicked(m_index);
		update();
	}

	return false;
}

MedicalNoticeTile::~MedicalNoticeTile()
{}

