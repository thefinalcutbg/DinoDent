#include "PlainTable.h"
#include "FreeFunctions.h"

void PlainTable::addEmptyRow()
{
    for (auto& column : data) {
        column.rows.push_back(PlainCell{""});
    }
}

PlainTable::PlainTable(const std::vector<Procedure>& pList)
{
    addColumn({"Дата",100,PlainColumn::Right });
    addColumn({"Диагноза", 200 });
    addColumn({"Зъб", 70, PlainColumn::Center });
    addColumn({"Манипулация", 300 });
    addColumn({"Код", 50, PlainColumn::Center });

    for (auto& p : pList) {

        addCell(0, {
            .data = p.date.toBgStandard(),
            .icon = p.isSentToHis() ? CommonIcon::HIS : CommonIcon::NOICON
            });

        addCell(1, { p.diagnosis.getFullDiagnosis() });
        addCell(2, { p.getToothString() });
        addCell(3, { p.code.name() });

        CommonIcon::Type icon;

        switch (p.financingSource) {
        case FinancingSource::NHIF: icon = CommonIcon::NHIF; break;
        case FinancingSource::PHIF: icon = CommonIcon::PHIF; break;
        default: icon = CommonIcon::NOICON;
        }

        addCell(4, PlainCell{
            .data = std::to_string(p.code.oldCode()),
            .icon = icon
            });
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
    addColumn({"Дозировка"});

    for (auto& m : mList)
    {
        addCell(0, { m.name() });

        std::string dosages;

        for (auto& d : m.dosageList())
        {
            dosages.append(d);
            dosages += "; ";
        }
        addCell(1, { dosages });

    }
}
