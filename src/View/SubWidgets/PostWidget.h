#pragma once

#include <QWidget>
#include "ui_PostWidget.h"

struct PostData;

class PostWidget : public QWidget
{
	Q_OBJECT

public:
	PostWidget(QWidget *parent = nullptr);
	PostData getData();
	void setData(const PostData& data);
	~PostWidget();

private:
	Ui::PostWidgetClass ui;
};
