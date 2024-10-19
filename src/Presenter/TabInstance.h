#pragma once

#include "View/Interfaces/ITabView.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Patient.h"
#include <memory>
#include "View/CommonIcon.h"

enum class TabType { PatientSummary, AmbList, Prescription, PerioStatus, Financial, Calendar};

struct TabName {
	std::string header;
	std::string footer;
	CommonIcon::Type header_icon{ CommonIcon::NOICON };
	CommonIcon::Type footer_icon{ CommonIcon::NOICON };
	std::string toString() const { return header + " " + footer; };
};

class TabInstance
{

	ScrollPos m_scrollPos;
	ITabView* _tabView;
	void setScrollPosition() { _tabView->setScrollPos(m_scrollPos); }

	int m_containerIdx{ -1 }; //used to change the tab name even if not current

	bool is_current{ false };

protected:


	bool edited{ false };

	virtual void makeEdited();

	void refreshTabName();

	virtual void setDataToView() = 0;

	virtual void prepareDerivedForSwitch() {};


public:
	const TabType type;

	std::shared_ptr<Patient> patient;

	TabInstance(ITabView* tabView, TabType type, std::shared_ptr<Patient> patient) : _tabView(tabView), type(type), patient(patient) {  };

	void setContainerIdx(int idx) { m_containerIdx = idx;}

	bool requiresSaving() { return edited || isNew(); }

	bool premissionToClose();

	void setCurrent();
	bool isCurrent() { return is_current; }
	virtual bool isNew() = 0;
	virtual TabName getTabName() = 0;
	virtual bool save() = 0;
	virtual void print() = 0;
	virtual long long rowID() const = 0;

public:

	virtual void prepareSwitch();
	virtual ~TabInstance() {};

};