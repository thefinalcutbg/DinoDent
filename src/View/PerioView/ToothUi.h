#pragma once

#include <QPushButton>
#include "Widgets/FurcationWidget.h"
#include "Widgets/PerioButton.h"
#include "Widgets/PerioSpinBox.h"
#include "PerioGraphics/PerioGraphicsButton.h"
#include "Model/PerioToothData.h"

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

		attachment[0]->setValue(data.attachment[0]);

		data.toothIndex < 16 ?			//upper teeth have no attached palatal gingiva
		attachment[1]->disable(true)
		:
		attachment[1]->setValue(data.attachment[1]);

		recession[0]->setValue(data.recession[0]);
		recession[1]->setValue(data.recession[1]);

		for (int i = 0; i < 6; i++)
		{
			QSignalBlocker bopBlocker(bop[i]);
			QSignalBlocker calBlocker(cal[i]);
			QSignalBlocker pdBlocker(pd[i]);
			QSignalBlocker gmBlocker(gm[i]);

			bop[i]->setChecked(data.bop[i]);
			cal[i]->setValue(data.cal[i]);
			pd[i]->setValue(data.pd[i]);
			gm[i]->setValue(data.gm[i]);
		}

		for (int i = 0; i < 4; i++)
		{
			FMBS[i]->setChecked(data.fmbs[i]);
			FMPS[i]->setChecked(data.fmps[i]);
		}


	}
};
