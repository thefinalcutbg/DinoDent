#include "DbSettingsDialog.h"
#include <QFileDialog>
#include "View/CommonIcon.h"

DbSettingsDialog::DbSettingsDialog(const DbSettings& s, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DbSettingsDialogClass())
{
	ui->setupUi(this);

    resize(width(), 320);

	setWindowTitle("Настройки на база данни");

	setStyleSheet("QDialog {background-color: white;}");

	ui->caPathButton->setIcon(CommonIcon::getPixmap(CommonIcon::OPEN));
	ui->prvKeyButton->setIcon(CommonIcon::getPixmap(CommonIcon::OPEN));
	ui->certPathButton->setIcon(CommonIcon::getPixmap(CommonIcon::OPEN));
	ui->removeCAButton->setIcon(CommonIcon::getPixmap(CommonIcon::REMOVE));
	ui->removeCertButton->setIcon(CommonIcon::getPixmap(CommonIcon::REMOVE));
	ui->removeKeyButton->setIcon(CommonIcon::getPixmap(CommonIcon::REMOVE));

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

	auto connectButtonToRemove = [this](QPushButton* b, QLineEdit* l) {
		connect(b, &QPushButton::clicked, this, [this, l]() {l->clear(); });
	};

	connectButtonToRemove(ui->removeCAButton, ui->caPathLine);
	connectButtonToRemove(ui->removeCertButton, ui->certPathLine);
	connectButtonToRemove(ui->removeKeyButton, ui->prvKeyLineEdit);

	connect(ui->okButton, &QPushButton::clicked, this, [&] { accept(); });
	connect(ui->cancelButton, &QPushButton::clicked, this, [&] { reject(); });

	connect(ui->localGroup, &QGroupBox::toggled, this, [&](bool clicked) { setDbBackend(!clicked); });
	connect(ui->serverGroup, &QGroupBox::toggled, this, [&](bool clicked) { setDbBackend(clicked); });


	ui->localGroup->setChecked(s.mode == DbSettings::DbType::Sqlite);
	ui->serverGroup->setChecked(s.mode == DbSettings::DbType::Rqlite);

	ui->pathLineEdit->setText(s.sqliteFilePath.c_str());
	ui->addressLineEdit->setText(s.dbServerConfig.rqliteUrl.c_str());
	ui->usrLineEdit->setText(s.dbServerConfig.rqliteUsr.c_str());
	ui->passLineEdit->setText(s.dbServerConfig.rqlitePass.c_str());
	ui->certPathLine->setText(s.dbServerConfig.clientCertPath.c_str());
	ui->prvKeyLineEdit->setText(s.dbServerConfig.clientKeyPath.c_str());
	ui->prvPassLineEdit->setText(s.dbServerConfig.clientKeyPass.c_str());
	ui->caPathLine->setText(s.dbServerConfig.caCertPath.c_str());
}

std::optional<DbSettings> DbSettingsDialog::getResult() {

	if (exec() != QDialog::Accepted) return {};
	
	DbSettings s;

	s.mode = ui->localGroup->isChecked() ? DbSettings::DbType::Sqlite : DbSettings::DbType::Rqlite;

	s.sqliteFilePath = ui->pathLineEdit->text().toStdString();

	s.dbServerConfig.rqliteUrl = ui->addressLineEdit->text().toStdString();
	s.dbServerConfig.rqliteUsr = ui->usrLineEdit->text().toStdString();
	s.dbServerConfig.rqlitePass = ui->passLineEdit->text().toStdString();

	if (s.dbServerConfig.rqliteUrl.empty()) {
		s.dbServerConfig.rqliteUrl = "http://localhost:4001";
	}

	s.dbServerConfig.clientCertPath = ui->certPathLine->text().toStdString();
	s.dbServerConfig.clientKeyPath = ui->prvKeyLineEdit->text().toStdString();
	s.dbServerConfig.clientKeyPass = ui->prvPassLineEdit->text().toStdString();
	s.dbServerConfig.caCertPath = ui->caPathLine->text().toStdString();

	return s;
}

void DbSettingsDialog::setDbBackend(bool server)
{
	ui->serverGroup->setChecked(server);
	ui->localGroup->setChecked(!server);
    ui->rqliteLabel->setText(server ?
        "За повече информация: <a href=\"https://www.dinodent.bg/rqlite\">dinodent.bg/rqlite</a>" :
        "За повече информация: dinodent.bg/rqlite");
}


DbSettingsDialog::~DbSettingsDialog()
{
	delete ui;
}

