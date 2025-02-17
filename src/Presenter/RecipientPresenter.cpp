#include "RecipientPresenter.h"
#include "View/Widgets/RecipientEditView.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"
#include "Database/DbInvoice.h"

RecipientPresenter::RecipientPresenter()
{
}

RecipientPresenter::RecipientPresenter(const Recipient& recipient) :
	result{ recipient }
{
}

void RecipientPresenter::idValidInput()
{
	auto id = view->lineEdit(RecipientEditView::ID)->getText();

	//searching by bulstat in financial:

	auto r = DbInvoice::getRecipient(id);

	if (r.has_value()) {
		view->setRecipient(*r);
		return;
	}

	if (id.size() != 10) return; //not a patient id

	//searching by personal id in patient:

	//check for patient Type 1
	auto p = DbPatient::get(id, 1);

	if (p.rowid) {

		view->setRecipient(Recipient(p));
		return;
	}

	//check for patient Type 2
	p = DbPatient::get(id, 2);

	if (p.rowid) {
		view->setRecipient(Recipient(p));
		return;
	}


	
}

void RecipientPresenter::setRecipientEditView(RecipientEditView* view)
{
	this->view = view;
	
	view->lineEdit(RecipientEditView::ID)->setInputValidator(&id_validator);
	view->lineEdit(RecipientEditView::Name)->setInputValidator(&notEmpty_validator);

	if (!result) return;

	view->setRecipient(*result);

	result.reset();
}

void RecipientPresenter::okPressed()
{
	LineEdit* validatableElements[1] = {
		view->lineEdit(RecipientEditView::Name)
	};

	for (auto e : validatableElements)
	{	
		e->validateInput();
		if (!e->isValid())
		{
            e->set_focus();
			return;
		}
	}

	result = view->getRecipient();

	if (Recipient::isNhifBulstat(result->identifier)) {
		ModalDialogBuilder::showMessage("За да добавите фактура по НЗОК, заредете месечно известие от \"Месечни отчети и известия\"");
		result.reset();
		return;
	}

	view->close();
}

std::optional<Recipient> RecipientPresenter::openDialog()
{
	RecipientEditView view(*this);
	view.exec();

	return result;
}
