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

	for (auto& str : ImplantData::typeStr()) ui.typeCombo->addItem(str.data());
	for (auto& str : ImplantData::timeStr()) ui.timeCombo->addItem(str.data());
	for (auto& str : ImplantData::tissueAugStr()) ui.tissueCombo->addItem(str.data());
	for (auto& str : ImplantData::boneAugStr()) ui.boneCombo->addItem(str.data());

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
				ui.system_edit->text().toStdString(),
				ui.width->value(),
				ui.length->value(),
				ui.timeCombo->currentIndex(),
				ui.typeCombo->currentIndex(),
				ui.tissueCombo->currentIndex(),
				boneCombo,
				boneCombo != 0 && ui.membrCheck->isChecked(),
				boneCombo != 0 && ui.sinusCheck->isChecked()
	};
}

void ImplantView::setData(const ImplantData& data)
{
	ui.system_edit->setText(QString::fromStdString(data.system));
	ui.timeCombo->setCurrentIndex(data.time);
	ui.typeCombo->setCurrentIndex(data.type);
	ui.length->setValue(data.length);
	ui.width->setValue(data.width);
	ui.tissueCombo->setCurrentIndex(data.tissue_aug);
	ui.boneCombo->setCurrentIndex(data.bone_aug);
	ui.membrCheck->setChecked(data.membrane && data.bone_aug);
	ui.sinusCheck->setChecked(data.sinusLift && data.bone_aug);

}
