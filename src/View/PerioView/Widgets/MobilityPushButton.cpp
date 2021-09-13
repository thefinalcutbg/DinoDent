#include "MobilityPushButton.h"

constexpr const char* label[4]{ "", "I", "II", "III" };



MobilityPushButton::MobilityPushButton(QWidget *parent)
	: PerioButton(parent)
{
	setCheckable(true);
	
	auto f = font();
	f.setBold(true);
	setFont(f);

	QPalette palet = palette();
	QColor red; red.setRgb(218, 69, 63);
	palet.setBrush(QPalette::Text, QBrush(red));
	setPalette(palet);


	connect(this, &QAbstractButton::clicked, [=] { setMobility(m_mobility + 1); });
}



void MobilityPushButton::setMobility(int mobility)
{
	
	setFixedSize(width(), height());

	switch (mobility)
	{
	case 0:
		m_text = ""; setChecked(false); break;
	case 1:
		m_text = "I"; setChecked(true); break;
	case 2:
		m_text = ("II"); setChecked(true); break;
	case 3:
		m_text = "III"; setChecked(true); break;
	default:
		m_text = ""; setChecked(false); mobility = 0; break;
	}

	m_mobility = mobility;
	update();
}



MobilityPushButton::~MobilityPushButton()
{
}
