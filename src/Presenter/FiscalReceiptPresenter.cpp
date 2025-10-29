#include "FiscalReceiptPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbFiscalReceipt.h"
#include "Model/FreeFunctions.h"
#include "View/Widgets/FiscalReceiptDialog.h"

void FiscalReceiptPresenter::openDialog()
{
	FiscalReceiptDialog d(*this);
	d.exec();
}

void FiscalReceiptPresenter::okPressed()
{
	auto receipt = view->getReceipt();
	receipt.amblistRowid = amblist_rowid;

	if (DbFiscalReceipt::alreadyExists(amblist_rowid, receipt.timestamp))
	{
		ModalDialogBuilder::showMessage(
			"За този пациент вече съществува опис на касов бон за съответния ден"
		);

		return;
	}

	DbFiscalReceipt::saveReceipt(receipt);

	view->close();
}

void FiscalReceiptPresenter::setView(FiscalReceiptDialog* v)
{
	this->view = v;

    view->setReceipt(FreeFn::getTimeStampLocal(), DbFiscalReceipt::getFiscalMemory());
}
