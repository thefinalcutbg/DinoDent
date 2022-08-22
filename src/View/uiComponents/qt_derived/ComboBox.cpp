#include "ComboBox.h"
#include <qfile.h>
#include <qabstractitemview.h>
#include "View/ModalDialogBuilder.h"
#include "View/Theme.h"
ComboBox::ComboBox(QWidget* parent)
	: QComboBox(parent)
{}

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
	setDisabled(disable);
	
}

