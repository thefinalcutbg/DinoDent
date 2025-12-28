#include "MultilineDialog.h"
#include <QClipboard>
#include <QApplication>
#include <QTextCursor>

#include "View/Widgets/NotesTemplateDialog.h"

MultilineDialog::MultilineDialog(const std::string& text, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Конзола");

	ui.setupUi(this);
	ui.textBox->setReadOnly(true);
    ui.textBox->setText(text);

    ui.templateButton->setIcon(QPixmap(":/icons/icon_template.png"));

    ui.templateButton->hide();
    ui.templateCheck->hide();

    connect(ui.templateButton, &QPushButton::clicked, this, [&] {
            NotesTemplateDialog d(m_template_type);

            auto result = d.getResult();

            if(result.size()){
                ui.textBox->setText(result.toStdString(), false);
            }

            ui.templateCheck->setChecked(false);
    });

    connect(ui.copyButton, &QPushButton::clicked, this, [&] {
		ui.textBox->selectAll();
		QClipboard* clipboard = QApplication::clipboard();
		clipboard->setText(ui.textBox->toPlainText());
	});

    connect(ui.okButton, &QPushButton::clicked, this, [&] { 

        m_result = ui.textBox->isReadOnly() ?
			ui.textBox->toPlainText().toStdString()
			:
            ui.textBox->getText();


        if(ui.templateCheck->isVisible() && ui.templateCheck->isChecked() && m_result->size()){
            DbNotes::insertTemplateNote(m_result.value(), m_template_type);
        }

        accept();
	});
}

std::optional<std::string> MultilineDialog::getResult()
{
    exec();

    return m_result;
}

void MultilineDialog::enableEditing()
{
	ui.textBox->setReadOnly(false);
	ui.copyButton->hide();
    ui.textBox->setFocus();
}

void MultilineDialog::enableNotesFormat()
{
     ui.textBox->setText(ui.textBox->toPlainText().toStdString(), true);
}

void MultilineDialog::enableTemplateLoading(DbNotes::TemplateType type)
{
    m_template_type = type;

    ui.templateButton->show();
    ui.templateCheck->show();
}

MultilineDialog::~MultilineDialog()
{
}
