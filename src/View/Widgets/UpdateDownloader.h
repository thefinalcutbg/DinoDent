#pragma once

#include <QDialog>
#include "ui_UpdateDownloader.h"

class QNetworkReply;

class UpdateDownloader : public QDialog
{
	Q_OBJECT

	QNetworkReply* m_reply{ nullptr };
	bool file_downloaded{ false };

public:
	UpdateDownloader(const char* url, QWidget* parent = nullptr);
	bool installerDownloaded() { return file_downloaded; }
	~UpdateDownloader();

private:
	Ui::UpdateDownloaderClass ui;
};
