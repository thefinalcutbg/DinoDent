#include "ICD10Dialog.h"

constexpr auto code_column = 0;
constexpr auto search_column = 1;

ICD10Dialog::ICD10Dialog(const ICD10& icd, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Международна класификация на болестите 10");
	setWindowFlag(Qt::WindowMaximizeButtonHint);

	ui.tableView->setModel(&m_proxyModel);
	m_proxyModel.setFilterKeyColumn(search_column);

	connect(ui.searchEdit, &QLineEdit::textChanged, this, [=, this](const QString& text)
		{
			m_proxyModel.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
		});

	connect(ui.cancelButton, &QPushButton::clicked, this, [=, this] { reject(); });
	connect(ui.okButton, &QPushButton::clicked, this, [=, this] {

			m_selectedRow = ui.tableView->selectionModel()->currentIndex().row();
			
			if (m_selectedRow > -1) {
				accept();
			}

		});
	connect(ui.tableView, &QTableView::doubleClicked, this, [=, this] { ui.okButton->click(); });

	connect(ui.checkBox, &QCheckBox::clicked, this, [&](bool clicked) {
		m_proxyModel.setSourceModel(clicked ? &s_full_model : &s_dental_model);
	});

	if (!icd.isValid()) {
		m_proxyModel.setSourceModel(&s_dental_model);
		initTable();
		return;
	}
	
	bool isDental = icd.isDental();

	QSignalBlocker b(ui.checkBox);

	m_proxyModel.setSourceModel(
		isDental ?
		&s_dental_model
		:
		&s_full_model
	);

	initTable();

	ui.checkBox->setChecked(!isDental);

	for (int i = 0; i < m_proxyModel.rowCount(); i++)
	{
		if (m_proxyModel.index(i, code_column).data().toString().toStdString() != icd.code()) continue;

		ui.tableView->selectRow(i);

		break;
	}
}

void ICD10Dialog::initTable()
{

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView->horizontalHeader()->setHighlightSections(false);
	ui.tableView->verticalHeader()->setVisible(false);
	//ui.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setWordWrap(true);
	ui.tableView->setColumnWidth(0, 70);
	ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

}


ICD10 ICD10Dialog::getResult()
{
	if (result() == QDialog::Rejected) return {};

	auto result = ICD10{ ui.tableView->model()->index(m_selectedRow, code_column)
		.data()
		.toString()
		.toStdString()
	};

	return result;
}

ICD10Dialog::~ICD10Dialog()
{}
