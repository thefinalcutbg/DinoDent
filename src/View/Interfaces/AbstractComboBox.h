#pragma once

class AbstractComboBox
{
protected:

public:
	AbstractComboBox() {};
	virtual int getIndex() = 0;
	virtual void setIndex(int index) = 0;

	virtual void disable(bool disable) = 0;
};

