#pragma once
#include <vector>
#include "Model/Tooth/ToothUtils.h"
#include <sstream>


class AutoComplete
{
	static ToothUtils utils;
	
	std::stringstream diagnosisStream;
	
public:

    std::string getBridgeDiag(int begin, int end, const std::array<Tooth, 32>& teeth);

	static std::string getExtrDiag(const Tooth& tooth);
	static std::string getObturDiag(const Tooth& tooth);
	static std::string getEndoDiag(const Tooth& tooth);
	static std::string getCrownDiag(const Tooth& tooth);
	static std::array<bool, 6> getSurfaces(const Tooth& tooth);
	static std::tuple<int, int> getInitialBridgeRange(const std::vector<Tooth*>& selectedTeeth);
};

