#include "PostWidget.h"
#include "Model/Dental/StatusData.h"

PostWidget::PostWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (auto& str : PostType::getNames())
	{
		ui.postTypeCombo->addItem(str);
	}

}

PostData PostWidget::getData()
{
	return PostData{ .type = ui.postTypeCombo->currentIndex() };
}

void PostWidget::setData(const PostData& data)
{
	ui.postTypeCombo->setCurrentIndex(data.type.getIndex());
}

PostWidget::~PostWidget()
{}
