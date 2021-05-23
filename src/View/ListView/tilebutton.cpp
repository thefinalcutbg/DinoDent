#include "tilebutton.h"

TileButton::TileButton(QWidget* parent) : QAbstractButton(parent), hover(0), clicked(0), patient(nullptr)
{
	header.setFamily("Segoe UI semibold");
	header.setPointSizeF(14);

	header.setBold(1);
	info.setPointSizeF(8);
	info.setFamily("Segoe UI");

}

void TileButton::setPatient(Patient &patient)
{
	this->patient = &patient;
	refresh();
	update();
}

void TileButton::paintEvent(QPaintEvent* e)
{

	QColor color;
	if (clicked) color.setRgb(217, 224, 248);
	else if (hover) color.setRgb(201, 222, 245);
	else color.setRgb(238, 235, 235);

	QPainter painter;
	painter.begin(this);

	painter.fillRect(0,0, this->width(), this->height(), color);

	if (patient != NULL) paintInfo(&painter);

	painter.end();
}

void TileButton::enterEvent(QEvent* event)
{
	hover = true;
}

void TileButton::leaveEvent(QEvent* event)
{
	hover = false;
}

void TileButton::mousePressEvent(QMouseEvent* e)
{
	clicked = true;
	QAbstractButton::mousePressEvent(e);
	
}

void TileButton::mouseReleaseEvent(QMouseEvent* e)
{
	clicked = false;
	QAbstractButton::mouseReleaseEvent(e);
}

QString TileButton::elide(const QString& text, int length)
{
	if (text.length() < length) 
		return text;
	
	QStringRef elided(&text, 0, length);

	return elided + "...";
}



void PatientTile::paintInfo(QPainter* painter)
{
	QFontMetrics fm(info);

	int phonePosX = width() - fm.horizontalAdvance(phone) - 10;
	int addressPosX = width() - fm.horizontalAdvance(address) - 10;

	painter->setFont(header);
	painter->drawText(20, 25, name);
	painter->setFont(info);
	painter->drawText(20, 50, id);
	painter->drawText(20, 70, hirbNo);
	painter->drawText(20, 90, birthDate);
	painter->drawText(20, 110, age);
	painter->drawText(phonePosX, 90, phone);
	painter->drawText(addressPosX, 110, address);
	
}

PatientTile::PatientTile(QWidget* parent) : TileButton(parent)
{
	setFixedSize(461, 121);

}

void PatientTile::refresh()
{
	if (patient == NULL) return;

	name = elide(QString::fromStdString(patient->FirstName + " " + patient->MiddleName + " " + patient->LastName), 40);
	
	if (patient->type == 1) type = "ЕГН: ";
	else type = "ЛНЧ: ";
	
	id = type + QString::fromStdString(patient->id);

	if (patient->HIRBNo == "")
		hirbNo = "Номер на ЗОК: Няма данни";
	else hirbNo = "Номер на ЗОК: " + QString::fromStdString(patient->HIRBNo);

	birthDate = "Рожденна дата: " + QString::fromStdString(patient->birth);

	age = "Възраст: " + QString::number(Patient::getAge(patient->birth)) + " г.";
	
	QString city = QString::fromStdString(patient->city);

	for (int i = 0; i < city.size(); i++)
	{
		if (city[i] == "(")
		{
			address = "с. " + city.mid(0, i);
			break;
		}
		else if (city[i] == ",")
		{
			address = "гр. " + city.mid(0, i);
			break;
		}
	}

	if(patient->address != "")
		address.append(", " + QString::fromStdString(patient->address));

	address = elide(address, 50);

	if (patient->phone != "")
		phone = "Телефон: " + QString::fromStdString(patient->phone);
	else phone = "";

}



AllergiesTile::AllergiesTile(QWidget* parent) :
	TileButton(parent),
	allergiesLabel("Алергии:"),
	currentDLabel("Настоящи заболявания:"),
	pastDLabel("Минали заболявания:"),
	noInfo("Пациентът не съобщава")
{
	header.setPointSizeF(10);
	setFixedSize(351, 121);
}

void AllergiesTile::refresh()
{
	if (!patient->allergies.size())
		allergies = noInfo;
	else
		allergies = elide(QString::fromStdString(patient->allergies), 50);

	if (!patient->currentDiseases.size())
		currentDiseases = noInfo;
	else currentDiseases = elide(QString::fromStdString(patient->currentDiseases), 50);

	if (!patient->pastDiseases.size())
		pastDiseases = noInfo;
	else
	pastDiseases = elide(QString::fromStdString(patient->pastDiseases), 50);
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
