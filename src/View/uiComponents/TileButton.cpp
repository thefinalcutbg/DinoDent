#include "TileButton.h"
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
#include "View/Graphics/Zodiac.h"

TileButton::TileButton(QWidget* parent) : QAbstractButton(parent)
{
	header.setFamily("Segoe UI semibold");
	header.setPointSizeF(14);

	header.setBold(1);
	info.setPointSizeF(8);
	info.setFamily("Segoe UI");
	
	infoLabel.setFamily("Segoe UI");
	infoLabel.setBold(true);
	infoLabel.setPointSizeF(8);

	this->installEventFilter(this);

}

void TileButton::paintEvent(QPaintEvent* e)
{


	//if(hover) color.setRgb(242, 242, 242);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//getting the half-rounded button path:

	auto path = Theme::getHalfCurvedPath(width(), height());

	if(m_reveresed) //transforming if reversed;
	{
		QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);
		painter.setTransform(mirror);
		painter.translate(-width(), 0);
	}

	painter.fillPath(path, Theme::sectionBackground);

	QPen pen(Theme::border);
	pen.setCosmetic(true);
	pen.setWidth(2);
	painter.setPen(pen);

	painter.drawPath(path);

	if (m_reveresed)
	{
		painter.resetTransform();
	}


	QColor textColor = hover && !clicked ?
		QColor(Theme::fontTurquoiseClicked)
		:
		QColor(Theme::fontTurquoise);

	painter.setPen(QPen(textColor));

	paintInfo(&painter);

	painter.end();
}

bool TileButton::eventFilter(QObject* obj, QEvent* e)
{
	if (e->type() == QEvent::HoverEnter) {
		hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
		QApplication::restoreOverrideCursor();
		hover = false;
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		clicked = true;
	}

	if (e->type() == QEvent::MouseButtonRelease)
	{
		clicked = false;
	}

	return false;
}

QString TileButton::elide(const QString& text, int length)
{
	if (text.length() < length) 
		return text;
	

	return text.chopped(text.length() - length) + "...";
}



PatientTile::PatientTile(QWidget* parent) : TileButton(parent)
{
	nraButton = new IconButton(this);
	nraButton->setIcon(QIcon(":/icons/icon_nra.png"));
	nraButton->move(5, 5);
	nraButton->setFixedSize(nraSize, nraSize);

	printButton = new IconButton(this);
	printButton->setIcon(QIcon(":/icons/icon_print.png"));
	printButton->setFixedSize(nraSize, nraSize);
	printButton->move(width() - (nraSize + 5), 5);
	printButton->setToolTip("Принтиране на декларации");

}


void PatientTile::paintInfo(QPainter* painter)
{
	QFontMetrics fm(info);

	int phonePosX = width() - fm.horizontalAdvance(phone) - 10;
	int addressPosX = width() - fm.horizontalAdvance(address) - 10;

	constexpr int rowYPos[3]{ 60,80,100 };

	painter->setFont(infoLabel);
	painter->drawText(20, rowYPos[0], idLabel);
	painter->drawText(20, rowYPos[1], "Номер на ЗОК: ");
	painter->drawText(20, rowYPos[2], "Рождена дата: ");
	
	painter->drawText(width() / 2, rowYPos[0], "Телефон: ");
	painter->drawText(width() / 2, rowYPos[1], "Адрес: ");
	painter->drawText(width() / 2, rowYPos[2], "Възраст: ");

	QFontMetrics metric(infoLabel);

	auto horizontalAdvance = [metric](const QString& label) {
		return metric.horizontalAdvance(label);
	};

	painter->setFont(info);
	painter->drawText(20 + horizontalAdvance(idLabel), rowYPos[0], id);
	painter->drawText(20 + horizontalAdvance("Номер на ЗОК: "), rowYPos[1], hirbNo);
	painter->drawText(20 + horizontalAdvance("Рождена дата: "), rowYPos[2], birthDate);

	painter->drawText(width()/2 + horizontalAdvance("Телефон: "), rowYPos[0], phone);
	painter->drawText(width()/2 + horizontalAdvance("Адрес: "), rowYPos[1], address);
	painter->drawText(width() / 2 + horizontalAdvance("Възраст: "), rowYPos[2], age);

	painter->setFont(header);
	painter->setPen(hover && !clicked ? QPen(Theme::fontRedClicked) : QPen(QColor(Theme::fontRed)));
	painter->drawText(nraButton->x() + nraSize + 5, 27, name);

	//QFontMetrics titleMetric(header);

	//int xZodiac = (nraButton->x() + nraSize + 5 + titleMetric.horizontalAdvance(name) + 5);

	if (zodiac) painter->drawPixmap(width()-35, height()-35, 30, 30, *zodiac);
	
}

