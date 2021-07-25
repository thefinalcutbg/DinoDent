#pragma once

#include "Model/Tooth/Tooth.h"
#include <optional>


class IDetailsView;

class DetailsPresenter
{
	
	IDetailsView* view{ nullptr };
	std::optional<Tooth> _tooth {};

public:
	DetailsPresenter();

	void setView(IDetailsView* view);

	std::optional<Tooth> open(const Tooth& tooth);

};

