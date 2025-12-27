#include "ProcedurePrintSelectDialog.h"
#include <QPainter>
#include "View/CommonIcon.h"
#include <QMouseEvent>

ProcedurePrintSelectDialog::ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, const std::vector<Referral>& referrals, QWidget* parent)
	: QDialog(parent), model(procedures)
{
	ui.setupUi(this);
	setModal(true);
    setWindowTitle("Избор на процедури");

    ui.tableView->viewport()->installEventFilter(this);

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	ui.tableView->setModel(&this->model);

	ui.tableView->setFocusPolicy(Qt::NoFocus);
	ui.tableView->setColumnWidth(0, 110);
	ui.tableView->setColumnWidth(1, 200);
    ui.tableView->setColumnWidth(2, 65);
	ui.tableView->setColumnWidth(3, 300);
    ui.tableView->setColumnWidth(4, 100);
    ui.tableView->setColumnWidth(5, 100);
	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	ui.tableView->setShowGrid(false);
	ui.tableView->verticalHeader()->hide();

	ui.patientButton->setIcon(CommonIcon::getPixmap(CommonIcon::PATIENT));
	ui.nhifButton->setIcon(CommonIcon::getPixmap(CommonIcon::NHIF));
	ui.phifButton->setIcon(CommonIcon::getPixmap(CommonIcon::PHIF));

	if (procedures.empty()) {
		ui.tableView->hide();
		ui.groupBox->hide();
		adjustSize();
	}

    connect(ui.okButton, &QPushButton::pressed, this, [=, this]
		{
			auto select = ui.tableView->selectionModel()->selectedIndexes();

			m_selectedRows = model.selectedRows();

			accept();

		});

    connect(ui.patientButton, &QPushButton::clicked, this, [&] { selectFinancingSource(FinancingSource::Patient); });
    connect(ui.nhifButton, &QPushButton::clicked, this, [&] { selectFinancingSource(FinancingSource::NHIF); });
	connect(ui.phifButton, &QPushButton::clicked, this, [&] { selectFinancingSource(FinancingSource::PHIF); });
	connect(ui.allButton, &QPushButton::clicked, this, [&] { model.selectAll(); });
	if (referrals.empty()) ui.referralCheck->setHidden(true);

}

const std::vector<int> ProcedurePrintSelectDialog::selectedProcedures() const
{
	return m_selectedRows;
}

bool ProcedurePrintSelectDialog::printReferrals() const
{
	return !ui.referralCheck->isHidden() && ui.referralCheck->isChecked();
}

void ProcedurePrintSelectDialog::selectFinancingSource(FinancingSource source)
{
	model.selectFinancingSource(source);
	ui.referralCheck->setChecked(source == FinancingSource::NHIF);
}

void ProcedurePrintSelectDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);
}

bool ProcedurePrintSelectDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui.tableView->viewport())
    {
        const auto type = event->type();
        if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick)
        {
            auto* me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton)
            {
                const QModelIndex clickedIndex = ui.tableView->indexAt(me->pos());
                if (!clickedIndex.isValid())
                    return QDialog::eventFilter(obj, event);

                const QModelIndex checkIndex = model.index(clickedIndex.row(), 0);

                //avoid double-toggling when user clicks the checkbox itself
                QStyleOptionViewItem opt;
                opt.initFrom(ui.tableView->viewport());
                opt.rect = ui.tableView->visualRect(checkIndex);
                opt.features |= QStyleOptionViewItem::HasCheckIndicator;
                opt.checkState = static_cast<Qt::CheckState>(model.data(checkIndex, Qt::CheckStateRole).toInt());
                opt.state |= (opt.checkState == Qt::Checked) ? QStyle::State_On : QStyle::State_Off;

                const QRect checkRect = ui.tableView->style()->subElementRect(
                    QStyle::SE_ItemViewItemCheckIndicator, &opt, ui.tableView->viewport());

                const bool clickedOnCheckbox = checkRect.contains(me->pos());

                // handle single-clicks on the checkbox but ignore double-clicks to avoid a second toggle.
                if (clickedOnCheckbox)
                    return (type == QEvent::MouseButtonDblClick);


                const bool isChecked = (opt.checkState == Qt::Checked);
                model.setData(checkIndex, isChecked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);


                return true; // swallow event
            }
        }
    }

    return QDialog::eventFilter(obj, event);
}

ProcedurePrintSelectDialog::~ProcedurePrintSelectDialog()
{
}