void PatientTile::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	printButton->move(width() - 5 - printButton->width(), 5);
	update();
}



void PatientTile::setData(const Patient& patient, int age)
{
	name = elide(QString::fromStdString(patient.fullName()), 35);

	if (patient.type == 1) idLabel = "ЕГН: ";
	else idLabel = "ЛНЧ: ";

	id = QString::fromStdString(patient.id);

	if (patient.HIRBNo == "")
		hirbNo = "Няма данни";
	else hirbNo = QString::fromStdString(patient.HIRBNo);

	birthDate = QString::fromStdString((patient.birth.toBgStandard(true)));

	this->age = QString::number(age) + " г.";

	address = elide(QString::fromStdString(patient.getFullAddress()), 37);

	if (patient.phone != "")
		phone = QString::fromStdString(patient.phone);
	else phone = "";

	if (patient.insuranceStatus)
	{
		QColor nraBackgroundColor;

		switch (patient.insuranceStatus->status)
		{
		case Insured::Yes:
			nraButton->setBackgroundColor(QColor(101,199,208));
			nraButton->setHoverColor(QColor(80,152,154));
			nraButton->setToolTip("Пациентът е здравноосигурен");
			break;
		case Insured::No:
			nraButton->setBackgroundColor(Theme::fontRed);
			nraButton->setHoverColor(Theme::fontRedClicked); 
			nraButton->setToolTip("Пациентът няма здравни осигуровки");
			break;
		case Insured::NoData:
			nraButton->setBackgroundColor(QColor(255, 165, 0));
			nraButton->setHoverColor(QColor(255, 165, 0));
			nraButton->setToolTip("За този пациент няма данни в НАП");
			break;
		}
	}
	else
	{
		nraButton->setBackgroundColor(Theme::sectionBackground);
		nraButton->setHoverColor(Theme::background);
		nraButton->setToolTip("Проверка на здравноосигурителен статус");

	}

	zodiac = Zodiac::getPixmap(patient.birth.day, patient.birth.month);

	update();
}


AllergiesTile::AllergiesTile(QWidget* parent) :
	TileButton(parent),
	allergiesLabel("Алергии:"),
	currentDLabel("Настоящи заболявания:"),
	pastDLabel("Минали заболявания:"),
	noInfo("Пациентът не съобщава")
{
	reverse();
	header.setPointSizeF(10);

	nhifButton = new IconButton(this);
	nhifButton->setIcon(QIcon(":/icons/icon_nzok.png"));
	nhifButton->setFixedSize(nzokSize, nzokSize);
	nhifButton->move(width() - nzokSize, 5);
	nhifButton->setToolTip("Проверка на диагнози в рецептурната книжка");

}


void AllergiesTile::setData(const Patient& patient)
{
	if (!patient.allergies.size())
		allergies = noInfo;
	else
		allergies = elide(QString::fromStdString(patient.allergies), 50);

	if (!patient.currentDiseases.size())
		currentDiseases = noInfo;
	else currentDiseases = elide(QString::fromStdString(patient.currentDiseases), 50);

	if (!patient.pastDiseases.size())
		pastDiseases = noInfo;
	else
		pastDiseases = elide(QString::fromStdString(patient.pastDiseases), 50);
}

void AllergiesTile::paintInfo(QPainter* painter)
{
	painter->setFont(header);
	painter->drawText(20, 20, allergiesLabel);
	painter->drawText(20, 58, currentDLabel);
	painter->drawText(20, 96, pastDLabel);

	painter->setFont(info);
	painter->drawText(20, 37, allergies);
	painter->drawText(20, 75, currentDiseases);
	painter->drawText(20, 113, pastDiseases);

}

void AllergiesTile::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	nhifButton->move(width() - 5 - nzokSize, 5);
	update();
}
