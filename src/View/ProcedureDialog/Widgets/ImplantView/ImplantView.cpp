#include "ImplantView.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"

ImplantView::ImplantView(QWidget *parent)
	: QWidget(parent),
	presenter(nullptr)
{
	ui.setupUi(this);

	connect(ui.boneCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index)
		{
			ui.membrCheck->setHidden(!index);
			ui.sinusCheck->setHidden(!index);
		});

	ui.membrCheck->setHidden(true);
	ui.sinusCheck->setHidden(true);
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

AbstractLineEdit* ImplantView::systemEdit()
{
	return ui.system_edit;
}


std::tuple<std::string, int, int, double, double, int, int, bool, bool> ImplantView::getData()
{
	auto boneCombo = ui.boneCombo->currentIndex();

	return 
			std::make_tuple
			(
				ui.system_edit->text().toStdString(),
				ui.timeCombo->currentIndex(),
				ui.typeCombo->currentIndex(),
				ui.width->value(),
				ui.length->value(),
				ui.tissueCombo->currentIndex(),
				boneCombo,
				boneCombo != 0 && ui.membrCheck->isChecked(),
				boneCombo != 0 && ui.sinusCheck->isChecked()
			);
}

void ImplantView::setData(ImplantData data)
{
	ui.system_edit->set_Text(data.system);
	ui.timeCombo->setCurrentIndex(data.time);
	ui.typeCombo->setCurrentIndex(data.type);
	ui.length->setValue(data.length);
	ui.width->setValue(data.width);
	ui.tissueCombo->setCurrentIndex(data.tissue_aug);
	ui.boneCombo->setCurrentIndex(data.bone_aug);
	ui.membrCheck->setChecked(data.membrane);
	ui.sinusCheck->setChecked(data.sinusLift);
}
