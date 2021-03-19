#include "ManipulationGenerator.h"

ManipulationGenerator::ManipulationGenerator() 
	: 
	extraction_factory(AutoComplete::getExtrDiag),
    crown_factory(AutoComplete::getCrownDiag),
    endo_factory(AutoComplete::getEndoDiag)
{
}

ManipulationFactoryProducts ManipulationGenerator::getManipulations(DialogData data)
{
	Manipulation manipulation
	{ 
		data.mTemplate, 
		data.date, 
		data.mName, 
		data.mDiagnosis, 
		data.price 
	};

	switch (manipulation.type)
	{

		case ManipulationType::general:
		{
			return { std::vector<Manipulation>{manipulation}, std::vector<int>{} };
		}

		case ManipulationType::any:
		{
			std::vector<Manipulation> mList;
			mList.reserve(data.selectedTeeth.size());

			for (int i = 0; i < data.selectedTeeth.size(); i++)
			{
				mList.push_back(manipulation);
				mList[i].tooth = data.selectedTeeth[i]->index;
			};
			return { mList, std::vector<int>{} };
		}

		case ManipulationType::bridge:
		{
			manipulation.material = data.mMaterial;
			auto& range = data.range;
			manipulation.additional = BridgeParam{ std::get<0>(range), std::get<1>(range) };
			return { std::vector<Manipulation>{manipulation}, std::vector<int>{} };
		}
		case ManipulationType::obturation:
			manipulation.material = data.mMaterial;
			return obturation_factory.getManipulations(data.selectedTeeth, manipulation);
		case ManipulationType::extraction:
			return extraction_factory.getManipulations(data.selectedTeeth, manipulation);
		case ManipulationType::crown:
			return crown_factory.getManipulations(data.selectedTeeth, manipulation);
		case ManipulationType::endo:
			return endo_factory.getManipulations(data.selectedTeeth, manipulation);
	}

    return ManipulationFactoryProducts();
}
