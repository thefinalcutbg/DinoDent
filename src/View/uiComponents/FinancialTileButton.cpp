#include "FinancialTileButton.h"
#include "Model/Financial/Recipient.h"
#include "View/Theme.h"
#include "Model/Financial/Issuer.h"

void RecipientTileButton::setRecipient(const Recipient& r)
{
	this->name = QString::fromStdString(r.name);
	this->id = QString::fromStdString(r.bulstat);
	this->address = QString::fromStdString(r.address);
	this->phone = QString::fromStdString(r.phone);
	repaint();
}


void RecipientTileButton::paintInfo(QPainter* painter)
{


	constexpr int rowYPos[4]{ 60,80,100,120 };

	painter->setFont(infoLabel);
	painter->drawText(20, rowYPos[0], "Име: ");
	painter->drawText(20, rowYPos[1], "Идент.№: ");
	painter->drawText(20, rowYPos[2], "Адрес: ");
	painter->drawText(20, rowYPos[3], "Телефон: ");

	QFontMetrics metric(infoLabel);

	auto horizontalAdvance = [metric](const QString& label) {
		return metric.horizontalAdvance(label);
	};

	painter->setFont(info);
	painter->drawText(20 + horizontalAdvance("Име: "), rowYPos[0], name);
	painter->drawText(20 + horizontalAdvance("Идент.№: "), rowYPos[1], id);
	painter->drawText(20 + horizontalAdvance("Адрес: "), rowYPos[2], address);
	painter->drawText(20 + horizontalAdvance("Телефон: "), rowYPos[3], phone);

	painter->setFont(header);
	painter->setPen(hover && !clicked ? QPen(Theme::fontRedClicked) : QPen(QColor(Theme::fontRed)));
	painter->drawText(20, 30, "Получател");
}

void IssuerTileButton::paintInfo(QPainter* painter)
{

	constexpr int rowYPos[4]{ 60,80,100,120 };

	painter->setFont(infoLabel);
	painter->drawText(20, rowYPos[0], "Име: ");
	painter->drawText(20, rowYPos[1], "Идент.№: ");
	painter->drawText(20, rowYPos[2], "Адрес: ");
	painter->drawText(20, rowYPos[3], "ДДС №: ");

	QFontMetrics metric(infoLabel);

	auto horizontalAdvance = [metric](const QString& label) {
		return metric.horizontalAdvance(label);
	};

	painter->setFont(info);
	painter->drawText(20 + horizontalAdvance("Име: "), rowYPos[0], name);
	painter->drawText(20 + horizontalAdvance("Идент.№: "), rowYPos[1], id);
	painter->drawText(20 + horizontalAdvance("Адрес: "), rowYPos[2], address);
	painter->drawText(20 + horizontalAdvance("ДДС №: "), rowYPos[3], this->vat);

	painter->setFont(header);
	painter->setPen(hover && !clicked ? QPen(Theme::fontRedClicked) : QPen(QColor(Theme::fontRed)));
	painter->drawText(20, 30, "Издател");

}

void IssuerTileButton::setIssuer(const Issuer& r)
{
	this->name = QString::fromStdString(r.company_name);
	this->id = QString::fromStdString(r.bulstat);
	this->address = QString::fromStdString(r.address_by_contract);
	this->vat = QString::fromStdString(r.issuer_bulstat_no_vat);
	repaint();
}





