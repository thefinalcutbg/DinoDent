#include "PracticeSelectorView.h"
#include <QPainter>
#include <qtextcodec.h>
#include "View/Theme.h"
PracticeSelectorView::PracticeSelectorView(PracticeManagerPresenter* p, QWidget *parent)
	: QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	setWindowTitle("Списък с практики");

	ui.backButton->setCursor(QCursor(Qt::PointingHandCursor));
	ui.backButton->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));

	connect(ui.dbButton, &QPushButton::clicked, [=] {presenter->dbChangePath(); });
	connect(ui.addButton, &QPushButton::clicked, [=] {presenter->addClicked(); });
	connect(ui.removeButton, &QPushButton::clicked, [=] {presenter->removeClicked(ui.listWidget->currentIndex().row());});
	connect(ui.editButton, &QPushButton::clicked, [=] {presenter->editClicked(ui.listWidget->currentIndex().row());});
	connect(ui.listWidget, &QListWidget::doubleClicked, [=] {presenter->editClicked(ui.listWidget->currentIndex().row());});
	connect(ui.backButton, &QPushButton::clicked, [&] { close();});
	presenter->setView(this);

}

void PracticeSelectorView::paintEvent(QPaintEvent* e)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}


PracticeSelectorView::~PracticeSelectorView()
{}

void PracticeSelectorView::setPracticeList(const std::vector<std::string>&practiceList)
{
	ui.listWidget->clear();

	for (auto& p : practiceList) ui.listWidget->addItem(p.c_str());

	ui.listWidget->setCurrentItem(0);
}

void PracticeSelectorView::setDbPath(const std::string& dbPath)
{
	ui.dbLine->setText(dbPath.c_str());
}
