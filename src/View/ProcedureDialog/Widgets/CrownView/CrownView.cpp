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

	connect(ui.rangeWidget, &RangeWidget::widgetChecked, [=](bool checked)
		{
			if (m_presenter == nullptr) return;
			m_presenter->rangeWidgetChecked(checked); 
		});
	
}

CrownView::~CrownView()
{

}

AbstractRangeEdit* CrownView::rangeWidget()
{
	return ui.rangeWidget;
}

AbstractLineEdit* CrownView::materialEdit()
{
	return ui.materialEdit;
}

int CrownView::getType()
{
	return ui.comboBox->currentIndex();
}

int CrownView::getColor()
{
	return ui.vitaWidget->getIndex();
}

void CrownView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

void CrownView::setData(const BridgeData& data)
{
	ui.rangeWidget->disable(false);
	ui.rangeWidget->setRange(data.tooth_begin, data.tooth_end);
	ui.comboBox->setCurrentIndex(data.crown.prep_type);
	ui.materialEdit->set_Text(data.crown.material);
	ui.vitaWidget->setIndex(data.crown.color);
}

void CrownView::setData(const CrownData& data)
{
	ui.rangeWidget->disable(true);
	ui.comboBox->setCurrentIndex(data.prep_type);
	ui.materialEdit->set_Text(data.material);
	ui.vitaWidget->setIndex(data.color);
}

CrownData CrownView::getData()
{

	return CrownData
	{
		ui.materialEdit->getText(),
		ui.comboBox->currentIndex(),
		ui.vitaWidget->getIndex()
	};
}


void CrownView::setPresenter(CrownPresenter* presenter)
{
	m_presenter = presenter;
}
