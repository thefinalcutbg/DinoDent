#include "ObturationView.h"



ObturationView::ObturationView(QWidget* parent)
	: QWidget(parent), m_presenter(nullptr)
{
	ui.setupUi(this);

	ui.postWidget->setDisabled(true);

	connect(ui.post_check, &QCheckBox::stateChanged, [&] {
				
			ui.postWidget->setEnabled(ui.post_check->isChecked());
		
	});

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

#include "Model/Dental/Procedure.h"

void ObturationView::setData(const ProcedureObtData& data)
{

	ui.surfaceSelector->setSurfaces(data.surfaces);
	ui.obturationWidget->setData(data.data);

	ui.post_check->setChecked(data.post.has_value());

	if (data.post.has_value())
	{
		ui.postWidget->setData(data.post.value());
	}
}

ProcedureObtData ObturationView::getData()
{
	return ProcedureObtData{
		.surfaces = ui.surfaceSelector->getSurfaces(),

		.post = ui.post_check->isChecked() ?
			ui.postWidget->getData() 
			: 
			std::optional<PostData>{},

		.data = ui.obturationWidget->getData()
	};


}
