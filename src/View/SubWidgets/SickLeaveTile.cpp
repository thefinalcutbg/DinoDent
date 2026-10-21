#include "SickLeaveTile.h"
#include "View/Theme.h"
#include <QPainter>

SickLeaveTile::SickLeaveTile(const SickLeave& notice, int index, QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::SickLeaveTileClass()),
	m_index(index)
{
	ui->setupUi(this);

	installEventFilter(this);
	setAttribute(Qt::WA_Hover, true);
	
	ui->printButton->setIcon(QIcon(":/icons/icon_print.png"));
	ui->deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui->nssiButton->setIcon(QIcon(":/icons/icon_nssi.png"));

	ui->printButton->setGraphicsEffect(nullptr);
	ui->deleteButton->setGraphicsEffect(nullptr);
	ui->nssiButton->setGraphicsEffect(nullptr);

	ui->printButton->setHoverColor(Theme::mainBackgroundColor);
	ui->deleteButton->setHoverColor(Theme::fontRedClicked);
	ui->nssiButton->setHoverColor(Theme::mainBackgroundColor);

	QString numLabelText = "<b>Номер:</b> ";

	numLabelText.append(notice.number ? "E" + QString::number(notice.number) : QString("Не е изпратен към НОИ"));

	ui->numLabel->setText(numLabelText);
	ui->nssiButton->setHidden(notice.number);
	
	ui->dateLabel->setText("<b>Дата:</b> " + QString::fromStdString(notice.date.toBgStandard(true)));
	ui->title->setText("<b>Болничен лист</b>");

	for (auto& label : findChildren<QLabel*>()) {
		label->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
		label->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));
	}

	connect(ui->printButton, &QPushButton::clicked, [&] { print(m_index); });
	connect(ui->deleteButton, &QPushButton::clicked, [&] { remove(m_index); });
	connect(ui->nssiButton, &QPushButton::clicked, [&] { sendToNSSI(m_index); });
	
}
void SickLeaveTile::paintEvent(QPaintEvent*)
{
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), height(), Theme::radius/2, Theme::radius/2);

	QPainter painter;
	painter.begin(this);

	painter.fillPath(path,Theme::sectionBackground);

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);

	painter.setPen(pen);

	painter.drawPath(path);

}

bool SickLeaveTile::eventFilter(QObject* obj, QEvent* e)
{
    Q_UNUSED(obj)

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

SickLeaveTile::~SickLeaveTile()
{
	QApplication::restoreOverrideCursor();

	delete ui;
}

