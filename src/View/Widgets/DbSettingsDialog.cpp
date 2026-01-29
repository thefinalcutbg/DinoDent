#include "DbSettingsDialog.h"
#include <QFileDialog>

DbSettingsDialog::DbSettingsDialog(const DbSettings& s, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DbSettingsDialogClass())
{
	ui->setupUi(this);

	setWindowTitle("Настройки на база данни");

	setStyleSheet("QDialog {background-color: white;}");

	connect(ui->okButton, &QPushButton::clicked, this, [&] { accept(); });
	connect(ui->cancelButton, &QPushButton::clicked, this, [&] { reject(); });

	connect(ui->localGroup, &QGroupBox::clicked, this, [&] { ui->serverGroup->setChecked(false); });
	connect(ui->serverGroup, &QGroupBox::clicked, this, [&] { ui->localGroup->setChecked(false); });

	connect(ui->pathButton, &QPushButton::clicked, this, [&] {

		auto str = QFileDialog::getOpenFileName(
			nullptr,
			"Изберете местонахождение на бaзата данни",
			ui->pathLineEdit->text(), "Файл база данни (*.db)"
		);

		if (str.isEmpty()) return;

		ui->pathLineEdit->setText(str);

	});

	ui->localGroup->setChecked(s.mode == DbSettings::DbType::Sqlite);
	ui->serverGroup->setChecked(s.mode == DbSettings::DbType::Rqlite);

	ui->pathLineEdit->setText(s.sqliteFilePath.c_str());
	ui->addressLineEdit->setText(s.rqliteUrl.c_str());
}

std::optional<DbSettings> DbSettingsDialog::getResult() {

	if (exec() != QDialog::Accepted) return {};

	DbSettings s;

	s.mode = ui->localGroup->isChecked() ? DbSettings::DbType::Sqlite : DbSettings::DbType::Rqlite;

	s.rqliteUrl = ui->addressLineEdit->text().toStdString();

	s.sqliteFilePath = ui->pathLineEdit->text().toStdString();

	if (s.rqliteUrl.empty()) {
		s.rqliteUrl = "127.0.0.1:4002";
	}

	return s;
}

DbSettingsDialog::~DbSettingsDialog()
{
	delete ui;
}

