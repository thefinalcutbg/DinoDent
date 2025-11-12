#include "PlainTable.h"
#include "FreeFunctions.h"

void PlainTable::addEmptyRow()
{
    for (auto& column : data) {
        column.rows.push_back(PlainCell{""});
    }
}

void PlainTable::setIndicatorToLastRow(const std::string& colorName)
{
    if (data.empty()) return;

    if (data[0].rows.empty()) return;

    QColor color(colorName.c_str());

    if (!color.isValid()) return;

    indicator_row_map[data[0].rows.size() - 1] = color;
}

PlainTable::PlainTable(const std::vector<Procedure>& pList, bool keepNotes)
{
    addColumn({"Дата",110,PlainColumn::Right });
    addColumn({"Диагноза", 180 });
    addColumn({"Зъб", 70, PlainColumn::Center });
    addColumn({"Процедура", 280 });
    addColumn({"КСМП", 100, PlainColumn::Center });
    addColumn({"НЗОК", 40, PlainColumn::Center });
    addColumn({ "Бележки", 100, PlainColumn::Left });

    bool hasNhif = false;

    for (auto& p : pList) {

        addCell(0, {
            .data = p.date.toBgStandard(),
            .icon = p.isSentToHis() ? CommonIcon::HIS : CommonIcon::NOICON
            });

        addCell(1, { p.diagnosis.getDiagnosisText() });
        addCell(2, { p.getToothString() });
        addCell(3, { p.code.name() });

        CommonIcon::Type icon;

        switch (p.financingSource) {
        case FinancingSource::NHIF: icon = CommonIcon::NHIF; break;
        case FinancingSource::PHIF: icon = CommonIcon::PHIF; break;
        case FinancingSource::Patient: icon = CommonIcon::PATIENT; break;
        default: icon = CommonIcon::NOICON;
        }

        addCell(4, PlainCell{
            .data = p.code.ACHICode().c_str(),
            .icon = icon
            });

        bool isNhif = p.financingSource == FinancingSource::NHIF;

        addCell(5, PlainCell{
            .data = isNhif ? std::to_string(p.code.nhifCode()) : "",});

        addCell(6, PlainCell{ .data = p.notes });

        if (isNhif) {
            hasNhif = true;
        }
    }

    //removing nhif
    if (!hasNhif) {
        data[data.size() - 2] = data[data.size() - 1];
        data.pop_back();
    }

    if (!keepNotes) {
        data.pop_back();
    }
}

PlainTable::PlainTable(const std::vector<BusinessOperation>& bList)
{
    addColumn({"Код",100,PlainColumn::Center });
    addColumn({"Наименование", 400 });
    addColumn({"Кол-во", 70, PlainColumn::Center });
    addColumn({"Ед. цена", 70, PlainColumn::Center });
    addColumn({"Сума", 70, PlainColumn::Center });

    for (auto& op : bList)
    {
        addCell(0, { op.activity_code });
        addCell(1, { op.activity_name });
        addCell(2, { std::to_string(op.quantity) });
        addCell(3, { FreeFn::formatDouble(op.unit_price) });
        addCell(4, { FreeFn::formatDouble(op.value_price) });
    }

}

PlainTable::PlainTable(const std::vector<Medication>& mList)
{
    addColumn({"Медикамент", 350 });
    addColumn({"Инструкции за прием"});

    for (auto& m : mList)
    {
        addCell(0, { m.name() });

        addCell(1, { m.parseFullDosage() });

    }
}
