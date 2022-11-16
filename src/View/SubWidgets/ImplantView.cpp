#include "ImplantView.h"
#include "Presenter/ProcedureDialogPresenter.h"
#include <QDebug>
ImplantView::ImplantView(QWidget *parent)
	: QWidget(parent),
	presenter(nullptr)
{
	ui.setupUi(this);

	ui.membrCheck->setDisabled(true);
	ui.sinusCheck->setDisabled(true);

	for (auto& str : ImplantType::getNames()) ui.typeCombo->addItem(str);
	for (auto& str : ImplantTime::getNames()) ui.timeCombo->addItem(str);
	for (auto& str : TissueAugmentation::getNames()) ui.tissueCombo->addItem(str);
	for (auto& str : BoneAugmentation::getNames()) ui.boneCombo->addItem(str);

	connect(ui.boneCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index)
		{
			ui.membrCheck->setDisabled(!index);
			ui.sinusCheck->setDisabled(!index);
		});
}

ImplantView::~ImplantView()
{
}

void ImplantView::setPresenter(ImplantPresenter* presenter)
{
	this->presenter = presenter;
}

void ImplantView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

ImplantData ImplantView::getData()
{
	auto boneCombo = ui.boneCombo->currentIndex();

	return 
			ImplantData
	{
		.time = ui.typeCombo->currentIndex(),
		.type = ui.typeCombo->currentIndex(),
		.width = ui.width->value(),
		.length = ui.length->value(),
		.tissue_aug = ui.tissueCombo->currentIndex(),
		.bone_aug = boneCombo,
		.membrane = boneCombo != 0 && ui.membrCheck->isChecked(),
		.sinusLift = boneCombo != 0 && ui.sinusCheck->isChecked()
	};
}

void ImplantView::setData(const ImplantData& data)
{
	ui.timeCombo->setCurrentIndex(data.time.getIndex());
	ui.typeCombo->setCurrentIndex(data.type.getIndex());
	ui.length->setValue(data.length);
	ui.width->setValue(data.width);
	ui.tissueCombo->setCurrentIndex(data.tissue_aug.getIndex());
	ui.boneCombo->setCurrentIndex(data.bone_aug.getIndex());
	ui.membrCheck->setChecked(data.membrane && data.bone_aug.getIndex());
	ui.sinusCheck->setChecked(data.sinusLift && data.bone_aug.getIndex());

}
