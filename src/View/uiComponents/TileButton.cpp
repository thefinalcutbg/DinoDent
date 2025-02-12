#include "TileButton.h"
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>
#include "View/Graphics/Zodiac.h"
#include "Model/FreeFunctions.h"
#include "Model/User.h"

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


PatientTile::PatientTile(QWidget* parent) : TileButton(parent)
{
	nraButton = new IconButton(this);
	nraButton->setIcon(QIcon(":/icons/icon_nra.png"));
	nraButton->move(5, 5);
	nraButton->setFixedSize(iconSize, iconSize);

	appointmentButton = new IconButton(this);
	appointmentButton->setIcon(QIcon(":/icons/icon_calendar.png"));
	appointmentButton->setFixedSize(iconSize, iconSize);
    appointmentButton->move(width() - (iconSize + 5), 5);
	appointmentButton->setToolTip("Запази посещение");

    notificationButton = new IconButton(this);
    notificationButton->setIcon(QIcon(":/icons/icon_bell.png"));
    notificationButton->setFixedSize(iconSize, iconSize);
    notificationButton->move(appointmentButton->x()-40, appointmentButton->y());
    notificationButton->setToolTip("Добавяне на напомняне");

    notesButton = new IconButton(this);
    notesButton->setIcon(QIcon(":/icons/icon_notes.png"));
    notesButton->setFixedSize(iconSize, iconSize);
    notesButton->move(notificationButton->x() - 40, notificationButton->y());
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

	int nameXPos = 20;

	if (!nraButton->isHidden()) {
		nameXPos = nraButton->x() + iconSize + 5;
	}

	painter->drawText(nameXPos, 27, name);

	painter->setRenderHint(QPainter::Antialiasing);

	if(zodiac) painter->drawPixmap(width()-37, height()-36, 32, 32, *zodiac);

	static QPixmap bdayPx{ ":/icons/icon_bday.png" };

    if (birthday) painter->drawPixmap(20 + horizontalAdvance("Рождена дата: 00.00.0000 г"), 86, 15, 15, bdayPx);
	
}

void PatientTile::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

    appointmentButton->move(width() - 5 - appointmentButton->width(), 5);
    notificationButton->move(appointmentButton->x()-40, appointmentButton->y());
    notesButton->move(notificationButton->x()-40, notificationButton->y());

	update();
}

void PatientTile::setData(const Patient& patient, int age)
{

	nraButton->setHidden(!User::hasNhifContract());

	name = patient.fullName().c_str();
	if (name.size() > 32) {
		name = patient.firstLastName().c_str();
	}

	if (name.size() > 32) {
		name = elide(QString::fromStdString(patient.fullName()), 30);
	}

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

	notesButton->setMonochrome(patient.patientNotes.empty());

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

	hospitalizedButton = new IconButton(this);
	hospitalizedButton->setIcon(QIcon(":/icons/icon_hospital.png"));
	hospitalizedButton->setFixedSize(buttonSize, buttonSize);
	hospitalizedButton->move(width() - 5 - buttonSize, 40);
	hospitalizedButton->setToolTip("Проверка за активна хоспитализация");

	nhifButton = new IconButton(this);
	nhifButton->setIcon(QIcon(":/icons/icon_nhif.png"));
	nhifButton->setFixedSize(buttonSize, buttonSize);
	nhifButton->move(width() - 5 - buttonSize, 80);
	nhifButton->setToolTip("Извличане на диагнози от рецептурната книжка");
}

void MedStatusTile::setData(const Patient& patient)
{

	auto listToString = [](const std::vector<MedicalStatus>& list, const std::string& emptyList)
	{
			std::string result;

			for (auto& str : list) {
				result += str.diagnosis.name();
				result += ", ";
			}

			if (result.size()) {
				result.pop_back();
				result.pop_back();
			}
			else {
				result = emptyList;
			};

			return result;
		};

    allergies = elide(patient.getAllergiesStr().c_str(), 30);
    currentDiseases = elide(listToString(patient.medStats.condition, "Не съобщава").c_str(), 30);
    pastDiseases = elide(listToString(patient.medStats.history, "Не съобщава").c_str(), 30);

	nhifButton->setHidden(!User::hasNhifContract());
	hospitalizedButton->setHidden(!User::hasNhifContract());

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
	const int spacing = (height() - (buttonSize * 3)) / 4;
	hisButton->move(width() - 5 - buttonSize, spacing);
	hospitalizedButton->move(width() - 5 - buttonSize, spacing*2 + buttonSize);
	nhifButton->move(width() - 5 - buttonSize, spacing*3 + buttonSize*2);
	

	update();
}
