#pragma once
#include <utility>
#include <string>
#include <vector>

enum class ProcedureType;
enum class ProcedureTemplateType;

struct KsmpDetails
{
	std::string code;
	std::string name;
	int block;
	int chapter;
	int type{ 0 };
};

typedef const std::vector<const KsmpDetails*> KsmpList;

namespace KSMP
{
	void initialize();
	KsmpList& getByType(ProcedureType type);
	KsmpList& getByType(ProcedureTemplateType type);
	const std::string& blockName(int block);
	const std::string& chapterName(int chapter);
	const KsmpDetails& getByCode(const std::string code);

}