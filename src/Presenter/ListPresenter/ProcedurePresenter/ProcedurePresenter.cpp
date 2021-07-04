#include "ProcedurePresenter.h"

#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureEditorPresenter.h"

#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Model/Tooth/ToothUtils.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Procedure/CustomProcedures.h"
#include "Model/User/User.h"
#include "Model/Procedure/Procedure.h"

#include "View/ListView/IProcedureView.h"
#include "View/uiComponents/AbstractComboBox.h"

ProcedurePresenter::ProcedurePresenter()
    :
    view(nullptr),
    _ambList(nullptr),
    _patient(nullptr),
    _selectedTeeth(nullptr),
    _index(-1)
{
}

void ProcedurePresenter::addToProcedureList(const std::vector<Procedure>& new_mList)
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
            auto[p, nzok] = MasterNZOK::instance().getPrices(m.code, _ambList->date, CurrentUser::instance().specialty, _patient->isAdult(m.date), _ambList->full_coverage);
            nzokPrice =nzokPrice+nzok;
        }
            
    }

    view->setProcedures(rows, patientPrice, nzokPrice);

   
}

void ProcedurePresenter::setData(AmbList& amb_list, Patient& patient, std::vector<Tooth*>& selectedTeeth)
{
	_ambList = &amb_list;
	_selectedTeeth = &selectedTeeth;
    _patient = &patient;
    
    view->setUnfav(_ambList->full_coverage);


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

    ProcedureDialogPresenter p
    {
        *_selectedTeeth,
        _ambList->teeth,
        _ambList->date,
        _patient->turns18At(),
        _ambList->full_coverage,
        CurrentUser::instance().specialty
    };

    auto newList = p.openDialog();

    if (newList.empty()) return;

    if (newList[0].nzok) //re-getting the prices because of edge-case (18th birthday in current month)
    {
        for (auto& m : newList)
             m.price = MasterNZOK::instance().
                getPatientPrice
                (
                    m.code, _ambList->date, 
                    CurrentUser::instance().specialty, 
                    _patient->isAdult(m.date), 
                    _ambList->full_coverage
                );
    }


        this->addToProcedureList(newList);

        refreshProcedureView();
        makeEdited();

}

void ProcedurePresenter::editProcedure()
{
    if (_index == -1) return;

    auto& m_for_edit = _ambList->manipulations.at(_index);

    ProcedureEditorPresenter p(m_for_edit, _ambList->date, _patient->turns18At());

    auto m = std::move(p.openDialog());

    if (m.nzok)
    {
        m.price = MasterNZOK::instance().
            getPatientPrice
            (
                m.code,
                _ambList->date,
                CurrentUser::instance().specialty,
                _patient->isAdult(m.date),
                _ambList->full_coverage
            );

    }

    if (m.date == m_for_edit.date)
    {
        m_for_edit = m;
    }
    else
    {
        deleteProcedure(_index);
        addToProcedureList(std::vector<Procedure>{m});
    }




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
    _index = index;
}

void ProcedurePresenter::setUnfavourable(bool unfav)
{
    _ambList->full_coverage = unfav;

    for (auto& m : _ambList->manipulations)
    {
        if (m.nzok)
        {
            m.price = MasterNZOK::instance().
                getPatientPrice
                (
                    m.code,
                    _ambList->date,
                    CurrentUser::instance().specialty,
                    _patient->isAdult(m.date),
                    unfav
                );
                
        }
    }

    if (unfav)
    {
        view->taxCombo()->setIndex(2);
        _ambList->charge = Charge::freed;
    }


    
    makeEdited();

    refreshProcedureView();
}
