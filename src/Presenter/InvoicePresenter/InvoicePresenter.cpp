#include "InvoicePresenter.h"

#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/UserManager.h"
#include "View/ModalDialogBuilder.h"
#include "View/InvoiceDialog/IInvoiceDialog.h"
#include "Model/XML/xml.h"
#include <QDebug>


void InvoicePresenter::loadFile(const std::string& filePath)
{
    TiXmlDocument doc;
    
    doc.LoadFile(filePath, TiXmlEncoding::TIXML_ENCODING_UTF8);

    try
    {
      m_invoice.emplace(doc, UserManager::currentUser());
    }
    catch(...)
    {
        ModalDialogBuilder::showError(u8"Неуспешно зареждане на месечното известие");
        return;
    }
    m_invoice.value().fin_document_no = "0000006666";
    m_invoice.value().fin_document_date = Date::currentDate();

    view->enableUserFields();

}

void InvoicePresenter::saveAsXML(const std::string& filePath)
{
    if (!invoiceIsValid())
        return;

    XML::saveXMLinvoice(m_invoice.value(), filePath);
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

    if (m_invoice.value().fin_document_no.empty())
    {
        ModalDialogBuilder::showError(u8"Въведете номер на фактурата!");
        return false;
    }

    return true;

}