#include "TileButton.h"
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
#include "View/Graphics/Zodiac.h"
#include "Model/FreeFunctions.h"

TileButton::TileButton(QWidget* parent) : QAbstractButton(parent)
{

	header.setBold(1);
    header.setPointSizeF(info.pointSizeF()+6);

	infoLabel.setBold(true);

	this->installEventFilter(this);

}

void TileButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);

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

bool TileButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
		hover = true;
		QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
		update();
	}

    if (e->type() == QEvent::Leave) {
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


#include <QDebug>
PatientTile::PatientTile(QWidget* parent) : TileButton(parent)
{
	nraButton = new IconButton(this);
	nraButton->setIcon(QIcon(":/icons/icon_nra.png"));
	nraButton->move(5, 5);
	nraButton->setFixedSize(iconSize, iconSize);

	printButton = new IconButton(this);
	printButton->setIcon(QIcon(":/icons/icon_print.png"));
	printButton->setFixedSize(iconSize, iconSize);
	printButton->move(width() - (iconSize + 5), 5);
	printButton->setToolTip("Принтиране на декларации");

	notesButton = new IconButton(this);
	notesButton->setIcon(QIcon(":/icons/icon_notes.png"));
	notesButton->setFixedSize(iconSize, iconSize);
	notesButton->move(printButton->width(), printButton->y() + 40);
	notesButton->setToolTip("Бележки за пациента");

}


void PatientTile::paintInfo(QPainter* painter)
{

    //int phonePosX = width() - fm.horizontalAdvance(phone) - 10;
    //int addressPosX = width() - fm.horizontalAdvance(address) - 10;

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
	painter->drawText(nraButton->x() + iconSize + 5, 27, name);

	painter->setRenderHint(QPainter::Antialiasing);

	if(zodiac) painter->drawPixmap(width()-36, height()-36, 32, 32, *zodiac);

	static QPixmap bdayPx{ ":/icons/icon_bday.png" };

    if (birthday) painter->drawPixmap(20 + horizontalAdvance("Рождена дата: 00.00.0000 г"), 86, 15, 15, bdayPx);
	
}

void PatientTile::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	printButton->move(width() - 5 - printButton->width(), 5);
	notesButton->move(printButton->x(), 40);
	update();
}



void PatientTile::setData(const Patient& patient, int age)
{
	name = elide(QString::fromStdString(patient.fullName()), 35);

	switch (patient.type)
	{
	case Patient::EGN: idLabel = "ЕГН: "; break;
	case Patient::LNCH: idLabel = "ЛНЧ: "; break;
	case Patient::SSN: idLabel = "ССН: "; break;
	case Patient::EU: idLabel = "ID: "; break;
	default: idLabel = "???: "; break;
	}

	id = QString::fromStdString(patient.id);

	if (patient.HIRBNo == "")
		hirbNo = "Няма данни";
	else hirbNo = QString::fromStdString(patient.HIRBNo);

	birthDate = QString::fromStdString((patient.birth.toBgStandard(true)));

	this->age = QString::number(age) + " г.";

    address = elide(QString::fromStdString(patient.getFullAddress()), 28);

	if (patient.phone != "")
		phone = QString::fromStdString(patient.phone);
	else phone = "";

	if (patient.insuranceStatus)
    {
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

	birthday = patient.birth.isSameDayInTheYear();

	zodiac = Zodiac::getPixmap(patient.birth.day, patient.birth.month);

	update();
}


MedStatusTile::MedStatusTile(QWidget* parent) :
	TileButton(parent)
{
	reverse();
	header.setPointSizeF(10);

	hisButton = new IconButton(this);
	hisButton->setIcon(QIcon(":/icons/icon_his.png"));
	hisButton->setFixedSize(buttonSize, buttonSize);
	hisButton->move(width() - 5 - buttonSize, 5);
	hisButton->setToolTip("Извличане на алергии от НЗИС");

	nhifButton = new IconButton(this);
	nhifButton->setIcon(QIcon(":/icons/icon_nhif.png"));
	nhifButton->setFixedSize(buttonSize, buttonSize);
	nhifButton->move(width() - 5 - buttonSize, 30);
	nhifButton->setToolTip("Извличане на диагнози от рецептурната книжка");


}


void MedStatusTile::setData(const Patient& patient)
{
	allergies = elide(patient.getAllergiesStr().c_str(), 40);
	currentDiseases = elide(FreeFn::listToString(patient.medStats.condition, "Не съобщава").c_str(), 40);
	pastDiseases = elide(FreeFn::listToString(patient.medStats.history, "Не съобщава").c_str(), 40);

	update();
}

void MedStatusTile::paintInfo(QPainter* painter)
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

void MedStatusTile::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	hisButton->move(width() - 5 - buttonSize, 5);
	nhifButton->move(width() - 5 - buttonSize, 35);

	update();
}
