#include "ProcedureDialog.h"
#include "Presenter/ProcedureDialogPresenter.h"

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter& presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter), proxyModel(this)
{
	ui.setupUi(this);

	setModal(true);
	
	setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
								   Qt::WindowMaximizeButtonHint |
								   Qt::WindowCloseButtonHint);

	setWindowTitle("Добавяне на процедура");


    connect(ui.procedureInput->qDateEdit(), &QDateEdit::dateChanged, this, [&] {

		auto date = ui.procedureInput->qDateEdit()->date();
        presenter.procedureDateChanged(Date{ date.day(), date.month(), date.year() });
        
	});

	connect(ui.procedureListView, &ProcedureListView::codeDoubleClicked, this, [&] { presenter.formAccepted(); });
	connect(ui.procedureListView, &ProcedureListView::codeSelected, this, [&](const std::string& code, bool nhif, double price){ presenter.setCode(code, nhif, price); });

	connect(ui.cancelButton, &QPushButton::clicked, this, [&] { close(); });
	connect(ui.okButton, &QPushButton::clicked, this, [&] { presenter.formAccepted(); });

    presenter.setView(this);
}

ProcedureDialog::~ProcedureDialog()
{
}

void ProcedureDialog::setSelectionLabel(const std::vector<int>& selectedTeethNum)
{
	QString selectedTeeth;

	selectedTeeth.reserve(15 + selectedTeethNum.size() * 3);

	selectedTeeth.append("Избрани зъби: ");

    for (size_t i = 0; i < selectedTeethNum.size(); i++)
	{
		selectedTeeth.append(QString::number(selectedTeethNum[i]));
		if (i < selectedTeethNum.size() - 1)
		{
			selectedTeeth.append(", ");
		}
	}

	if (!selectedTeethNum.size())
	{
		selectedTeeth.append("Няма");
	}

	ui.statusLabel->setText(selectedTeeth);
}

IProcedureInput* ProcedureDialog::procedureInput()
{
	return ui.procedureInput;
}

ProcedureListView* ProcedureDialog::procedureList()
{
	return ui.procedureListView;
}




