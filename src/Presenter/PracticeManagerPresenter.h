#pragma once

#include "Model/UserStructs.h"

class PracticeSelectorView;

class PracticeManagerPresenter
{
	PracticeSelectorView* view{ nullptr };
	std::vector<PracticePair> practices;

public:
	PracticeManagerPresenter();
	void setView(PracticeSelectorView* view);
	void addClicked();
	void removeClicked(int idx);
	void dbChangePath();
};
