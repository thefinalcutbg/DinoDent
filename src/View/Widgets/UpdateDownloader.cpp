#include "UpdateDownloader.h"

#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QPainter>
#include "Network/NetworkManager.h"
#include "View/ModalDialogBuilder.h"

UpdateDownloader::UpdateDownloader(const char* url, QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setModal(true);
    setWindowTitle("Изтегляне на актуализацията");

    connect(this, &QDialog::rejected, this, [=, this] {

        if (m_reply) {
            m_reply->close();
            m_reply->deleteLater();
        }
    });

    connect(ui.pushButton_2, &QPushButton::clicked, this, [=, this] { reject(); });


    m_reply = NetworkManager::simpleRequest(url);

    connect(m_reply, &QNetworkReply::downloadProgress, this, [&](qint64 recieved, qint64 total)
        {
            ui.progressBar->setRange(0, total);
            ui.progressBar->setValue(recieved);
        }
    );

    connect(m_reply, &QNetworkReply::finished, this, [=, this]
        {
            if (m_reply->error()) {
                accept();
                return;
            };

            auto dataFolder = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

            if (!dataFolder.exists()) dataFolder.mkpath(".");

#ifdef Q_OS_WIN
            auto filePath = dataFolder.filePath("DinoSetup.exe");
#elif defined(Q_OS_LINUX)
            auto filePath = dataFolder.filePath("DinoSetup");
#elif defined(Q_OS_MAC)
            auto filePath = dataFolder.filePath("DinoSetup.dmg");
#endif
            QFile output(filePath);

            if (output.exists()) output.remove();

            output.open(QIODevice::ReadWrite);

            output.write(m_reply->readAll());

            output.close();


            m_reply->deleteLater();

            ModalDialogBuilder::showMessage(
                "Натиснете ОК за да стартирате инсталатора. "
                "\nНе стартирайте програмата по време на инсталация!"
            );

            QProcess process;

#ifdef Q_OS_WIN
            process.startDetached(filePath);
#endif

#ifdef Q_OS_MAC
            process.execute("open", {filePath});
#endif

            file_downloaded = true;

            accept();

        }
    );
}

void UpdateDownloader::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
}

UpdateDownloader::~UpdateDownloader()
{}
