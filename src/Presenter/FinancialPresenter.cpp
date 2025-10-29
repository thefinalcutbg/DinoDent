#include "FinancialPresenter.h"

#include "Model/xml.h"
#include "Database/DbInvoice.h"

#include "Model/User.h"
#include "Presenter/RecipientPresenter.h"
#include <TinyXML/tinyxml.h>
#include "GlobalSettings.h"
#include "Printer/FilePaths.h"
#include "Printer/Print.h"
#include "View/Widgets/FinancialView.h"
#include "View/Widgets/TabView.h"

Invoice getInvoiceFromMonthNotif(const std::string& xmlstring, const std::string& claimedHash)
{
    TiXmlDocument doc;

    doc.Parse(xmlstring.data(), 0, TIXML_ENCODING_UTF8);

    Invoice i(doc, claimedHash, User::practice(), User::doctor());
    i.date = Date::currentDate();
    i.number = DbInvoice::getNewInvoiceNumber();
    auto existingRowid = DbInvoice::invoiceAlreadyExists(claimedHash);

    if (!existingRowid) {
        return i;
    }

    bool loadFromDb = ModalDialogBuilder::askDialog
    ("Към това месечно известие съществува такава фактура. \n"
        "Желаете ли да бъде заредена? \nВ противен случай ще бъде създаден нов документ.");

    return loadFromDb ? DbInvoice::getInvoice(existingRowid) : i;    
}

FinancialPresenter::FinancialPresenter(TabView* tabView, const std::string& monthNotif, const std::string& claimedHash) :
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(getInvoiceFromMonthNotif(monthNotif, claimedHash))
{
    this->view = tabView->financialView();

}

FinancialPresenter::FinancialPresenter(TabView* tabView, std::shared_ptr<Patient> patient, const std::vector<Procedure>& procedures) :
    TabInstance(tabView, TabType::Financial, patient),
    view(tabView->financialView()),
    m_invoice(*patient.get())
{
    m_invoice.date = Date::currentDate();

    for (auto& p : procedures) {

        m_invoice.taxEventDate = std::max(m_invoice.taxEventDate, p.date);

        BusinessOperation newOp(p);
        
        bool toBeInserted{ true };

        for (auto& existingOp : m_invoice.businessOperations) {

            if (existingOp == newOp) //&& existingOp.unit_price == p.value)
            {
                existingOp.addQuantity(1);
                toBeInserted = false;
                break;
            }
                
        }

        if(toBeInserted) m_invoice.businessOperations.push_back(newOp);
    }

    if (!procedures.size()) m_invoice.taxEventDate = Date::currentDate();

    m_invoice.number = DbInvoice::getNewInvoiceNumber();
}

FinancialPresenter::FinancialPresenter(TabView* tabView, long long rowId) :
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(DbInvoice::getInvoice(rowId))
{
}

FinancialPresenter::FinancialPresenter(TabView* tabView, const Recipient& r) :
    TabInstance(tabView, TabType::Financial, nullptr),
    view(tabView->financialView()),
    m_invoice(r)
{
    m_invoice.date = Date::currentDate();
    m_invoice.number = DbInvoice::getNewInvoiceNumber();

}

void FinancialPresenter::editOperation(int idx)
{

    if (m_invoice.nhifData.has_value()) return;

    if (idx < 0 || idx >= m_invoice.businessOperations.size()) return;;

    auto op = ModalDialogBuilder::editBusinessOperation(m_invoice.businessOperations[idx]);

    if (!op.has_value()) return;

    m_invoice.editOperation(op.value(), idx);
    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.amount(), m_invoice.isVAT);
    makeEdited();
}

void FinancialPresenter::addOperation()
{
    if (m_invoice.nhifData.has_value()) return;

    auto newOp = ModalDialogBuilder::addBusinessOperation();

    if (newOp.has_value())
        m_invoice.addOperation(newOp.value());

    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.amount(), m_invoice.isVAT);

    makeEdited();

}

