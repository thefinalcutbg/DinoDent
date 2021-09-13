#pragma once

#include <QGraphicsView>
#include "PerioScene.h"

class PerioView : public QGraphicsView
{
	Q_OBJECT

public:
	PerioView(QWidget *parent);
	~PerioView();
};
