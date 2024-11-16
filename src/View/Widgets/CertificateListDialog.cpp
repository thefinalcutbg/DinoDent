#include "CertificateListDialog.h"
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include "Model/Date.h"
#include "View/Theme.h"

class HtmlDelegate : public QStyledItemDelegate
{
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle* style = optionV4.widget ? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(optionV4.text);

    /// Painting item without text
    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected) {
        painter->fillRect(optionV4.rect, Theme::mainBackgroundColor);
    }

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem optionV4 = option;
    initStyleOption(&optionV4, index);

    QTextDocument doc;
    doc.setHtml(optionV4.text);
    doc.setTextWidth(optionV4.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}


void CertificateListDialog::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
}

CertificateListDialog::CertificateListDialog(const std::vector<X509Details>& certList, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

    setWindowTitle("Изберете сертификат");

    auto delegate = new HtmlDelegate();

    ui.tableWidget->setItemDelegate(delegate);
    ui.tableWidget->setRowCount(certList.size());
    ui.tableWidget->verticalHeader()->setDefaultSectionSize(70);

    for (int i = 0; i < certList.size(); i++)
    {
        auto& cert = certList[i];

        QString text = "<b>" + QString::fromStdString(cert.name) + "</b>&nbsp;&nbsp;&nbsp;";
        text += "Валиден до: <b><span style=\"";
        text += cert.isValid() ? "color:green;\">" : "color:red;\">";

        text += Date(cert.validTo8601).toBgStandard(true).c_str();
        text += "</span></b><br>";

        if (cert.organization.size()) {
            text += QString::fromStdString(cert.organization);
            text += "<br>";
        }

        text += QString::fromStdString(cert.issuer);


        auto item = new QTableWidgetItem(text);

        ui.tableWidget->setItem(i, 0, item);
    }

    if (certList.size()) {
        ui.tableWidget->selectRow(0);
    }

    connect(ui.tableWidget, &QTableWidget::doubleClicked, this, [&] {
        ui.okButton->click();
    });

    connect(ui.okButton, &QPushButton::clicked, this, [&] {
        m_index = ui.tableWidget->selectionModel()->currentIndex().row();
        if (m_index == -1) return;

        close();
    });
}

CertificateListDialog::~CertificateListDialog()
{}
