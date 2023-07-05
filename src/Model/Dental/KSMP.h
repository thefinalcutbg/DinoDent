#pragma once
/*
#include <utility>
#include <string>
#include <vector>

enum class ProcedureType;
enum class ProcedureTemplateType;

struct KSMP
{

	std::string code;
	std::string name;
	int block;
	int chapter;
	int type{ 0 };

	static void initialize();
	static const std::vector<const KSMP*>& getByType(ProcedureType type);
	static const std::vector<const KSMP*>& getByType(ProcedureTemplateType type);
	static const std::string& blockName(int block);
	static const std::string& chapterName(int chapter);
	static const std::string& getName(const std::string& code);
	static const KSMP& getByCode(const std::string& code);
	static bool isValid(const std::string& code);

};

typedef const std::vector<const KSMP*> KsmpList;

*/