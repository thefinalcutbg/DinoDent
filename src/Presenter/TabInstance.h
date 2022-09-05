#pragma once

#include "View/Interfaces/ITabView.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Patient.h"
#include <memory>

enum class TabType {AmbList, PerioList, Perscription, PatientSummary, Financial};

struct TabName {
	std::string header;
	std::string footer;
	bool nzok{ false };
	std::string toString() { return header + " " + footer; };
};

class TabInstance
{

	ScrollPos m_scrollPos;
	ITabView* _tabView;
	void setScrollPosition() { _tabView->setScrollPos(m_scrollPos); }

protected:


	bool edited{ false };

	void makeEdited()
	{
		if (edited) return;

		edited = true;
		auto tabName = getTabName();
		tabName.header = "*" + tabName.header;
		_tabView->changeTabName(tabName);
	}

	void refreshTabName() { _tabView->changeTabName(getTabName()); }

	virtual void setDataToView() = 0;


public:
	const TabType type;

	std::shared_ptr<Patient> patient;

	TabInstance(ITabView* tabView, TabType type, std::shared_ptr<Patient> patient) : _tabView(tabView), type(type), patient(patient) {  };



	bool requiresSaving() { return edited || isNew(); }

	virtual long long rowID() const = 0;
	virtual bool save() = 0;

	bool close()
	{
		if (!isNew() && !edited) return true;

		DialogAnswer answer = ModalDialogBuilder::openSaveDialog(getTabName().toString());

		switch (answer)
		{
		case DialogAnswer::Yes:
			//is save interrupted?
			return save() ? true : false; 

		case DialogAnswer::No: return true;

		case DialogAnswer::Cancel: return false;
		}
	}

	virtual void print() = 0;



	void setCurrent() {

		setDataToView();

		switch (type)
		{
			case TabType::AmbList: _tabView->showListView(); break;
			case TabType::PerioList: _tabView->showPerioView(); break;
			case TabType::PatientSummary: _tabView->showSummaryView(); break;
			case TabType::Financial:_tabView->showFinancialView(); break;
			case TabType::Perscription:_tabView->showPerscriptionView(); break;
		}

		setScrollPosition();
	}

	virtual bool isNew() = 0;
	virtual TabName getTabName() = 0;




public:


	virtual void prepareSwitch()
	{
		m_scrollPos = _tabView->getScrollPos();
	};

	virtual ~TabInstance() {};

};