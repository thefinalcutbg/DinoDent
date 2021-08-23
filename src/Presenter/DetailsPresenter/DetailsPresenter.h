#pragma once

#include "SubPresenters/DetailedStatusPresenter.h"
#include "Model/Tooth/Tooth.h"
#include <optional>
#include <memory>

class IDetailsView;
class DetailedStatusController;


class DetailsPresenter
{
	StatusType m_category{ StatusType::general };
	int m_code{ -1 };
	CheckModel m_checkModel;

	std::unique_ptr<DetailedStatusController> controller;

	const std::string& patientID;

	Tooth tooth;

	IDetailsView* view{ nullptr };
	std::optional<Tooth> _result {};

	//SubPresenters:
	DetailedStatusPresenter m_detailedStatus;


public:
	DetailsPresenter(const Tooth& tooth, const std::string& patientID);

	void okPressed();
	void setView(IDetailsView* view);

	std::optional<Tooth> open();

	~DetailsPresenter();

};

