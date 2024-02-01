#include "FiscalReceiptPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbFiscalReceipt.h"
#include "Model/FreeFunctions.h"

void FiscalReceiptPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(*this);
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

	view->closeDialog();
}

void FiscalReceiptPresenter::setView(IFiscalReceiptDialog* v)
{
	this->view = v;

    view->setReceipt(FreeFn::getTimeStampLocal(), DbFiscalReceipt::getFiscalMemory());
}
