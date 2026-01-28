#include "PracticeSelectorView.h"
#include <QPainter>
#include "View/Theme.h"

PracticeSelectorView::PracticeSelectorView(PracticeManagerPresenter& p, QWidget *parent)
	: QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	setWindowTitle("Списък с практики");

	ui.backButton->setCursor(QCursor(Qt::PointingHandCursor));
	ui.backButton->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));

    connect(ui.dbButton, &QPushButton::clicked, this, [&] {presenter.dbChangePath(); });
    connect(ui.addButton, &QPushButton::clicked, this, [&] {presenter.addClicked(); });
    connect(ui.removeButton, &QPushButton::clicked, this, [&] {presenter.removeClicked(ui.listWidget->currentIndex().row());});
    connect(ui.backButton, &QPushButton::clicked, this, [&] { close();});

    presenter.setView(this);
}

void PracticeSelectorView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
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