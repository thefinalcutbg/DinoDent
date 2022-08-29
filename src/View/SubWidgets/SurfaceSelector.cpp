#include "SurfaceSelector.h"
#include <QLabel>

SurfaceSelector::SurfaceSelector(QWidget *parent)
	: QWidget(parent), errorLabel(nullptr)
{
	ui.setupUi(this);

	connect(ui.o_check, &QCheckBox::stateChanged, [=] {validateInput(); });
	connect(ui.m_check, &QCheckBox::stateChanged, [=] {validateInput(); });
	connect(ui.d_check, &QCheckBox::stateChanged, [=] {validateInput(); });
	connect(ui.b_check, &QCheckBox::stateChanged, [=] {validateInput(); });
	connect(ui.l_check, &QCheckBox::stateChanged, [=] {validateInput(); });
	connect(ui.c_check, &QCheckBox::stateChanged, [=] {validateInput(); });

}

SurfaceSelector::~SurfaceSelector()
{
}

void SurfaceSelector::setErrorLabel(QLabel* errorLabel)
{
	this->errorLabel = errorLabel;
}

void SurfaceSelector::setFocus()
{
	ui.o_check->setFocus();
}

void SurfaceSelector::disable(bool disable)
{
		this->setHidden(disable);
}

void SurfaceSelector::setValidAppearence(bool valid)
{/*
	if (!errorLabel) return;

	valid ?
		errorLabel->setText("")
		:
		errorLabel->setText("Изберете поне една повърхност!");
		*/
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
