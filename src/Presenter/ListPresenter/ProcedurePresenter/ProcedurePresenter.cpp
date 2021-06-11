#include "ProcedurePresenter.h"

#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"

#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Model/Tooth/ToothUtils.h"

#include "Model/Manipulation/MasterNZOK.h"
#include "Model/Manipulation/CustomProcedures.h"
#include "Model/User/User.h"

#include "Model/Manipulation/Manipulation.h"

#include "View/ListView/IProcedureView.h"


ProcedurePresenter::ProcedurePresenter()
    :
    view(nullptr),
    _ambList(nullptr),
    _patient(nullptr),
    _selectedTeeth(nullptr)
{
}

void ProcedurePresenter::addToProcedureList(const std::vector<Manipulation>& new_mList)
{

    auto& mList = _ambList->manipulations;

    mList.reserve(mList.size() + new_mList.size());

    for (int i = 0; i < mList.size(); i++)
    {
        if (new_mList[0].date < mList[i].date)
        {
            mList.insert(mList.begin() + i, new_mList.begin(), new_mList.end());
            return;
        }
    }

    //if list is empty, or date is max
    for (auto& mInsert : new_mList)
    {
        mList.push_back(mInsert);
    }

}

void ProcedurePresenter::refreshProcedureView()
{
    if (view == nullptr) return;

    auto& mList = _ambList->manipulations;

    double patientPrice(0);
    double nzokPrice(0);

    std::vector<RowData> rows;
    rows.reserve(mList.size());

    for (auto& m : mList)
    {
        int toothNumber{ m.tooth }, toothIndex{ m.tooth };
        
        if (toothNumber >= 0 && toothNumber < 32)
        {
            toothNumber = ToothUtils::getToothNumber(toothNumber, _ambList->teeth[toothIndex].temporary.exists());
        }

        rows.emplace_back
        (
            RowData
            {
                Date::toString(m.date),
                m.diagnosis,
                toothNumber,
                toothIndex,
                m.name,
                m.code,
                m.price,
                m.nzok
            }
        );

        patientPrice = patientPrice + m.price;

        if (m.nzok)
        {
            auto[p, nzok] = MasterNZOK::instance().getPrices(m.code, _ambList->date, CurrentUser::instance().specialty, _patient->isAdult(m.date), _ambList->unfavourable);
            nzokPrice =nzokPrice+nzok;
        }
            
    }

    view->setProcedures(rows);

   
}


void ProcedurePresenter::setData(AmbList& amb_list, Patient& patient, std::vector<Tooth*>& selectedTeeth)
{
	_ambList = &amb_list;
	_selectedTeeth = &selectedTeeth;
    _patient = &patient;
    
    view->setUnfav(_ambList->unfavourable);

    refreshProcedureView();

}

void ProcedurePresenter::setView(IProcedureView* view)
{
	this->view = view;
	view->setProcedurePresenter(this);
}

void ProcedurePresenter::addProcedure()
{
    if (view == nullptr) return;

    auto mNzokTemplate = MasterNZOK::instance().getM_Templates(_ambList->date, CurrentUser::instance().specialty, _patient->isAdult(), _ambList->unfavourable);
    auto mCustomTemplate = CustomProcedures::instance().getCustomProcedures();

    mNzokTemplate.insert(mNzokTemplate.end(), mCustomTemplate.begin(), mCustomTemplate.end());

    ProcedureDialogPresenter p
    {
        mNzokTemplate,
        *_selectedTeeth,
        _ambList->teeth,
        _ambList->date
    };

    auto newList = p.openDialog();

    if (newList.empty()) return;

        this->addToProcedureList(newList);
        refreshProcedureView();
        makeEdited();

}

void ProcedurePresenter::deleteProcedure(int index)
{
    if (!_ambList->manipulations.size()) return;
        
    _ambList->manipulations.erase(_ambList->manipulations.begin() + index);

    refreshProcedureView();
    makeEdited();
}

void ProcedurePresenter::setSelectedProcedure(int index)
{
}

void ProcedurePresenter::setUnfavourable(bool unfav)
{
    _ambList->unfavourable = unfav;

    for (auto& m : _ambList->manipulations)
    {
        if (m.nzok)
        {
            m.price = std::get<0>(

                    MasterNZOK::instance().getPrices
                    (
                        m.code,
                        _ambList->date,
                        CurrentUser::instance().specialty,
                        _patient->isAdult(m.date),
                        unfav
                    )
                );
        }
    }

    makeEdited();

    refreshProcedureView();
}
