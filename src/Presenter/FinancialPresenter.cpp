#include "FinancialPresenter.h"

#include "Model/xml.h"
#include "Database/DbInvoice.h"
#include "View/Printer.h"
#include "Model/User.h"

#include <TinyXML/tinyxml.h>
#include <FileSystem>
#include <stdexcept>
#include <fstream>

Invoice getInvoiceFromMonthNotif(const std::string& xmlstring)
{
    TiXmlDocument doc;

    const char* load = doc.Parse(xmlstring.data(), 0, TIXML_ENCODING_UTF8);

    Invoice i(doc, User::practice(), User::doctor());
    i.date = Date::currentDate();
    i.number = DbInvoice::getNewInvoiceNumber();
    auto existingRowid = DbInvoice::invoiceAlreadyExists(i.nhifData->fin_document_month_no);

    if (!existingRowid) {
        return i;
    }

    bool loadFromDb = ModalDialogBuilder::askDialog
    ("Към това месечно известие съществува такава фактура. \n"
        "Желаете ли да бъде заредена? \nВ противен случай ще бъде създаден нов документ.");

    return loadFromDb ? DbInvoice::getInvoice(existingRowid) : i;    
}

FinancialPresenter::FinancialPresenter(ITabView* tabView, const std::string& monthNotif) :
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(getInvoiceFromMonthNotif(monthNotif))
{
    this->view = tabView->financialView();

}

FinancialPresenter::FinancialPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, const std::vector<Procedure>& procedures) :
    TabInstance(tabView, TabType::Financial, patient),
    view(tabView->financialView()),
    m_invoice(*patient.get(), User::practice(), User::doctor())
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

    m_invoice.number = DbInvoice::getNewInvoiceNumber();
}

FinancialPresenter::FinancialPresenter(ITabView* tabView, long long rowId) :
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(DbInvoice::getInvoice(rowId))
{
}

void FinancialPresenter::editOperation(int idx)
{

    if (m_invoice.nhifData.has_value()) return;

    if (idx < 0 || idx >= m_invoice.businessOperations.size()) return;;

    auto op = ModalDialogBuilder::editBusinessOperation(m_invoice.businessOperations[idx]);

    if (!op.has_value()) return;

    m_invoice.editOperation(op.value(), idx);
    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.aggragated_amounts);
    makeEdited();
}

void FinancialPresenter::addOperation()
{
    if (m_invoice.nhifData.has_value()) return;

    auto newOp = ModalDialogBuilder::addBusinessOperation();

    if (newOp.has_value())
        m_invoice.addOperation(newOp.value());

    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.aggragated_amounts);

    makeEdited();

}

void FinancialPresenter::removeOperation(int idx)
{
    if (m_invoice.nhifData.has_value()) return;

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

    ModalDialogBuilder::saveFile(XML::getInvoice(m_invoice), m_invoice.getFileName());
 
}

void FinancialPresenter::sendToPis()
{
    if (!save()) return;

    if (
        !ModalDialogBuilder::askDialog("Желаете ли да изпратите финансовият документ към ПИС?")
    ) return;

    if (User::practice().legal_entity != 0) {
        ModalDialogBuilder::showMessage("Уверете се, че ползвате фирмения си електронен подпис");
    }

    file_handler.sendInvoice(
        m_invoice,
        User::practice().rziCode
    );


}

void FinancialPresenter::docTypeChanged(int index)
{
    m_invoice.type = static_cast<FinancialDocType>(index);

    //forcing refresh of the tab name:
    edited = false;
    makeEdited();

    if (m_invoice.type != FinancialDocType::Invoice) {

        auto mainDocDb = DbInvoice::getMainDocument(m_invoice.recipient.bulstat, m_invoice.rowId);

        if (mainDocDb) {
            m_invoice.setMainDocumentData(mainDocDb->number, mainDocDb->date);
        }

    }

    view->setMainDocument(m_invoice.mainDocument());

   
}

void FinancialPresenter::mainDocumentChanged(long long num, Date date)
{
    //we assume, that since ui has enabled mainDocument,
    //the optional main doc has value:
    if (m_invoice.mainDocument()->number != num) {
        
        //it means that user has changed the number, so
        //we get the date from db if main document with that number already exist:

        auto db_date = DbInvoice::getMainDocDate(num, m_invoice.recipient.bulstat);

        if (db_date)
            date = db_date.value();
    }

    m_invoice.setMainDocumentData(num, date);

    view->setMainDocument(m_invoice.mainDocument());

    makeEdited();
}

void FinancialPresenter::editRecipient()
{
    if (m_invoice.nhifData) {

        ModalDialogBuilder::showMessage("Не можете да редактирате тези данни");
        return;
    }
    auto result = ModalDialogBuilder::editRecipient(m_invoice.recipient);

    if (!result) return;

    m_invoice.recipient = result.value();

    view->setInvoice(m_invoice);

    makeEdited();
}

void FinancialPresenter::invoiceNumberChanged(long long number)
{
    m_invoice.number = number;
    edited = false;
    makeEdited();
}

long long FinancialPresenter::rowID() const
{
	return m_invoice.rowId;
}

bool FinancialPresenter::save()
{
    if (m_invoice.businessOperations.empty()) {
        ModalDialogBuilder::showError(
            "Финансовият документ трябва да съдържа поне една услуга"
        );
        return false;
    }

    if(DbInvoice::invoiceAlreadyExists(m_invoice.number, m_invoice.rowId) &&
        !ModalDialogBuilder::askDialog(
            "Фактура с такъв номер вече съществуа. Сигурни ли сте, че искате да дублирате номерацията?"
        )
    )
    {
        return false;
    }

    if (!isNew() && !edited) return true;

    if (isNew())
    {
        m_invoice.rowId = DbInvoice::insertInvoice(m_invoice);
    }
    else
    {
        DbInvoice::updateInvoice(m_invoice);

    }
    edited = false;

    refreshTabName();

	return true;
}

void FinancialPresenter::print()
{
    if (save())
    Print::invoice(m_invoice);
}

void FinancialPresenter::setDataToView()
{
    
    view->setPresenter(this);

    if (!m_invoice.nhifData && patient != nullptr) {
        m_invoice.recipient = Recipient{ *patient.get() }; //refreshing the patient incase it's changed
    }


    view->setInvoice(m_invoice);
    view->setNumberSpinBox(m_invoice.number);
}

bool FinancialPresenter::isNew()
{
    return m_invoice.rowId == 0;
}

TabName FinancialPresenter::getTabName()
{
    int nameIdx = static_cast<int>(m_invoice.type);

    if (!m_invoice.rowId) {

        static const std::string newName[3]{ "Новa фактура", "Ново дебитно известие", "Ново кредитно известие" };

        return { newName[nameIdx], "", m_invoice.nhifData.has_value()};
    }

    static const std::string docTypeName[3]{ "Фактура", "Дебитно известие", "Кредитно известие" };

    return { docTypeName[nameIdx], "№" + m_invoice.getInvoiceNumber(), m_invoice.nhifData.has_value()};
   
}
