#pragma once

#include <QMessageBox>

class SaveDialog : public QMessageBox
{
	Q_OBJECT

public:
	SaveDialog(QString title, QWidget *parent = 0);
	~SaveDialog();
};
