#include "ComboBox.h"

ComboBox::ComboBox(QWidget *parent)
	: QComboBox(parent)
{
	connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index)
		{
			if (observer != NULL) observer->notify(Notification::edited);
		});
}

ComboBox::~ComboBox()
{
}

int ComboBox::getIndex()
{
	return currentIndex();
}

void ComboBox::setIndex(int index)
{
	blockSignals(true);
	setCurrentIndex(index);
	blockSignals(false);
}

void ComboBox::disable(bool disable)
{
	if (disable) setDisabled(1);
	else setDisabled(0);
}

