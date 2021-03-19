#include "SurfaceSelector.h"

SurfaceSelector::SurfaceSelector(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.error_label->setStyleSheet("color:red");

	connect(ui.o_check, &QCheckBox::stateChanged, [=]{stateChangedByUser(); });
	connect(ui.m_check, &QCheckBox::stateChanged, [=] {stateChangedByUser(); });
	connect(ui.d_check, &QCheckBox::stateChanged, [=] {stateChangedByUser(); });
	connect(ui.b_check, &QCheckBox::stateChanged, [=] {stateChangedByUser(); });
	connect(ui.l_check, &QCheckBox::stateChanged, [=] {stateChangedByUser(); });
	connect(ui.c_check, &QCheckBox::stateChanged, [=] {stateChangedByUser(); });
}

SurfaceSelector::~SurfaceSelector()
{
}

void SurfaceSelector::setFocusAndSelectAll()
{
	ui.o_check->setFocus();
}

void SurfaceSelector::disable(bool disable)
{
	if (disable)
		this->hide();
	else
		this->show();
}

void SurfaceSelector::setAppearence(bool valid)
{
	if (valid)
		ui.error_label->setText("");
	else
		ui.error_label->setText("Изберете поне една повърхност!");
}

std::array<bool, 6> SurfaceSelector::getSurfaces()
{
	return std::array<bool, 6>
	{
			ui.o_check->isChecked(),
			ui.m_check->isChecked(),
			ui.d_check->isChecked(),
			ui.b_check->isChecked(),
			ui.l_check->isChecked(),
			ui.c_check->isChecked()
	};
}

void SurfaceSelector::setSurfaces(const std::array<bool, 6>& surfaces)
{
	ui.o_check->setChecked(surfaces[0]);
	ui.m_check->setChecked(surfaces[1]);
	ui.d_check->setChecked(surfaces[2]);
	ui.b_check->setChecked(surfaces[3]);
	ui.l_check->setChecked(surfaces[4]);
	ui.c_check->setChecked(surfaces[5]);
}
