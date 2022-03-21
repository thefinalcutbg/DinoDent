#include "FinancialTileButton.h"
#include "Model/Financial/Recipient.h"
#include "View/Theme.h"
#include "Model/Financial/Issuer.h"

void RecipientTileButton::setRecipient(const Recipient& r)
{
	this->name = QString::fromStdString(r.name);
	this->id = QString::fromStdString(r.bulstat);
	this->address = QString::fromStdString(r.address);
	repaint();
}


void RecipientTileButton::paintInfo(QPainter* painter)
{
	painter->setFont(header);
	painter->setPen(hover && !clicked ? QPen(Theme::fontRedClicked) : QPen(QColor(Theme::fontRed)));
	painter->drawText(20, 30, u8"Получател");

	constexpr int rowYPos[3]{ 60,80,100 };

	painter->setFont(infoLabel);
	painter->drawText(20, rowYPos[0], u8"Име: ");
	painter->drawText(20, rowYPos[1], u8"Идент.№: ");
	painter->drawText(20, rowYPos[2], u8"Адрес: ");

	QFontMetrics metric(infoLabel);

	auto horizontalAdvance = [metric](const QString& label) {
		return metric.horizontalAdvance(label);
	};

	painter->setFont(info);
	painter->drawText(20 + horizontalAdvance(u8"Име"), rowYPos[0], name);
	painter->drawText(20 + horizontalAdvance(u8"Идент.№: "), rowYPos[1], id);
	painter->drawText(20 + horizontalAdvance(u8"Адрес: "), rowYPos[2], address);
}

void IssuerTileButton::paintInfo(QPainter* painter)
{
	painter->setFont(header);
	painter->setPen(hover && !clicked ? QPen(Theme::fontRedClicked) : QPen(QColor(Theme::fontRed)));
	painter->drawText(20, 30, u8"Идател");

	constexpr int rowYPos[4]{ 60,80,100,120 };

	painter->setFont(infoLabel);
	painter->drawText(20, rowYPos[0], u8"Име: ");
	painter->drawText(20, rowYPos[1], u8"Идент.№: ");
	painter->drawText(20, rowYPos[2], u8"Адрес: ");
	painter->drawText(20, rowYPos[3], u8"ДДС №: ");

	QFontMetrics metric(infoLabel);

	auto horizontalAdvance = [metric](const QString& label) {
		return metric.horizontalAdvance(label);
	};

	painter->setFont(info);
	painter->drawText(20 + horizontalAdvance(u8"Име"), rowYPos[0], name);
	painter->drawText(20 + horizontalAdvance(u8"Идент.№: "), rowYPos[1], id);
	painter->drawText(20 + horizontalAdvance(u8"Адрес: "), rowYPos[2], address);
	painter->drawText(20 + horizontalAdvance(u8"ДДС №: "), rowYPos[3], this->vat);
}

void IssuerTileButton::setIssuer(const Issuer& r)
{
	this->name = QString::fromStdString(r.company_name);
	this->id = QString::fromStdString(r.bulstat);
	this->address = QString::fromStdString(r.address_by_contract);
	this->vat = QString::fromStdString(r.issuer_bulstat_no_vat);
	repaint();
}





