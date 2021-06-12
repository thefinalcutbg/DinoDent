#pragma once

class IPatientDialog
{

public:

	virtual void close() = 0;
	virtual void setLn4View(bool show) = 0;
	virtual void open(bool editMode) = 0;
	
};

