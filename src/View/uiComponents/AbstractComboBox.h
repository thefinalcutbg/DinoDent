#pragma once
#include "Presenter/PatientDialog/uiObserver.h"

class AbstractComboBox
{
protected:
	uiObserver* observer;
public:
	AbstractComboBox() : observer(0) {};
	virtual int getIndex() = 0;
	virtual void setIndex(int index) = 0;
	void setObserver(uiObserver* observer) { this->observer = observer; }
	virtual void disable(bool disable) = 0;
};

