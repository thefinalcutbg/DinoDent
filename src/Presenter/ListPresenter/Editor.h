#pragma once


class EditObserver
{
public:
	virtual void editNotify() = 0;
};

class Editor
{
	EditObserver* edit_observer{ nullptr };


protected:
	void makeEdited();

public:
	virtual void attachEditObserver(EditObserver* observer);
};

