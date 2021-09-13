#include "PerioSurfaceHeader.h"

PerioSurfaceHeader::PerioSurfaceHeader(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	static auto icon(QIcon(QPixmap("icons/implant.png")));

	setMinimumHeight(41);

	constexpr int toothidx[16]{ 18, 17,16,15,14,13,12,11,21,22,23,24,25,26,27,28 };

	for (int i = 0; i < 16; i++)
	{
		m_tooth[i] = new QPushButton(this);
		m_tooth[i]->setCheckable(true);
		m_tooth[i]->setChecked(true);
		m_tooth[i]->setMinimumWidth(10);
		m_tooth[i]->setText(QString::number(toothidx[i]));
		ui.toothLayout->addWidget(m_tooth[i]);


		connect(m_tooth[i], &QPushButton::clicked,
			[=] { emit toothStateChanged(i, m_tooth[i]->isChecked()); toothStateSlot(i, m_tooth[i]->isChecked()); });

		m_mobi[i] = new MobilityPushButton(this);
		ui.mobilityLayout->addWidget(m_mobi[i]);

		
		m_implant[i] = new PerioButton(this);
		m_implant[i]->setCheckable(true);
		//m_implant[i]->setMinimumWidth(1);
		m_implant[i]->setIcon(icon);
		ui.implantLayout->addWidget(m_implant[i]);


	}
}

void PerioSurfaceHeader::fixSize()
{
	for (int i = 0; i < 16; i++)
	{

		m_mobi[i]->setFixedWidth(m_mobi[i]->width());
		m_tooth[i]->setFixedWidth(m_tooth[i]->width());
		m_implant[i]->setFixedWidth(m_tooth[i]->width());
	}
}

void PerioSurfaceHeader::toothStateSlot(int tooth, bool present)
{
	m_mobi[tooth]->setDisabled(!present);
	m_implant[tooth]->setDisabled(!present);
}



PerioSurfaceHeader::~PerioSurfaceHeader()
{
}
