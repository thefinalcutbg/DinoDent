#include "FinancialPresenter.h"
#include "Model/User/UserManager.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "View/Printer/Printer.h"
#include <FileSystem>
#include <stdexcept>
#include "Model/XML/xml.h"

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
    m_invoice(getDocument(monthNotifFilepath), UserManager::currentUser())
{
    this->view = tabView->financialView();

    //the name of the output file
    std::filesystem::path p(monthNotifFilepath);
    std::string fileName = p.filename().string();

    if (std::string_view(fileName.c_str(), 10) == "MON_NOTIF_") {
        fileName.erase(0, 10);
    }

    m_invoice.nzokData->outputFileName = "Invoice_" + fileName;

    m_invoice.date = Date::currentDate();

    //checking the db for monthNotif number, and getting it from there actually

}

void FinancialPresenter::dateChanged(Date date)
{
    m_invoice.date = date;
    makeEdited();
}

void FinancialPresenter::saveAsXML(const std::string& filePath)
{
    XML::saveXMLinvoice(m_invoice, filePath);
}

const std::string& FinancialPresenter::rowID() const
{
	return m_invoice.rowId;
}

bool FinancialPresenter::save()
{
    if (!isNew() && !edited) return true;

    if (isNew()) return saveAs();

    //save the invoice

	return true;
}

bool FinancialPresenter::saveAs()
{
    std::unordered_set<int> existingNumbers; //get it from db;

    int newNumber = 1; //we have to get it from db actually

    newNumber = ModalDialogBuilder::saveAsAmbSheetNumber(newNumber, existingNumbers);

    if (!newNumber) return false; //it means the dialog has been cancled

    m_invoice.number = newNumber;

    m_invoice.rowId = ""; // <- put the invoice into the db and get its rowid;

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
