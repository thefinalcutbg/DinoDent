#include "ObturationView.h"

static std::array<QString, 17> colorCodes
{
	"",
	"A1", "A2", "A3", "A3,5", "A4",
	"B1", "B2", "B3", "B4",
	"C1", "C2", "C3", "C4",
	"D2", "D3", "D4"
};

ObturationView::ObturationView(QWidget* parent)
	: QWidget(parent), m_presenter(nullptr)
{
	ui.setupUi(this);

	for (QString code : colorCodes)
	{
		ui.combo_color->addItem(code);
	}
}

ObturationView::~ObturationView()
{
}

void ObturationView::setPresenter(ObturationPresenter* presenter)
{
	this->m_presenter = presenter;
}

AbstractLineEdit* ObturationView::material()
{
	return ui.materialEdit;
}

AbstractSurfaceSelector* ObturationView::surfaceSelector()
{
	return ui.surfaceSelector;
}

std::tuple<int, bool> ObturationView::getColor()
{
	return std::tuple<int, bool>(ui.combo_color->currentIndex(), false);
}

bool ObturationView::hasPost()
{
	return ui.post_check->isChecked();
}

void ObturationView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

#include "Model/Procedure/Procedure.h"

void ObturationView::setData(const ObturationData& data)
{
	ui.materialEdit->set_Text(data.material);
	ui.combo_color->setCurrentIndex(data.color);
	ui.post_check->setChecked(data.post);
	ui.surfaceSelector->setSurfaces(data.surfaces);
}

ObturationData ObturationView::getData()
{
	return ObturationData
	{
		ui.surfaceSelector->getSurfaces(),
		ui.post_check->isChecked(),
		ui.combo_color->currentIndex(),
		ui.materialEdit->getText()
	};
}
