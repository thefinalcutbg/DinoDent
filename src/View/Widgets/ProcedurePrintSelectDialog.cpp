#include "ProcedurePrintSelectDialog.h"
#include <set>
#include "Model/FreeFunctions.h"
ProcedurePrintSelectDialog::ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, const std::vector<Referral>& referrals, QWidget* parent)
	: QDialog(parent), model(procedures)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle("Избор на манипулации");

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	ui.tableView->setModel(&this->model);
	ui.tableView->setProcedurePrintSelectLayout();

	connect(ui.okButton, &QPushButton::pressed, [=]
		{
			auto select = ui.tableView->selectionModel()->selectedIndexes();

			m_selectedRows = model.selectedRows();

			accept();

		});

	connect(ui.paidButton, &QPushButton::clicked, [=] { this->model.selectOnlyRowsWhereNzokIs(false); });
	connect(ui.nzokButton, &QPushButton::clicked, [=] { this->model.selectOnlyRowsWhereNzokIs(true); });

	if (referrals.empty())
	{
		ui.mdd4group->hide();
		return;
	}

	for (auto& ref : referrals)
	{
		ref_typeIndexes.push_back(ref.type);

		if (ref.type == ReferralType::MDD4)
		{
			QString buttonName = "Направление МДД №";
			buttonName += FreeFn::leadZeroes(ref.number, 6).c_str();

			mdd4_buttons.push_back(new QRadioButton(buttonName, this));
			ui.mdd4Layout->addWidget(mdd4_buttons.back());
		}
	}

	if (mdd4_buttons.size()) {

		ui.mdd4group->show();
		ui.mdd4group->setChecked(true);

		mdd4_buttons[0]->setChecked(true);

		connect(ui.mdd4group, &QGroupBox::toggled, [&](bool toggled) {

				for (auto& button : mdd4_buttons) {
					button->setDisabled(!toggled);
				}
			}
		);

	}
	

}

const std::vector<int> ProcedurePrintSelectDialog::selectedProcedures() const
{
	return m_selectedRows;
}

int ProcedurePrintSelectDialog::mdd4Referral() const
{

	int mdd4Index = -1;

	if (!ui.mdd4group->isChecked()) return mdd4Index;

	//conversion between Mdd4 index and referral index:

	for (auto& type : ref_typeIndexes)
	{
		if (type != ReferralType::MDD4) continue;

		mdd4Index++;

		if (mdd4_buttons[mdd4Index]->isChecked()) break;
	}
	
	return mdd4Index;


}

void ProcedurePrintSelectDialog::selectOnlyWhereNzokIs(bool nhif)
{
	model.selectOnlyRowsWhereNzokIs(nhif);
}


ProcedurePrintSelectDialog::~ProcedurePrintSelectDialog()
{
}
