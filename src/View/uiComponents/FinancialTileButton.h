#pragma once
#include "View/uiComponents/TileButton.h"

struct Recipient;
struct Issuer;

class RecipientTileButton : public TileButton
{
    Q_OBJECT

	QString name;
	QString id;
	QString address;
	QString vat_or_phone;
	bool hasVat{ false };
	void paintInfo(QPainter* painter) override;

public:
	RecipientTileButton(QWidget* parent) : TileButton(parent) {};
	void setRecipient(const Recipient& r);

};

class IssuerTileButton : public TileButton
{
    Q_OBJECT

	QString name;
	QString id;
	QString address;
	QString vat;

	void paintInfo(QPainter* painter) override;

public:
	IssuerTileButton(QWidget* parent) : TileButton(parent) { m_reveresed = true; };
	void setIssuer(const Issuer& r);

};
