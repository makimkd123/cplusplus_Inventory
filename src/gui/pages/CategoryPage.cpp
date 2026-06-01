#include "gui/pages/CategoryPage.h"
#include "gui/dialogs/CategoryDialog.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>

CategoryPage::CategoryPage(
    InventoryService& inventoryService,
    QWidget* parent
)
    : QWidget(parent),
      inventoryService(inventoryService)
{
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search categories...");

    categoryTable = new QTableWidget(this);
    categoryTable->setColumnCount(4);

    categoryTable->setHorizontalHeaderLabels({
        "CategoryID",
        "Name",
        "Description",
        "IsActive"
    });

    categoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    categoryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    categoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    categoryTable->horizontalHeader()->setStretchLastSection(true);

    refreshButton = new QPushButton("Refresh", this);
    addButton = new QPushButton("Add Category", this);
    editButton = new QPushButton("Edit Category", this);
    deactivateButton = new QPushButton("Deactivate Category", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deactivateButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(searchBar);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(categoryTable);

    setLayout(mainLayout);

    connect(refreshButton, &QPushButton::clicked, this, &CategoryPage::onRefreshClicked);
    connect(searchBar, &QLineEdit::textChanged, this, &CategoryPage::onSearchTextChanged);
    connect(categoryTable, &QTableWidget::itemSelectionChanged, this, &CategoryPage::onSelectionChanged);
    connect(addButton, &QPushButton::clicked, this, &CategoryPage::onAddClicked);
    connect(editButton, &QPushButton::clicked, this, &CategoryPage::onEditClicked);
    connect(deactivateButton, &QPushButton::clicked, this, &CategoryPage::onDeactivateClicked);

    updateButtonStates();
    loadCategories();
}

void CategoryPage::loadCategories(const QString& keyword) {
    categoryTable->setRowCount(0);

    auto categories = inventoryService.getCategories();

    for (const auto& category : categories) {
        if (!keyword.isEmpty()) {
            QString name = QString::fromStdString(category.getName());
            QString description = QString::fromStdString(category.getDescription());

            if (!name.contains(keyword, Qt::CaseInsensitive) &&
                !description.contains(keyword, Qt::CaseInsensitive)) {
                continue;
            }
        }

        int row = categoryTable->rowCount();
        categoryTable->insertRow(row);

        categoryTable->setItem(
            row,
            0,
            new QTableWidgetItem(QString::number(category.getCategoryId()))
        );

        categoryTable->setItem(
            row,
            1,
            new QTableWidgetItem(QString::fromStdString(category.getName()))
        );

        categoryTable->setItem(
            row,
            2,
            new QTableWidgetItem(QString::fromStdString(category.getDescription()))
        );

        categoryTable->setItem(
            row,
            3,
            new QTableWidgetItem(category.getIsActive() ? "ACTIVE" : "INACTIVE")
        );

        if (!category.getIsActive()) {
            for (int col = 0; col < categoryTable->columnCount(); ++col) {
                categoryTable->item(row, col)->setBackground(
                    QBrush(QColor(255, 220, 220))
                );
            }
        }
    }

    updateButtonStates();
}

int CategoryPage::selectedCategoryId() const {
    int row = categoryTable->currentRow();

    if (row < 0) {
        return -1;
    }

    return categoryTable->item(row, 0)->text().toInt();
}

void CategoryPage::updateButtonStates() {
    bool hasSelection = selectedCategoryId() != -1;

    editButton->setEnabled(hasSelection);
    deactivateButton->setEnabled(hasSelection);
}

void CategoryPage::onRefreshClicked() {
    loadCategories(searchBar->text());
}

void CategoryPage::onSearchTextChanged(const QString& text) {
    loadCategories(text);
}

void CategoryPage::onSelectionChanged() {
    updateButtonStates();
}

void CategoryPage::onAddClicked() {
    CategoryDialog dialog(inventoryService, this);

    if (dialog.exec() == QDialog::Accepted) {
        loadCategories(searchBar->text());
    }
}

void CategoryPage::onEditClicked() {
    int categoryId = selectedCategoryId();

    if (categoryId == -1) {
        QMessageBox::warning(this, "No selection", "Please select a category.");
        return;
    }

    CategoryDialog dialog(inventoryService, categoryId, this);

    if (dialog.exec() == QDialog::Accepted) {
        loadCategories(searchBar->text());
    }
}

void CategoryPage::onDeactivateClicked() {
    int categoryId = selectedCategoryId();

    if (categoryId == -1) {
        QMessageBox::warning(this, "No selection", "Please select a category.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deactivation",
        "Are you sure you want to deactivate this category?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) {
        return;
    }

    ServiceResult result = inventoryService.deactivateCategory(categoryId);

    if (result.success) {
        QMessageBox::information(this, "Success", QString::fromStdString(result.message));
        loadCategories(searchBar->text());
    } else {
        QMessageBox::warning(this, "Error", QString::fromStdString(result.message));
    }
}