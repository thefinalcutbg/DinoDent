#include "ObturationView.h"



ObturationView::ObturationView(QWidget* parent)
	: QWidget(parent), m_presenter(nullptr)
{
	ui.setupUi(this);

}

ObturationView::~ObturationView()
{
}

void ObturationView::setPresenter(ObturationPresenter* presenter)
{
	this->m_presenter = presenter;
}

AbstractSurfaceSelector* ObturationView::surfaceSelector(){
	return ui.surfaceSelector;
}


void ObturationView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

#include "Model/Procedure/Procedure.h"

void ObturationView::setData(const ProcedureObtData& data)
{
	ui.obturationWidget->setData(data.data);
	ui.post_check->setChecked(data.post);
	ui.surfaceSelector->setSurfaces(data.surfaces);
}

ProcedureObtData ObturationView::getData()
{
	return ProcedureObtData{
		ui.surfaceSelector->getSurfaces(),
		ui.post_check->isChecked(),
		ObturationData{
		ui.obturationWidget->getData()
		}
	};


}
