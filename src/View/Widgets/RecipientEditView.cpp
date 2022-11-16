#include "RecipientEditView.h"

#include <QPainter>

RecipientEditView::RecipientEditView(const Recipient& r, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Редакция на получател");

	ui.nameEdit->setText(r.name.c_str());
	ui.addressEdit->setText(r.address.c_str());
	ui.idEdit->setText(r.bulstat.c_str());
	ui.phoneEdit->setText(r.phone.c_str());

	connect(ui.okButton, &QPushButton::clicked, [&] {

			Recipient r;
			r.name = ui.nameEdit->text().toStdString();
			r.address = ui.addressEdit->text().toStdString();
			r.bulstat = ui.idEdit->text().toStdString();
			r.phone = ui.phoneEdit->text().toStdString();
			
			m_recipient.emplace(r);

			close();
		}
	);
}

void RecipientEditView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}


RecipientEditView::~RecipientEditView()
{}
