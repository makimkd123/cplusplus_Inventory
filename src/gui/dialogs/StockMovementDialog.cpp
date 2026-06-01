#include "gui/dialogs/StockMovementDialog.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include <QHBoxLayout>
#include <QFormLayout>

StockMovementDialog::StockMovementDialog(
    MovementType movementType,
    QWidget* parent
)
    : QDialog(parent),
      movementType(movementType)
{
    reasonComboBox = new QComboBox(this);
    quantityInput = new QDoubleSpinBox(this);

    quantityInput->setMinimum(0.00);
    quantityInput->setMaximum(1000000.0);
    quantityInput->setDecimals(2);

    if (movementType == MovementType::STOCK_IN) {
        setWindowTitle("Stock In");

        reasonComboBox->addItem("Bought");
        reasonComboBox->addItem("Other");
    }
    else {
        setWindowTitle("Stock Out");

        reasonComboBox->addItem("Sold");
        reasonComboBox->addItem("Damaged");
        reasonComboBox->addItem("Other");
    }

    QPushButton* okButton = new QPushButton("OK", this);
    QPushButton* cancelButton = new QPushButton("Cancel", this);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Reason:", reasonComboBox);
    formLayout->addRow("Quantity:", quantityInput);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    connect(okButton, &QPushButton::clicked,
            this, &QDialog::accept);

    connect(cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
}
double StockMovementDialog::quantity() const {
    return quantityInput->value();
}
MovementReason StockMovementDialog::selectedReason() const {
    QString reasonText = reasonComboBox->currentText();

    if (reasonText == "Bought") {
        return MovementReason::BOUGHT;
    }

    if (reasonText == "Sold") {
        return MovementReason::SOLD;
    }

    if (reasonText == "Damaged") {
        return MovementReason::DAMAGED;
    }

    return MovementReason::OTHER;
}