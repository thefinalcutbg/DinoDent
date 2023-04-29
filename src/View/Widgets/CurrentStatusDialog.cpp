#include "CurrentStatusDialog.h"

#include <QPainter>

#include "Presenter/ToothHintCreator.h"
#include "View/Graphics/TeethViewScene.h"



CurrentStatusDialog::CurrentStatusDialog(const ToothContainer& t, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Текущ статус на пациента");

	auto hints = ToothHintCreator::getTeethHint(t);

	TeethViewScene* s = new TeethViewScene();

	ui.graphicsView->setScene(s);
	
	for (auto& hint : hints)
	{
		s->display(hint);
	}
	ui.graphicsView->setDisabled(true);

	connect(ui.pushButton, &QPushButton::clicked, [this] { accept(); });
}

void CurrentStatusDialog::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

CurrentStatusDialog::~CurrentStatusDialog()
{}
