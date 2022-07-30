#pragma once
#include "View/uiComponents/qt_derived/Widgets/TileButton.h"

struct Recipient;
struct Issuer;

class RecipientTileButton : public TileButton
{
	QString name;
	QString id;
	QString address;
	QString phone;

	void paintInfo(QPainter* painter) override;

public:
	RecipientTileButton(QWidget* parent) : TileButton(parent) {};
	void setRecipient(const Recipient& r);

};

class IssuerTileButton : public TileButton
{
	QString name;
	QString id;
	QString address;
	QString vat;

	void paintInfo(QPainter* painter) override;

public:
	IssuerTileButton(QWidget* parent) : TileButton(parent) { m_reveresed = true; };
	void setIssuer(const Issuer& r);

};