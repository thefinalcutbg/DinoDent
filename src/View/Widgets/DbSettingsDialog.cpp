#include "DbSettingsDialog.h"
#include <QFileDialog>
#include "View/CommonIcon.h"

DbSettingsDialog::DbSettingsDialog(const DbSettings& s, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DbSettingsDialogClass())
{
	ui->setupUi(this);

	ui->mTLSFrame->hide();
	
	setWindowTitle("Настройки на база данни");

	setStyleSheet("QDialog {background-color: white;}");

	ui->caPathButton->setIcon(CommonIcon::getPixmap(CommonIcon::OPEN));
	ui->prvKeyButton->setIcon(CommonIcon::getPixmap(CommonIcon::OPEN));
	ui->certPathButton->setIcon(CommonIcon::getPixmap(CommonIcon::OPEN));
	ui->removeCAButton->setIcon(CommonIcon::getPixmap(CommonIcon::REMOVE));

	auto connectButtonToLineEdit = [this](QPushButton* b, QLineEdit* l, const QString& title, const QString& fileType)
	{
		connect(b, &QPushButton::clicked, this, [this, l, title, fileType]() {
			auto str = QFileDialog::getOpenFileName(this, title, l->text(), fileType);
			if (str.isEmpty()) return;
			l->setText(str);
		});
	};

	connectButtonToLineEdit(ui->pathButton, ui->pathLineEdit, "Изберете местоположение на базата данни", "База данни (*.db);;Всички файлове (*)");
	connectButtonToLineEdit(ui->certPathButton, ui->certPathLine, "Изберете PEM клиентски сертификат", "PEM сертификати (*.pem *.crt *.cer);;Всички файлове (*)");
	connectButtonToLineEdit(ui->prvKeyButton, ui->prvKeyLineEdit, "Изберете PEM частен ключ", "PEM частен ключ (*.key *.pem);;Всички файлове (*)");
	connectButtonToLineEdit(ui->caPathButton, ui->caPathLine, "Изберете PEM CA сертификат", "PEM CA сертификати (*.pem *.crt *.cer);;Всички файлове (*)");

	connect(ui->removeCAButton, &QPushButton::clicked, [&] { ui->caPathLine->clear(); });

	connect(ui->okButton, &QPushButton::clicked, this, [&] { 
		
		if (!ui->certPathLine->validateInput()) { return; };
		if (!ui->prvKeyLineEdit->validateInput()) { return; };

		accept();

	});
	connect(ui->cancelButton, &QPushButton::clicked, this, [&] { reject(); });

	connect(ui->localGroup, &QGroupBox::clicked, this, [&](bool clicked) { setDbBackend(!clicked); });
	connect(ui->serverGroup, &QGroupBox::clicked, this, [&](bool clicked) { setDbBackend(clicked); });


	connect(ui->mTLScheck, &QCheckBox::clicked, this, [this](bool checked) {
		ui->mTLSFrame->setHidden(!checked);
		ui->certPathLine->setInputValidator(checked ? &notEmptyValidator : nullptr);
		ui->prvKeyLineEdit->setInputValidator(checked ? &notEmptyValidator : nullptr);
	});

	ui->localGroup->setChecked(s.mode == DbSettings::DbType::Sqlite);
	ui->serverGroup->setChecked(s.mode == DbSettings::DbType::Rqlite);

	ui->pathLineEdit->setText(s.sqliteFilePath.c_str());
	ui->addressLineEdit->setText(s.rqliteUrl.c_str());
	ui->usrLineEdit->setText(s.rqliteUsr.c_str());
	ui->passLineEdit->setText(s.rqlitePass.c_str());

	ui->mTLScheck->setChecked(s.sslConfig.has_value());

	if (!s.sslConfig) return;

	ui->certPathLine->setText(s.sslConfig->clientCertPath.c_str());
	ui->prvKeyLineEdit->setText(s.sslConfig->clientKeyPath.c_str());
	ui->prvPassLineEdit->setText(s.sslConfig->clientKeyPass.c_str());
	ui->caPathLine->setText(s.sslConfig->caCertPath.c_str());

}

std::optional<DbSettings> DbSettingsDialog::getResult() {

	if (exec() != QDialog::Accepted) return {};

	DbSettings s;

	s.mode = ui->localGroup->isChecked() ? DbSettings::DbType::Sqlite : DbSettings::DbType::Rqlite;

	s.rqliteUrl = ui->addressLineEdit->text().toStdString();

	s.sqliteFilePath = ui->pathLineEdit->text().toStdString();

	s.rqliteUsr = ui->usrLineEdit->text().toStdString();

	s.rqlitePass = ui->passLineEdit->text().toStdString();

	if (s.rqliteUrl.empty()) {
		s.rqliteUrl = "http://localhost:4001";
	}

	if (!ui->mTLScheck->isChecked()) return s;

	DbSslConfig cfg;

	cfg.clientCertPath = ui->certPathLine->text().toStdString();
	cfg.clientKeyPath = ui->prvKeyLineEdit->text().toStdString();
	cfg.clientKeyPass = ui->prvPassLineEdit->text().toStdString();
	cfg.caCertPath = ui->caPathLine->text().toStdString();

	s.sslConfig = cfg;

	return s;
}

void DbSettingsDialog::setDbBackend(bool server)
{
	ui->serverGroup->setChecked(server);
	ui->localGroup->setChecked(!server);
	ui->rqliteLabel->setText(server ? "<a href=\"https://rqlite.io/\">https://rqlite.io/</a>" : "https://rqlite.io/");
}


DbSettingsDialog::~DbSettingsDialog()
{
	delete ui;
}

