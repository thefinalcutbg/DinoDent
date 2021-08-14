#pragma once

#include "Model/Tooth/Tooth.h"
#include <optional>

class IDetailsView;

class DetailsPresenter
{
	Tooth tooth;

	IDetailsView* view{ nullptr };
	std::optional<Tooth> _result {};

public:
	DetailsPresenter(const Tooth& tooth);

	void setView(IDetailsView* view);

	std::optional<Tooth> open();

};

