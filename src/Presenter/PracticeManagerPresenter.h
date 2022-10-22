#pragma once

#include "View/Interfaces/IPracticeSelectorView.h"
#include "Model/UserStructs.h"

class PracticeManagerPresenter
{
	IPracticeSelectorView* view{ nullptr };
	std::vector<PracticePair> practices;

public:
	PracticeManagerPresenter();
	void setView(IPracticeSelectorView* view);
	void addClicked();
	void removeClicked(int idx);
	void editClicked(int idx);
	void dbChangePath();
};