#include "CrownView.h"

CrownView::CrownView(QWidget *parent)
	: QWidget(parent),
	m_presenter(NULL)
{
	ui.setupUi(this);

	connect(ui.rangeWidget, &RangeWidget::rangeChanged, [=](int begin, int end)
		{ m_presenter->rangeChanged(begin, end);});

	connect(ui.rangeWidget, &RangeWidget::widgetChecked, [=](bool checked)
		{ m_presenter->rangeWidgetChecked(checked); });
	
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

std::tuple<int, bool> CrownView::getColor()
{
	return ui.vitaWidget->getIndex();
}

void CrownView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

void CrownView::setPresenter(CrownPresenter* presenter)
{
	m_presenter = presenter;
}
