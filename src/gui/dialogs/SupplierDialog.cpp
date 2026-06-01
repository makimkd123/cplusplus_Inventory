// SupplierDialog.cpp
#include "gui/dialogs/SupplierDialog.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <algorithm>

SupplierDialog::SupplierDialog(
    InventoryService& inventoryService,
    QWidget* parent
)
    : QDialog(parent),
      inventoryService(inventoryService),
      supplierId(-1),
      editMode(false)
{
    setWindowTitle("Add Supplier");

    nameEdit = new QLineEdit(this);
    contactNameEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    isActiveCheck = new QCheckBox("Active", this);
    isActiveCheck->setChecked(true);
    isActiveCheck->hide();

    QPushButton* saveButton = new QPushButton("Save", this);
    QPushButton* cancelButton = new QPushButton("Cancel", this);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Contact Name:", contactNameEdit);
    formLayout->addRow("Phone:", phoneEdit);
    formLayout->addRow("Email:", emailEdit);
    formLayout->addRow("Address:", addressEdit);
    formLayout->addRow("", isActiveCheck);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &SupplierDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SupplierDialog::reject);
}

SupplierDialog::SupplierDialog(
    InventoryService& inventoryService,
    int supplierId,
    QWidget* parent
)
    : SupplierDialog(inventoryService, parent)
{
    this->supplierId = supplierId;
    this->editMode = true;
    setWindowTitle("Edit Supplier");
    isActiveCheck->show();
    loadSupplier();
}

void SupplierDialog::onSaveClicked() {
    saveSupplier();
}

void SupplierDialog::loadSupplier() {
    auto suppliers = inventoryService.getSuppliers();

    auto it = std::find_if(
        suppliers.begin(),
        suppliers.end(),
        [this](const Supplier& supplier) {
            return supplier.getId() == supplierId;
        }
    );

    if (it == suppliers.end()) {
        QMessageBox::warning(
            this,
            "Error",
            "Supplier not found."
        );

        reject();
        return;
    }

    nameEdit->setText(
        QString::fromStdString(it->getName())
    );

    contactNameEdit->setText(
        QString::fromStdString(it->getContactName())
    );

    phoneEdit->setText(
        QString::fromStdString(it->getPhone())
    );

    emailEdit->setText(
        QString::fromStdString(it->getEmail())
    );

    addressEdit->setText(
        QString::fromStdString(it->getAddress())
    );

    isActiveCheck->setChecked(
        it->isActive()
    );
}

void SupplierDialog::saveSupplier() {
    QString name = nameEdit->text().trimmed();
    QString contactName = contactNameEdit->text().trimmed();
    QString phone = phoneEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    QString address = addressEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Supplier name is required.");
        return;
    }

    ServiceResult result;

    if (editMode) {
        result = inventoryService.updateSupplier(
            supplierId,
            name.toStdString(),
            contactName.toStdString(),
            phone.toStdString(),
            email.toStdString(),
            address.toStdString(),
            isActiveCheck->isChecked()
        );
    } else {
        result = inventoryService.addSupplier(
            name.toStdString(),
            contactName.toStdString(),
            phone.toStdString(),
            email.toStdString(),
            address.toStdString()
        );
    }

    if (result.success) {
        accept();
    } else {
        QMessageBox::warning(this, "Error", QString::fromStdString(result.message));
    }
}