#include "AllergiesDialog.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"

AllergiesDialog::AllergiesDialog(AllergiesDialogPresenter* p)
    : presenter(p)
{
    ui.setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Алергии, настоящи и минали заболявания");

    p->setView(this);

    connect(ui.okButton, &QPushButton::clicked, this,
        [=]
        {
            presenter->okClicked();
        });

    connect(ui.cancelButton, &QPushButton::clicked, this, [=] {close(); });

    ui.okButton->setDefault(true);

}

void AllergiesDialog::setData(const Allergies& a)
{
    ui.allergiesEdit->set_Text(a.allergies);
    ui.pastEdit->set_Text(a.past);
    ui.currentEdit->set_Text(a.current);
}

Allergies AllergiesDialog::getData()
{
    return Allergies
    { 
        ui.allergiesEdit->getText(), 
        ui.currentEdit->getText(), 
        ui.pastEdit->getText() 
    };
}


void AllergiesDialog::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(0, 0, width(), height(), Qt::white);
    painter.end();
}


void AllergiesDialog::close()
{
    accept();
}


AllergiesDialog::~AllergiesDialog()
{
}

