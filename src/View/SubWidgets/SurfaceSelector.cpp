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
{
	if (!errorLabel) return;

	valid ?
		errorLabel->setText("")
		:
		errorLabel->setText("Изберете поне една повърхност!");
		
}

RestorationData SurfaceSelector::getData()
{
	return
	{
			ui.o_check->isChecked(),
			ui.m_check->isChecked(),
			ui.d_check->isChecked(),
			ui.b_check->isChecked(),
			ui.l_check->isChecked(),
			ui.c_check->isChecked(),
			ui.postCheck->isChecked()
	};
}


void SurfaceSelector::setData(const RestorationData& data)
{
	ui.o_check->setChecked(data.surfaces[0]);
	ui.m_check->setChecked(data.surfaces[1]);
	ui.d_check->setChecked(data.surfaces[2]);
	ui.b_check->setChecked(data.surfaces[3]);
	ui.l_check->setChecked(data.surfaces[4]);
	ui.c_check->setChecked(data.surfaces[5]);

	ui.postCheck->setChecked(data.post);
}
