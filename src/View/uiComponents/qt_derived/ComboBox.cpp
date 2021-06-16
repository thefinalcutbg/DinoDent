#include "ComboBox.h"

ComboBox::ComboBox(QWidget *parent)
	: QComboBox(parent)
{
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

