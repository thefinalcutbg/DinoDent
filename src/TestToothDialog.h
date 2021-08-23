#pragma once

#include "Model/Tooth/Tooth.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/DetailsPresenter/DetailedStatusPresenter.h"

void TestToothDialog()
{
	Tooth tooth;
	tooth.setIndex(4);
	tooth.obturation.setDefaultSurface(0);
	tooth.caries.setDefaultSurface(0);
	
	DetailedStatusPresenter p(tooth, "8903261129");
	p.open();

}