void FinancialPresenter::removeOperation(int idx)
{
    if (m_invoice.nhifData.has_value()) return;

    if (idx < 0 || idx >= m_invoice.businessOperations.size()) return;

    m_invoice.removeOperation(idx);
    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.amount(), m_invoice.isVAT);
    makeEdited();
}

void FinancialPresenter::dateChanged(Date date)
{
    m_invoice.date = date;
    makeEdited();
}

void FinancialPresenter::taxEventDateChanged(Date date)
{
    m_invoice.taxEventDate = date;
    makeEdited();
}

void FinancialPresenter::paymentTypeChanged(PaymentType type)
{
    m_invoice.paymentType = type;
    makeEdited();
}


void FinancialPresenter::saveAsXML()
{
    if (!save()) return;

    if (!User::practice().nhif_contract) {
        ModalDialogBuilder::showError("Попълнете данните на НЗОК договора от насторйки");
        return;
    }

    ModalDialogBuilder::saveFile(XML::getInvoice(m_invoice), m_invoice.getFileName());
 
}

void FinancialPresenter::sendToPis()
{
    if (!save()) return;

    if (!User::practice().nhif_contract) {
        ModalDialogBuilder::showError("Попълнете данните на НЗОК договора от насторйки");
        return;
    }

    if (
        !ModalDialogBuilder::askDialog("Желаете ли да изпратите финансовият документ към ПИС?")
    ) return;

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

        auto mainDocDb = DbInvoice::getMainDocument(m_invoice.recipient.identifier, m_invoice.rowId);

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

        auto db_date = DbInvoice::getMainDocDate(num, m_invoice.recipient.identifier);

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

        ModalDialogBuilder::showMessage("Тези данни са генерирани на базата на договора с НЗОК и не могат да бъдат редактирани.");
        return;
    }

    RecipientPresenter p(m_invoice.recipient);

    auto result = p.openDialog();

    if (!result) return;

    m_invoice.recipient = result.value();

    if (m_invoice.nhifData) {
        m_invoice.recipient = Recipient(User::practice());
    }

    view->setInvoice(m_invoice);

    makeEdited();
}

void FinancialPresenter::editIssuer()
{
    ModalDialogBuilder::openSettingsDialog(SettingsDialog::Tab::Company);
    view->setInvoice(m_invoice);
}

void FinancialPresenter::invoiceNumberChanged(long long number)
{
    m_invoice.number = number;
    edited = false;
    makeEdited();
}

void FinancialPresenter::vatChanged(bool isVat)
{
    m_invoice.isVAT = isVat;

    view->setBusinessOperations(m_invoice.businessOperations, m_invoice.amount(), m_invoice.isVAT);

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

void FinancialPresenter::pdfPrint()
{
    if (!save()) return;

    auto filepath = FilePaths::get(m_invoice);
    
    if (filepath.empty()) return;

    if(!Print::invoice(m_invoice, filepath)) return;

    if (ModalDialogBuilder::askDialog(
        "Файлът е запазен успешно. Желаете ли да отворите директорията?"
    )) {
        ModalDialogBuilder::openExplorer(filepath);
    }

}

void FinancialPresenter::setDataToView()
{
    
    view->setPresenter(this);

    if (!m_invoice.nhifData && patient != nullptr) {
        m_invoice.recipient = Recipient{ *patient.get() }; //refreshing the patient incase it's changed
    }

    if (m_invoice.nhifData) {
        m_invoice.recipient = Recipient(User::practice());
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

        TabName tab;
        tab.header = newName[nameIdx];

        tab.header_icon = m_invoice.nhifData.has_value() ?
            CommonIcon::NHIF
            :
            CommonIcon::INVOICE;


        tab.footer = m_invoice.recipient.name;

        return tab;
    }

    static const std::string docTypeName[3]{ "Фактура", "Дебитно известие", "Кредитно известие" };

    TabName tab;
    tab.header = docTypeName[nameIdx] + " №" + m_invoice.getInvoiceNumber()
        ;
    tab.header_icon = CommonIcon::INVOICE;

    tab.footer = m_invoice.recipient.name;

    if (m_invoice.nhifData.has_value()) {
        tab.footer_icon = CommonIcon::NHIF;
    }

    return tab;
   
}
