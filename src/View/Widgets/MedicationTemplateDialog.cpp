#include "MedicationTemplateDialog.h"
#include "Database/DbPrescription.h"
#include "View/ModalDialogBuilder.h"
#include <QKeyEvent>

MedicationTemplateDialog::MedicationTemplateDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Шаблони за медикаменти");

	ui.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

	ui.tableView->setModel(&m_model);

	ui.tableView->installEventFilter(this);

	m_templates = DbPrescription::getTemplates();

	std::vector<std::string> m_rows;

	for (auto& [rowid, med] : m_templates) {

		m_rows.push_back(med.name() + "; " + med.parseFullDosage());
	}

	m_model.setRows(m_rows);

	connect(ui.okButton, &QPushButton::clicked, this, [&] { 
		
		auto index = ui.tableView->selectionModel()->currentIndex().row();

		if (index < 0 || index >= m_templates.size()) return;

		accept(); 
	});

	connect(ui.tableView, &QTableView::doubleClicked, this, [&] { ui.okButton->click(); });

	connect(ui.deleteButton, &QPushButton::clicked, this, [&] { removeTemplate(); });

	ui.okButton->setDisabled(m_templates.empty());
	ui.deleteButton->setDisabled(m_templates.empty());
}

std::optional<Medication> MedicationTemplateDialog::getResult()
{
	if (result() != QDialog::Accepted) {
		return {};
	};

	auto index = ui.tableView->selectionModel()->currentIndex().row();

	if (index < 0 || index >= m_templates.size()) return {};

	return m_templates[index].second;
}

bool MedicationTemplateDialog::eventFilter(QObject* object, QEvent* e)
{
	if (object == ui.tableView
		&& e->type() == QEvent::Type::KeyPress
		&& static_cast<QKeyEvent*>(e)->key() == Qt::Key_Delete
	) 
	{
		ui.deleteButton->click();
		return true;
	}

	return false;
}

void MedicationTemplateDialog::removeTemplate()
{
	auto index = ui.tableView->selectionModel()->currentIndex().row();

	if (index < 0 || index >= m_templates.size()) return;

	bool answer = ModalDialogBuilder::askDialog("Сигурни ли сте, че искате да премахнете избрания медикамент от шаблони?");

	if (!answer) return;

	bool success = DbPrescription::deleteTemplate(m_templates[index].first);

	if (success) {
		m_templates.erase(m_templates.begin() + index);
	}

	std::vector<std::string> m_rows;

	for (auto& [rowid, med] : m_templates) {

		m_rows.push_back(med.name() + "; " + med.parseFullDosage());
	}

	m_model.setRows(m_rows);

	ui.tableView->selectRow(index < m_rows.size() ? index : index-1);
	
}

MedicationTemplateDialog::~MedicationTemplateDialog()
{}
