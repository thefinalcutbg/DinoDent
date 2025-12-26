#include "NotesTemplateDialog.h"
#include "View/ModalDialogBuilder.h"

NotesTemplateDialog::NotesTemplateDialog(DbNotes::TemplateType type, QWidget *parent)
    : QDialog(parent),
    m_type(type)
{
	ui.setupUi(this);

	setWindowTitle("Шаблони за допълнителни бележки");

    auto notes = DbNotes::getTemplateNotes(m_type);

	ui.tableWidget->setColumnCount(1);
	ui.tableWidget->setRowCount(notes.size());

	ui.tableWidget->verticalHeader()->hide();
	ui.tableWidget->horizontalHeader()->hide();
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	for (int i = 0; i < notes.size(); i++) {

        auto item = new QTableWidgetItem(notes[i].c_str());
        ui.tableWidget->setItem(i, 0, item);

	}

    if(notes.size()){
        ui.tableWidget->selectRow(0);
    }

	connect(ui.addButton, &QPushButton::clicked, this, [&] {
		
		auto result = ModalDialogBuilder::inputDialog("", "Добави шаблон");
		
		if (result.empty()) return; 
			 
		auto rows = ui.tableWidget->rowCount() + 1;

		ui.tableWidget->setRowCount(rows);
		ui.tableWidget->setItem(rows-1, 0, new QTableWidgetItem(result.c_str()));

		edited = true;
		 
	});

	connect(ui.deleteButton, &QPushButton::clicked, this, [&] {
		
		auto currentRow = ui.tableWidget->currentRow();

		if (currentRow < 0) return;

		bool confirm = ModalDialogBuilder::askDialog("Сигурни ли сте, че искате да изтриете шаблона?");

		if (!confirm) return;

		ui.tableWidget->removeRow(currentRow);

		edited = true;

	});

	connect(ui.okButton, &QPushButton::clicked, this, [&] {

		auto currentRow = ui.tableWidget->currentRow();

		if (currentRow < 0) return;

		m_result = ui.tableWidget->item(currentRow, 0)->text();
		
		close();
	});

	connect(ui.tableWidget, &QTableWidget::doubleClicked, this, [&] {
		emit ui.okButton->clicked();
	});
}

NotesTemplateDialog::~NotesTemplateDialog()
{
	if (!edited) return;

	std::vector<std::string> toInsert;

	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		toInsert.push_back(ui.tableWidget->item(i, 0)->text().toStdString());
	}

    DbNotes::setTemplateNotes(toInsert, m_type);

}
