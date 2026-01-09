#pragma once
#include <string>
#include <vector>

namespace DbNotes
{
	std::string getNote(long long patientRowId, int toothIdx);
	bool saveNote(const std::string& note, long long patientRowId, int toothIdx);

    enum TemplateType {ProcedureNote, PlanStage, PlannedProcedure, PlanConclusion, Supplements};

    std::vector<std::string> getTemplateNotes(TemplateType type);
    void setTemplateNotes(const std::vector<std::string>& notes, TemplateType type);
    void insertTemplateNote(const std::string& note, TemplateType type);
};

