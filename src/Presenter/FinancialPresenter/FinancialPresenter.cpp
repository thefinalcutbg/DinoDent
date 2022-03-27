#include "FinancialPresenter.h"
#include "Model/User/UserManager.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "View/Printer/Printer.h"
#include <FileSystem>
#include <stdexcept>
#include "Model/XML/xml.h"
#include "Database/DbInvoice.h"


TiXmlDocument getDocument(const std::string& filePath)
{
    TiXmlDocument doc;

    bool loaded = doc.LoadFile(filePath, TiXmlEncoding::TIXML_ENCODING_UTF8);
   
    if (!loaded) {
        std::string err = u8"Не е намерен файл: " + filePath;
        throw std::exception(err.c_str());
    }

    return doc;
}

FinancialPresenter::FinancialPresenter(ITabView* tabView, const std::string& monthNotifFilepath) : 
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(getDocument(monthNotifFilepath), UserManager::currentUser())
{
    this->view = tabView->financialView();

    //if the month notif is already loaded in the db:

    auto existingData = DbInvoice::getDetailsIfAlreadyExist(m_invoice.nzokData->fin_document_month_no);

    if (existingData.has_value())
    {
        m_invoice.rowId = existingData->rowId;
        m_invoice.number = existingData->num;
        m_invoice.date = existingData->date;
        return;
    }

    m_invoice.date = Date::currentDate();


}

FinancialPresenter::FinancialPresenter(ITabView* tabView, const Procedures& procedures, std::shared_ptr<Patient> patient) :
    TabInstance(tabView, TabType::Financial, patient),
    view(tabView->financialView()),
    m_invoice(*patient.get(), UserManager::currentUser())
{
    m_invoice.date = Date::currentDate();

    for (auto& p : procedures) {

        m_invoice.aggragated_amounts.taxEventDate = std::max(m_invoice.aggragated_amounts.taxEventDate, p.date);

        BusinessOperation newOp(p);
        
        bool toBeInserted{ true };

        for (auto& existingOp : m_invoice.businessOperations) {

            if (existingOp == newOp) {
                existingOp.addQuantity(1);
                toBeInserted = false;
                break;
            }
                
        }

        if(toBeInserted) m_invoice.businessOperations.push_back(newOp);
    }

    m_invoice.aggragated_amounts.calculate(m_invoice.businessOperations);

    if (!procedures.size()) m_invoice.aggragated_amounts.taxEventDate = Date::currentDate();
}

FinancialPresenter::FinancialPresenter(ITabView* tabView, int rowId) :
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(DbInvoice::getInvoice(std::to_string(rowId)))
{
}

void FinancialPresenter::editOperation(int idx)
{

    if (m_invoice.nzokData.has_value()) return;

    if (idx < 0 || idx >= m_invoice.businessOperations.size()) return;;

    auto op = ModalDialogBuilder::editBusinessOperation(m_invoice.businessOperations[idx]);

    if (!op.has_value()) return;

    m_invoice.editOperation(op.value(), idx);
    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.aggragated_amounts);
    makeEdited();
}

void FinancialPresenter::addOperation()
{
    if (m_invoice.nzokData.has_value()) return;

    auto newOp = ModalDialogBuilder::addBusinessOperation(UserManager::currentUser().practice.priceList);

    if (newOp.has_value())
        m_invoice.addOperation(newOp.value());

    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.aggragated_amounts);

    makeEdited();

}

void FinancialPresenter::removeOperation(int idx)
{
    if (m_invoice.nzokData.has_value()) return;

    if (idx < 0 || idx >= m_invoice.businessOperations.size()) return;

    m_invoice.removeOperation(idx);
    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.aggragated_amounts);
}

void FinancialPresenter::dateChanged(Date date)
{
    m_invoice.date = date;
    makeEdited();
}

void FinancialPresenter::taxEventDateChanged(Date date)
{
    m_invoice.aggragated_amounts.taxEventDate = date;
    makeEdited();
}

void FinancialPresenter::paymentTypeChanged(PaymentType type)
{
    m_invoice.aggragated_amounts.paymentType = type;
    makeEdited();
}



void FinancialPresenter::saveAsXML()
{
    if (!save()) return;

    auto filepath = ModalDialogBuilder::getFileNamePath(m_invoice.getFileName());

    if (filepath.has_value()) {
        XML::saveXMLinvoice(m_invoice, filepath.value());
    }
}

const std::string& FinancialPresenter::rowID() const
{
	return m_invoice.rowId;
}

bool FinancialPresenter::save()
{
    if (!m_invoice.businessOperations.size()) {
        ModalDialogBuilder::showError(
            u8"Финансовият документ трябва да съдържа поне една услуга"
        );
        return false;
    }

    if (!isNew() && !edited) return true;

    if (isNew()) return saveAs();

    DbInvoice::updateInvoice(m_invoice);

    edited = false;

    _tabView->changeTabName(getTabName());

	return true;
}

bool FinancialPresenter::saveAs()
{

    std::unordered_set<int> existingNumbers = DbInvoice::getExistingNumbers();

    int newNumber = 0;

    if (isNew()) {
        newNumber = DbInvoice::getNewInvoiceNumber();
    }
    else {
        newNumber = m_invoice.number;
        existingNumbers.erase(newNumber);
    }

    newNumber = ModalDialogBuilder::saveAsDocNumber(newNumber, existingNumbers, u8"Финансов документ");

    if (!newNumber) return false; //it means the dialog has been canceled

    m_invoice.number = newNumber;

    if (isNew()) {
        m_invoice.rowId = DbInvoice::insertInvoice(m_invoice);
    }
    else{
        DbInvoice::updateInvoice(m_invoice);
    }

    edited = false;

    _tabView->changeTabName(getTabName());

    return true;

}

void FinancialPresenter::print()
{
    if (save())
    Print::invoice(m_invoice);
}

void FinancialPresenter::setCurrent()
{
    
    view->setPresenter(this);

    if (!m_invoice.nzokData && patient != nullptr) {
        m_invoice.recipient = Recipient{ *patient.get() }; //refreshing the patient incase it's changed
    }


    view->setInvoice(m_invoice);

    _tabView->showFinancialView();
    
}

bool FinancialPresenter::isNew()
{
    return m_invoice.rowId.empty();
}

TabName FinancialPresenter::getTabName()
{
    if(!m_invoice.number) return TabName{ u8"Нов финансов документ", ""};

    static constexpr const char* names[3]{ u8"Фактура", u8"Дебитно известие", u8"Кредитно известие" };

    return TabName{  
        names[static_cast<int>(m_invoice.type)],
        "№" + m_invoice.getInvoiceNumber()
    };
   
}
