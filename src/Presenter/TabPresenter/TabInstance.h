#pragma once

#include "View/TabView/ITabView.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Patient.h"
#include <memory>

enum class TabType {AmbList, PerioList, PatientSummary, Financial};

struct TabName {
	std::string header;
	std::string footer;
	std::string toString() { return header + " " + footer; };
};

class TabInstance
{

	ScrollPos m_scrollPos;

protected:
	ITabView* _tabView;

	void makeEdited()
	{
		if (edited) return;

		edited = true;
		auto tabName = getTabName();
		tabName.header = "*" + tabName.header;
		_tabView->changeTabName(tabName);
	}

public:
	const TabType type;

	std::shared_ptr<Patient> patient;

	TabInstance(ITabView* tabView, TabType type, std::shared_ptr<Patient> patient) : _tabView(tabView), type(type), patient(patient) {  };

	bool edited{ false };

	virtual const std::string& rowID() const = 0;
	virtual bool save() = 0;
	virtual bool saveAs() = 0;

	bool close()
	{
		if (isNew() || edited)
		{
			DialogAnswer answer = ModalDialogBuilder::openSaveDialog(getTabName().toString());

			switch (answer)
			{
			case DialogAnswer::Yes:
				if (save()) //if the save is not interrupted
					break;
				else
					return false;

			case DialogAnswer::No: break;

			case DialogAnswer::Cancel: return false;
			}
		}

		return true;
	}

	virtual void print() = 0;
	virtual void setCurrent() = 0;
	virtual bool isNew() = 0;
	virtual TabName getTabName() = 0;

protected:

	void setScrollPosition() {_tabView->setScrollPos(m_scrollPos);}


public:


	virtual void prepareSwitch()
	{
		m_scrollPos = _tabView->getScrollPos();
	};

	virtual ~TabInstance() {};

};