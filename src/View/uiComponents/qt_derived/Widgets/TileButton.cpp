﻿#include "TileButton.h"
#include <QPainterPath>
#include "View/Theme.h"

TileButton::TileButton(QWidget* parent) : QAbstractButton(parent), hover(0), clicked(0)
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

	painter.setPen(QPen(Theme::border));

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
		update();
	}

	if (e->type() == QEvent::HoverLeave) {
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

void PatientTile::paintInfo(QPainter* painter)
{
	QFontMetrics fm(info);

	int phonePosX = width() - fm.horizontalAdvance(phone) - 10;
	int addressPosX = width() - fm.horizontalAdvance(address) - 10;

	constexpr int rowYPos[3]{ 60,80,100 };

	painter->setFont(infoLabel);
	painter->drawText(20, rowYPos[0], idLabel);
	painter->drawText(20, rowYPos[1], u8"Номер на ЗОК: ");
	painter->drawText(20, rowYPos[2], u8"Рождена дата: ");
	
	painter->drawText(width() / 2, rowYPos[0], u8"Телефон: ");
	painter->drawText(width() / 2, rowYPos[1], u8"Адрес: ");
	painter->drawText(width() / 2, rowYPos[2], u8"Възраст: ");

	QFontMetrics metric(infoLabel);

	auto horizontalAdvance = [metric](const QString& label) {
		return metric.horizontalAdvance(label);
	};

	painter->setFont(info);
	painter->drawText(20 + horizontalAdvance(idLabel), rowYPos[0], id);
	painter->drawText(20 + horizontalAdvance(u8"Номер на ЗОК: "), rowYPos[1], hirbNo);
	painter->drawText(20 + horizontalAdvance(u8"Рождена дата: "), rowYPos[2], birthDate);
	
	painter->drawText(width()/2 + horizontalAdvance(u8"Телефон: "), rowYPos[0], phone);
	painter->drawText(width()/2 + horizontalAdvance(u8"Адрес: "), rowYPos[1], address);
	painter->drawText(width() / 2 + horizontalAdvance(u8"Възраст: "), rowYPos[2], age);

	painter->setFont(header);
	painter->setPen(hover && !clicked ? QPen(Theme::fontRedClicked) : QPen(QColor(Theme::fontRed)));
	painter->drawText(20, 30, name);
	
}

PatientTile::PatientTile(QWidget* parent) : TileButton(parent){}

void PatientTile::setData(const Patient& patient, Date currentDate)
{
	name = elide(QString::fromStdString(patient.fullName()), 35);

	if (patient.type == 1) idLabel = "ЕГН: ";
	else idLabel = "ЛНЧ: ";

	id = QString::fromStdString(patient.id);

	if (patient.HIRBNo == "")
		hirbNo = "Няма данни";
	else hirbNo = QString::fromStdString(patient.HIRBNo);

	birthDate = QString::fromStdString((patient.birth.toString()));

	age = QString::number(patient.getAge(currentDate)) + " г.";

	address = elide(QString::fromStdString(patient.getFullAddress()), 37);

	if (patient.phone != "")
		phone = QString::fromStdString(patient.phone);
	else phone = "";

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