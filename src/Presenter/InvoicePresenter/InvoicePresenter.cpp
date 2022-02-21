#include "InvoicePresenter.h"

#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/UserManager.h"
#include "View/ModalDialogBuilder.h"
#include "View/InvoiceDialog/IInvoiceDialog.h"
#include "Model/XML/xml.h"
#include <filesystem>
#include <string_view>

std::string getOutputFileName(const std::string& filepath)
{
    std::filesystem::path p(filepath);
    std::string fileName = p.filename().string();

    if (std::string_view(fileName.c_str(), 10) == "MON_NOTIF_") {
        fileName.erase(0, 10);
    }
    
    return "Invoice_" + fileName;

}

std::string addLeadZeroes(int num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);

    std::string number(std::to_string(num));

    for (int i = 0; i < totalLength - number.length(); i++)
        formated += "0";

    formated += number;

    return formated;
}


void InvoicePresenter::loadFile(const std::string& filePath)
{
    TiXmlDocument doc;
    
    std::filesystem::path p(filePath);

    outputFileName = getOutputFileName(filePath);

    bool loaded = doc.LoadFile(filePath, TiXmlEncoding::TIXML_ENCODING_UTF8);

    if (!loaded){
        ModalDialogBuilder::showError(u8"не е намерен файл: " + filePath);
        return;
    }

    try{
      m_invoice.emplace(doc, UserManager::currentUser());
    }
    catch(const std::exception& e) {
        ModalDialogBuilder::showError(u8"Неуспешно зареждане на месечното известие");
        return;
    }
    m_invoice.value().fin_document_no = addLeadZeroes(0, 10);
    m_invoice.value().fin_document_date = Date::currentDate();

    view->enableUserFields();
    view->setInvoice(m_invoice.value());

}

void InvoicePresenter::saveAsXML(const std::string& filePath)
{
    XML::saveXMLinvoice(m_invoice.value(), filePath + "/" + outputFileName);
}

void InvoicePresenter::docNumberChanged(int number)
{
    m_invoice.value().fin_document_no = addLeadZeroes(number, 10);
    view->refreshNumberAndDate(m_invoice.value().fin_document_no, m_invoice.value().fin_document_date);
}

void InvoicePresenter::docDateChanged(Date date)
{
    m_invoice.value().fin_document_date = date;
    view->refreshNumberAndDate(m_invoice.value().fin_document_no, m_invoice.value().fin_document_date);
}

void InvoicePresenter::open()
{
    if (!UserManager::currentUser().practice.nzok_contract.has_value()) {
        ModalDialogBuilder::showError(u8"За тази практика не е попълнен договор с НЗОК!");
        return;
    }


    ModalDialogBuilder::openDialog(this);
}



bool InvoicePresenter::invoiceIsValid()
{
    if (!m_invoice.has_value()) {

        ModalDialogBuilder::showError(u8"Моля, заредете месечно известие в xml формат!");
        return false;
    }

    if (m_invoice.value().fin_document_no == "0000000000")
    {
        ModalDialogBuilder::showError(u8"Въведете номер на финансовият документ!");
        return false;
    }

    return true;
}