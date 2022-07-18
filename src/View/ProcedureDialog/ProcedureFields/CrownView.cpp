#include "CrownView.h"

CrownView::CrownView(QWidget *parent)
	: QWidget(parent),
	m_presenter(nullptr)
{
	ui.setupUi(this);

	connect(ui.rangeWidget, &RangeWidget::rangeChanged, [=](int begin, int end)
		{ 
			if(m_presenter == nullptr) return;  
			m_presenter->rangeChanged(begin, end); 
		});
	
	connect(ui.bridgeCheckBox, &QCheckBox::toggled, [=](bool checked)
		{
			ui.rangeWidget->disable(!checked);

			if (m_presenter)m_presenter->selectAsBridge(checked);
		});

	ui.rangeWidget->disable(true);
}

CrownView::~CrownView()
{

}

AbstractRangeEdit* CrownView::rangeWidget()
{
	return ui.rangeWidget;
}

void CrownView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

void CrownView::setData(const ProcedureBridgeData& data)
{
	ui.bridgeCheckBox->setChecked(true);
	ui.rangeWidget->disable(false);
	ui.rangeWidget->setBridgeRange(data.tooth_begin, data.tooth_end);
	ui.crownWidget->setData(data.crown);
}

void CrownView::setData(const CrownData& data)
{
	ui.bridgeCheckBox->setChecked(false);
	ui.rangeWidget->disable(true);
	ui.crownWidget->setData(data);
}

void CrownView::setMaterial(const std::string& material)
{
	ui.crownWidget->ui.materialEdit->setText(material.c_str());
}

CrownData CrownView::getData()
{
	return ui.crownWidget->getData();
}

void CrownView::lockBridgeCheckbox(bool asChecked = true)
{
	ui.bridgeCheckBox->setChecked(asChecked);
	ui.bridgeCheckBox->setDisabled(true);
}


void CrownView::setPresenter(CrownPresenter* presenter)
{
	m_presenter = presenter;
}
