#pragma once

#include <QPushButton>
#include "View/uiComponents/FurcationWidget.h"
#include "View/uiComponents/PerioButton.h"
#include "View/uiComponents/PerioSpinBox.h"
#include "View/Graphics/PerioGraphicsButton.h"
#include "Model/Dental/PerioToothData.h"

struct ToothUi
{
    QPushButton* tooth;
    PerioStateButton* mobi;
    FurcationWidget* furcation;
    PerioSpinBox* recession[2];
    PerioSpinBox* attachment[2];
    PerioSpinBox* pd[6];
    PerioSpinBox* cal[6];
    PerioSpinBox* gm[6];
    PerioButton* bop[6];
    PerioGraphicsButton* FMBS[4];
    PerioGraphicsButton* FMPS[4];

    void disable(bool disabled)
    {
		tooth->setChecked(!disabled);
		mobi->setDisabled(disabled);
		furcation->setDisabled(disabled);

		for (int i = 0; i < 4; i++){
			FMBS[i]->setDisabled(disabled);
			FMPS[i]->setDisabled(disabled);
		}


		attachment[0]->disable(disabled);
		attachment[1]->disable(disabled);

		recession[0]->disable(disabled);
		recession[1]->disable(disabled);

		for (int i = 0; i < 6; i++){
			bop[i]->setDisabled(disabled);
			pd[i]->disable(disabled);
			cal[i]->disable(disabled);
			gm[i]->disable(disabled);
		}
		
    }

	void setData(const PerioToothData data)
	{
		disable(false);
		mobi->setState(data.mobility);
		furcation->setMeasurment(data.furc[0], data.furc[1], data.furc[2]);

		attachment[0]->setValueCustom(data.attachment[0]);

		data.toothIndex < 16 ?			//upper teeth have no attached palatal gingiva
		attachment[1]->disable(true)
		:
		attachment[1]->setValueCustom(data.attachment[1]);

		recession[0]->setValueCustom(data.recession[0]);
		recession[1]->setValueCustom(data.recession[1]);

		for (int i = 0; i < 6; i++)
		{
			QSignalBlocker bopBlocker(bop[i]);

			bop[i]->setChecked(data.bop[i]);
			cal[i]->setValueCustom(data.cal[i]);
			pd[i]->setValueCustom(data.pd[i]);
			gm[i]->setValueCustom(data.gm[i]);
		}

		for (int i = 0; i < 4; i++)
		{
			FMBS[i]->setChecked(data.fmbs[i]);
			FMPS[i]->setChecked(data.fmps[i]);
		}


	}
};
