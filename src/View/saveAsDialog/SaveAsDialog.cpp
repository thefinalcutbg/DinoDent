#include "SaveAsDialog.h"
#include "View/ModalDialogBuilder.h"

SaveAsDialog::SaveAsDialog(std::unordered_set<int>& existingNumbers, int currentNumber, QWidget* parent)
    : QDialog(parent), existingNumbers(existingNumbers), currentNumber(currentNumber)
{
    ui.setupUi(this);

    setWindowTitle(u8"Запази като");

    ui.ambNumSpin->setValue(currentNumber);

    connect(ui.okButton, &QPushButton::clicked, this,
        [=] { 

            auto value = ui.ambNumSpin->value();
            if (existingNumbers.count(value))
            {
                bool answer = ModalDialogBuilder::askDialog
                (u8"Амбулаторен лист с такъв номер вече съществува."
                    u8"Сигурни ли сте, че искате да дублирате номерацията?");

                if (!answer) return;
            }

                done(ui.ambNumSpin->value()); close();
        
        
        });
    connect(ui.cancelButton, &QPushButton::clicked, this, [=] { done(0); close(); });
}


SaveAsDialog::~SaveAsDialog()
{

}



