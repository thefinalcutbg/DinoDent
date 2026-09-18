#pragma once

#include <QDialog>
#include "ui_PrintPreviewDialog.h"

#ifdef Q_OS_WIN //in MSVC build limereport is dynamic lib
#include <LimeReport/include/lrreportengine.h>
#else
#include "lrreportengine.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class PrintPreviewDialogClass; };
QT_END_NAMESPACE

class PrintPreviewDialog : public QDialog
{
	Q_OBJECT

	int currentPage = 0;
	int pageCount = 0;
	QGraphicsScene* scene = nullptr;
	QVector<QRectF> pageRects;

	void fitSceneWidth();
	void gotoPage(int page);
	void updatePageControls();
	void detectCurrentPage();

public:
	PrintPreviewDialog(LimeReport::ReportEngine& r);
	~PrintPreviewDialog();

private:
	Ui::PrintPreviewDialogClass *ui;
};

