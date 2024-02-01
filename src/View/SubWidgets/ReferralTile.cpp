#include "ReferralTile.h"
#include <QPainter>
#include "View/Theme.h"
#include "Model/Referrals/Referral.h"
#include "Model/FreeFunctions.h"

ReferralTile::ReferralTile(const Referral& ref, int index, QWidget* parent)
    : QWidget(parent), m_index(index)
{
	ui.setupUi(this);

	installEventFilter(this);
	setAttribute(Qt::WA_Hover, true);

	ui.printButton->setIcon(QIcon(":/icons/icon_print.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.hisButton->setIcon(QIcon(":/icons/icon_his.png"));

	ui.printButton->setHoverColor(Theme::mainBackgroundColor);
	ui.deleteButton->setHoverColor(Theme::mainBackgroundColor);

	if (ref.type != ReferralType::MH119 && ref.type != ReferralType::MDD4)
	{
		ui.printButton->hide();
	}

	if (ref.isNrnType())
	{	
		bool hasNrn = ref.nrn.size();

		if (hasNrn) ui.hisButton->hide();

		QString nrn = hasNrn ? "<b>НРН:</b> " + QString(ref.nrn.c_str())
			: "<b>НРН: </b>Няма";

		ui.numLabel->setText(nrn);

	}
	else
	{
		ui.hisButton->hide();
		ui.numLabel->setText("<b>Номер:</b> " + QString::fromStdString(FreeFn::leadZeroes(ref.number, 6)));
	}
	ui.dateLabel->setText("<b>Дата:</b> " + QString::fromStdString(ref.date.toBgStandard(true)));
	ui.title->setText("<b>" + QString::fromStdString(ref.getTypeAsString()) + "</b>");

	for (auto& label : findChildren<QLabel*>()) {
		label->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
		label->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));
	}

	connect(ui.printButton, &QPushButton::clicked, [&] { print(m_index); });
	connect(ui.deleteButton, &QPushButton::clicked, [&] { remove(m_index); });
	connect(ui.hisButton, &QPushButton::clicked, [&] { sendToHis(m_index); });

}


void ReferralTile::paintEvent(QPaintEvent*)
{
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), height(), Theme::radius, Theme::radius);

	QPainter painter;
	painter.begin(this);

	painter.fillPath(path,Theme::sectionBackground);

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);

	painter.setPen(pen);

	painter.drawPath(path);

}

bool ReferralTile::eventFilter(QObject* obj, QEvent* e)
{
    Q_UNUSED(obj)

	if (e->type() == QEvent::HoverEnter) {
		m_hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));

		for (auto& label : findChildren<QLabel*>()) {

			label->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoiseClicked));
		}

		update();
	}

	if (e->type() == QEvent::HoverLeave) {
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


ReferralTile::~ReferralTile()
{
	QApplication::restoreOverrideCursor();
}
