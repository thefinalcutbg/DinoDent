#include "ProcedurePrintSelectDialog.h"
#include <set>
#include "Model/FreeFunctions.h"
#include <QPainter>

void ProcedurePrintSelectDialog::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);

	painter.fillRect(rect(), Qt::white);
}

ProcedurePrintSelectDialog::ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, const std::vector<Referral>& referrals, QWidget* parent)
	: QDialog(parent), model(procedures)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle("Избор на манипулации");

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	ui.tableView->setModel(&this->model);
	//ui.tableView->setProcedurePrintSelectLayout();

	ui.tableView->setFocusPolicy(Qt::NoFocus);
	ui.tableView->setColumnWidth(0, 110);
	ui.tableView->setColumnWidth(1, 200);
	ui.tableView->setColumnWidth(2, 25);
	ui.tableView->setColumnWidth(3, 300);
	ui.tableView->setColumnWidth(4, 49);
	ui.tableView->setColumnWidth(5, 80);
	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	ui.tableView->setShowGrid(false);
	ui.tableView->verticalHeader()->hide();

	ui.mdd4group->hide();
	ui.mh119check->hide();

	if (procedures.empty()) {
		ui.tableView->hide();
		ui.groupBox->hide();
		adjustSize();
	}

	connect(ui.okButton, &QPushButton::pressed, [=]
		{
			auto select = ui.tableView->selectionModel()->selectedIndexes();

			m_selectedRows = model.selectedRows();

			accept();

		});

	connect(ui.paidButton, &QPushButton::clicked, [&] { selectOnlyWhereNzokIs(false); });
	connect(ui.nzokButton, &QPushButton::clicked, [=] { selectOnlyWhereNzokIs(true); });

	if (referrals.empty()) return;
	

	for (int i = 0; i < referrals.size(); i++)
	{
		auto ref = referrals[i];

		ref_typeIndexes.push_back(ref.type);

		if (ref.type == ReferralType::MDD4)
		{
			ui.mdd4group->show();

			QString buttonName = "Направление МДД №";
			buttonName += FreeFn::leadZeroes(ref.number, 6).c_str();

			mdd4_buttons.push_back(new QRadioButton(buttonName, this));
			ui.mdd4Layout->addWidget(mdd4_buttons.back());
		}

		if (ref.type == ReferralType::MH119)
		{
			ui.mh119check->show();
			ui.mh119check->setChecked(true);
			mh119index = i;
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

int ProcedurePrintSelectDialog::mh119Referral() const
{
	return ui.mh119check->isChecked() ? mh119index : -1;
}

void ProcedurePrintSelectDialog::selectOnlyWhereNzokIs(bool nhif)
{
	model.selectOnlyRowsWhereNzokIs(nhif);

	ui.mdd4group->setChecked(nhif);
	ui.mh119check->setChecked(nhif);
}


ProcedurePrintSelectDialog::~ProcedurePrintSelectDialog()
{
}
