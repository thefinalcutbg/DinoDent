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

void CrownView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

void CrownView::setData(const ProcedureBridgeData& data)
{
	ui.rangeWidget->disable(false);
	ui.rangeWidget->setBridgeRange(data.tooth_begin, data.tooth_end);
	ui.crownWidget->setData(data.crown);
}

void CrownView::setData(const CrownData& data)
{
	ui.rangeWidget->disable(true);
	ui.crownWidget->setData(data);
}

CrownData CrownView::getData()
{
	return ui.crownWidget->getData();
}


void CrownView::setPresenter(CrownPresenter* presenter)
{
	m_presenter = presenter;
}
