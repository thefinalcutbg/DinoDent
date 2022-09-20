#include "FiberSplintView.h"
#include "Model/Dental/Procedure.h"

FiberSplintView::FiberSplintView(QWidget *parent)
	: QWidget(parent),
	  m_presenter(nullptr)
{
	ui.setupUi(this);


	connect(ui.rangeWidget, &RangeWidget::rangeChanged, [=](int begin, int end)
		{
			if (m_presenter == nullptr) return;
			m_presenter->rangeChanged(begin, end);
		});
}

FiberSplintView::~FiberSplintView()
{
}

AbstractRangeEdit* FiberSplintView::rangeWidget()
{
	return ui.rangeWidget;
}

void FiberSplintView::set_hidden(bool hidden)
{
	setHidden(hidden);
}

void FiberSplintView::setData(const ProcedureFiberData& data)
{
	ui.rangeWidget->setBridgeRange(data.tooth_begin, data.tooth_end);
	ui.obturationWidget->setData(data.obtur);
}

ObturationData FiberSplintView::getData()
{
	return ui.obturationWidget->getData();
}

void FiberSplintView::setPresenter(FiberSplintPresenter* presenter)
{
	m_presenter = presenter;
}
