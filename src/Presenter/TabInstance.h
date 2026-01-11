#pragma once

#include "View/ModalDialogBuilder.h"
#include "Model/Patient.h"
#include <memory>
#include "View/CommonIcon.h"
#include "Model/TabType.h"

class TabView;

struct TabName {
	std::string header;
	std::string footer;
	CommonIcon::Type header_icon{ CommonIcon::NOICON };
	CommonIcon::Type footer_icon{ CommonIcon::NOICON };
	std::string indicatorColor;
	std::string toString() const { return header + " - " + footer; };
};



class TabInstance
{

	std::pair<int, int> m_scrollPos;

	void setScrollPosition();

	int m_containerIdx{ -1 }; //used to change the tab name even if not current

	bool is_current{ false };

protected:

	TabView* _tabView;

	bool edited{ false };

	void refreshTabName();

	virtual void setDataToView() = 0;

	virtual void prepareDerivedForSwitch() {};

	virtual void makeEdited();

public:

	virtual void patientDataChanged() {};

	const TabType type;

	std::shared_ptr<Patient> patient;

	TabInstance(TabView* tabView, TabType type, std::shared_ptr<Patient> patient) : _tabView(tabView), type(type), patient(patient) {  };

	void setContainerIdx(int idx) { m_containerIdx = idx;}

	bool requiresSaving() { return edited || isNew(); }

	bool premissionToClose();

	void setCurrent();
	bool isCurrent() { return is_current; }
	virtual bool isNew() = 0;
	virtual TabName getTabName() = 0;
	virtual bool save() = 0;
	virtual void print() = 0;
	virtual void pdfPrint() {}
	virtual long long rowID() const = 0;

public:

    virtual void prepareSwitch();
	virtual ~TabInstance() {};

};
