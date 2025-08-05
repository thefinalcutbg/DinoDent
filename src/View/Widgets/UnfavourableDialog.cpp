#include "UnfavourableDialog.h"
#include "Model/User.h"
#include "ui_UnfavourableDialog.h"

#include "View/Widgets/TableViewDialog.h"
#include "Model/Ekatte.h"
#include "Model/FreeFunctions.h"
#include "View/TableModels/PlainTableModel.h"
#include "Database/DbPractice.h"

UnfavourableDialog::UnfavourableDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UnfavourableDialog)
{   
    ui->setupUi(this);

    setWindowTitle("Неблагоприятни условия");

    ui->addButton->setIcon(QIcon(":/icons/icon_add.png"));
    ui->removeButton->setIcon(QIcon(":/icons/icon_remove.png"));

    auto unfavList = DbPractice::getUnfavEkatte(User::practice().rziCode);

    for(auto idx : unfavList){
        ui->listWidget->addItem(Ekatte(idx).getString(false).c_str());
    }

    connect(ui->addButton, &QPushButton::clicked, this, [&]{

        PlainTable t;

        t.addColumn(PlainColumn("ЕКАТТЕ"));
        t.addColumn(PlainColumn("Населено място"));

        for(auto&[name, idx] : Ekatte::cityNameToIdx())
        {
            t.addCell(0, PlainCell{.data = FreeFn::leadZeroes(idx, 5)});
            t.addCell(1, PlainCell{.data = Ekatte(idx).getString(false)});
        }

        PlainTableModel m;
        m.setTableData(t);

        TableViewDialog d(m, 1);
        d.setWindowTitle("Добавяне на населено място");
        d.exec();
        auto cityStr = d.getResult();

        if(cityStr.empty()) return;

        ui->listWidget->addItem(cityStr.c_str());
    });

    connect(ui->removeButton, &QPushButton::clicked, this, [&]{
        if (QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow()))
            delete item;

    });

    connect(ui->okButton, &QPushButton::clicked, this, [&]{
        std::set<int> unfavList;

        for (int i = 0; i < ui->listWidget->count(); ++i) {

            auto ekatteIdx = Ekatte::cityNameToIdx().at(ui->listWidget->item(i)->text().toStdString());

            unfavList.insert(ekatteIdx);
        }

        DbPractice::setUnfavEkatte(unfavList, User::practice().rziCode);

        Ekatte::s_unfavList = unfavList;

        close();
    });
}


UnfavourableDialog::~UnfavourableDialog()
{
    delete ui;
}
