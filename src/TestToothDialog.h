#pragma once

#include "Model/Tooth/Tooth.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/DetailsPresenter/DetailsPresenter.h"

void TestToothDialog()
{
	Tooth tooth;

	DetailsPresenter p(tooth);
	p.open();

}