#include "UpdateDownloader.h"

#include "qapplication.h"
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>

#include "Network/NetworkManager.h"
#include "Path.h"
#include "View/ModalDialogBuilder.h"

UpdateDownloader::UpdateDownloader(const char* url, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle(u8"Изтегляне на актуализацията");
	connect(this, &QDialog::rejected, [=] {

		if (m_reply) { 
			m_reply->close(); 
			m_reply->deleteLater();
		}
	});

	connect(ui.pushButton_2, &QPushButton::clicked, [=] { reject(); });


	m_reply = NetworkManager::simpleRequest(url);

	connect(m_reply, &QNetworkReply::downloadProgress, [=](qint64 recieved, qint64 total)
		{
			ui.progressBar->setRange(0, total);
			ui.progressBar->setValue(recieved);
		}
	);

	connect(m_reply, &QNetworkReply::finished, [=]
		{
			if (m_reply->error()) {
				accept();
				return;
			};

			auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

			if (!dataFolder.exists()) dataFolder.mkpath(".");

			auto filePath = dataFolder.filePath("DinoSetup.exe");

			QFile output(filePath);

			if (output.exists()) output.remove();

			output.open(QIODevice::ReadWrite);

			output.write(m_reply->readAll());

			output.close();


			m_reply->deleteLater();

			QProcess process;

			process.startDetached(filePath);
			
			file_downloaded = true;

			accept();

		}
	);


}


UpdateDownloader::~UpdateDownloader()
{}
