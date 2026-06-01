#include "gui/dialogs/CategoryDialog.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

#include <algorithm>

CategoryDialog::CategoryDialog(
    InventoryService& inventoryService,
    QWidget* parent
)
    : QDialog(parent),
      inventoryService(inventoryService),
      categoryId(-1),
      editMode(false)
{
    setWindowTitle("Add Category");

    nameEdit = new QLineEdit(this);
    descriptionEdit = new QLineEdit(this);

    isActiveCheck = new QCheckBox("Active", this);
    isActiveCheck->setChecked(true);
    isActiveCheck->hide();

    QPushButton* saveButton = new QPushButton("Save", this);
    QPushButton* cancelButton = new QPushButton("Cancel", this);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Description:", descriptionEdit);
    formLayout->addRow("", isActiveCheck);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &CategoryDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &CategoryDialog::reject);
}

CategoryDialog::CategoryDialog(
    InventoryService& inventoryService,
    int categoryId,
    QWidget* parent
)
    : CategoryDialog(inventoryService, parent)
{
    this->categoryId = categoryId;
    this->editMode = true;

    setWindowTitle("Edit Category");

    isActiveCheck->show();

    loadCategory();
}

void CategoryDialog::onSaveClicked() {
    saveCategory();
}

void CategoryDialog::loadCategory() {
    auto categories = inventoryService.getCategories();

    auto it = std::find_if(
        categories.begin(),
        categories.end(),
        [this](const Category& category) {
            return category.getCategoryId() == categoryId;
        }
    );

    if (it == categories.end()) {
        QMessageBox::warning(this, "Error", "Category not found.");
        reject();
        return;
    }

    nameEdit->setText(QString::fromStdString(it->getName()));
    descriptionEdit->setText(QString::fromStdString(it->getDescription()));
    isActiveCheck->setChecked(it->getIsActive());
}

void CategoryDialog::saveCategory() {
    QString name = nameEdit->text().trimmed();
    QString description = descriptionEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Category name is required.");
        return;
    }

    ServiceResult result;

    if (editMode) {
        result = inventoryService.updateCategory(
            categoryId,
            name.toStdString(),
            description.toStdString(),
            isActiveCheck->isChecked()
        );
    } else {
        result = inventoryService.addCategory(
            name.toStdString(),
            description.toStdString()
        );
    }

    if (result.success) {
        accept();
    } else {
        QMessageBox::warning(this, "Error", QString::fromStdString(result.message));
    }
}