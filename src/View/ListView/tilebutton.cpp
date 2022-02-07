#include "tilebutton.h"
#include <QPainterPath>

TileButton::TileButton(QWidget* parent) : QAbstractButton(parent), hover(0), clicked(0)
{
	header.setFamily("Segoe UI semibold");
	header.setPointSizeF(14);

	header.setBold(1);
	info.setPointSizeF(8);
	info.setFamily("Segoe UI");

	this->installEventFilter(this);

}

void TileButton::paintEvent(QPaintEvent* e)
{

	QColor color(251, 251, 251);

	if(hover) color.setRgb(242, 242, 242);

	QPainter painter(this);

	constexpr int radius = 8;

	QPainterPath path;
	path.addRoundedRect(rect(), radius, radius);

	painter.fillPath(path, color);
		

	painter.setPen(QPen(QColor(200, 200, 200)));

	painter.drawPath(path);

	QColor textColor = clicked ?
		QColor(92, 93, 95)
		:
		QColor(26, 26, 26);

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

void PatientTile::setData(const Patient& patient, Date currentDate)
{
	name = elide(QString::fromStdString(patient.FirstName + " " + patient.MiddleName + " " + patient.LastName), 40);

	if (patient.type == 1) type = "ЕГН: ";
	else type = "ЛНЧ: ";

	id = type + QString::fromStdString(patient.id);

	if (patient.HIRBNo == "")
		hirbNo = "Номер на ЗОК: Няма данни";
	else hirbNo = "Номер на ЗОК: " + QString::fromStdString(patient.HIRBNo);

	birthDate = "Рождена дата: " + QString::fromStdString((patient.birth.toString()));

	age = "Възраст: " + QString::number(patient.getAge(currentDate)) + " г.";

	QString city = QString::fromStdString(patient.city);

	for (int i = 0; i < city.size(); i++)
	{
		if (city[i] == '(')
		{
			address = "с. " + city.mid(0, i);
			break;
		}
		else if (city[i] == ',' )
		{
			address = "гр. " + city.mid(0, i);
			break;
		}
	}

	if (patient.address != "")
		address.append(", " + QString::fromStdString(patient.address));

	address = elide(address, 50);

	if (patient.phone != "")
		phone = "Телефон: " + QString::fromStdString(patient.phone);
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
	header.setPointSizeF(10);
	setFixedSize(351, 121);
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
