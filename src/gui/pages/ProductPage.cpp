#include "gui/pages/ProductPage.h"
#include "gui/dialogs/ProductDialog.h"
#include "gui/dialogs/StockMovementDialog.h"



#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QBrush>
#include <QColor>

#include <unordered_set>
#include <optional>
#include <vector>

#include "utils/Utils.h"

ProductPage::ProductPage(
    InventoryService& inventoryService,
    QWidget* parent
)
    : QWidget(parent),
      inventoryService(inventoryService)
{
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search products...");

    productTable = new QTableWidget(this);
    productTable->setColumnCount(5);

    productTable->setHorizontalHeaderLabels({
        "ID",
        "Name",
        "Barcode",
        "Quantity",
        "Status"
    });

    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->setSelectionMode(QAbstractItemView::SingleSelection);
    productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    productTable->horizontalHeader()->setStretchLastSection(true);

    refreshButton = new QPushButton("Refresh", this);
    detailsButton = new QPushButton("Show Product Details", this);
    addButton = new QPushButton("Add Product", this);
    editButton = new QPushButton("Edit Product", this);
    stockInButton = new QPushButton("Stock In", this);
    stockOutButton = new QPushButton("Stock Out", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(detailsButton);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(stockInButton);
    buttonLayout->addWidget(stockOutButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(searchBar);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(productTable);

    setLayout(mainLayout);

    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        &ProductPage::onRefreshClicked
    );

    connect(
        searchBar,
        &QLineEdit::textChanged,
        this,
        &ProductPage::onSearchTextChanged
    );

    connect(
        productTable,
        &QTableWidget::itemSelectionChanged,
        this,
        &ProductPage::onSelectionChanged
    );

    connect(
        detailsButton,
        &QPushButton::clicked,
        this,
        &ProductPage::onDetailsClicked
    );

    connect(
        addButton,
        &QPushButton::clicked,
        this,
        &ProductPage::onAddClicked
    );

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        &ProductPage::onEditClicked
    );

    connect(
        stockInButton,
        &QPushButton::clicked,
        this,
        &ProductPage::onStockInClicked
    );

    connect(
        stockOutButton,
        &QPushButton::clicked,
        this,
        &ProductPage::onStockOutClicked
    );

    updateButtonStates();
    loadProducts();
}

void ProductPage::loadProducts(const QString& keyword) {
    productTable->setRowCount(0);

    try {
        std::unordered_set<int> lowStockProductIds;

        std::vector<Product> lowStockProducts =
            inventoryService.getLowStockProducts();

        for (const Product& product : lowStockProducts) {
            lowStockProductIds.insert(product.getId());
        }

        std::vector<Product> products =
            inventoryService.searchProducts(keyword.toStdString());

        for (const Product& product : products) {
            int row = productTable->rowCount();
            productTable->insertRow(row);

            productTable->setItem(
                row,
                0,
                new QTableWidgetItem(QString::number(product.getId()))
            );

            productTable->setItem(
                row,
                1,
                new QTableWidgetItem(QString::fromStdString(product.getName()))
            );

            productTable->setItem(
                row,
                2,
                new QTableWidgetItem(QString::fromStdString(product.getBarcode()))
            );

            productTable->setItem(
                row,
                3,
                new QTableWidgetItem(QString::number(product.getQuantity()))
            );

            productTable->setItem(
                row,
                4,
                new QTableWidgetItem(QString::fromStdWString(toString(product.getStatus())))
            );

            if (lowStockProductIds.find(product.getId()) != lowStockProductIds.end()) {
                productTable->item(row, 3)->setBackground(
                    QBrush(QColor(255, 180, 180))
                );
            }
        }

        productTable->resizeColumnsToContents();
        updateButtonStates();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(
            this,
            "Load Products Failed",
            e.what()
        );
    }
}

void ProductPage::onRefreshClicked() {
    loadProducts(searchBar->text());
}

void ProductPage::onSearchTextChanged(const QString& text) {
    loadProducts(text);
}

void ProductPage::onSelectionChanged() {
    updateButtonStates();
}

void ProductPage::updateButtonStates() {
    bool hasSelection = productTable->currentRow() >= 0;

    detailsButton->setEnabled(hasSelection);
    editButton->setEnabled(hasSelection);
    stockInButton->setEnabled(hasSelection);
    stockOutButton->setEnabled(hasSelection);
}

int ProductPage::selectedProductId() const {
    int row = productTable->currentRow();

    if (row < 0) {
        return -1;
    }

    QTableWidgetItem* idItem = productTable->item(row, 0);

    if (idItem == nullptr) {
        return -1;
    }

    return idItem->text().toInt();
}

void ProductPage::onDetailsClicked() {
    int productId = selectedProductId();

    if (productId == -1) {
        QMessageBox::warning(
            this,
            "No Product Selected",
            "Please select a product first."
        );
        return;
    }

    try {
        std::optional<Product> product =
            inventoryService.getProductDetails(productId);

        if (!product.has_value()) {
            QMessageBox::warning(
                this,
                "Product Not Found",
                "Could not find the selected product."
            );
            return;
        }

        QString details;

        details += "ID: " + QString::number(product->getId()) + "\n";
        details += "Name: " + QString::fromStdString(product->getName()) + "\n";
        details += "Barcode: " + QString::fromStdString(product->getBarcode()) + "\n";
        details += "Quantity: " + QString::number(product->getQuantity()) + "\n";
        details += "Status: " + QString::fromStdWString(toString(product->getStatus()));

        QMessageBox::information(
            this,
            "Product Details",
            details
        );
    }
    catch (const std::exception& e) {
        QMessageBox::critical(
            this,
            "Product Details Failed",
            e.what()
        );
    }
}

void ProductPage::onAddClicked() {
    ProductDialog dialog(inventoryService, this);

    if (dialog.exec() == QDialog::Accepted) {
        loadProducts(searchBar->text());
    }
}

void ProductPage::onEditClicked() {
    int productId = selectedProductId();

    if (productId == -1) {
        QMessageBox::warning(
            this,
            "No Product Selected",
            "Please select a product first."
        );
        return;
    }

    ProductDialog dialog(
        inventoryService,
        productId,
        this
    );

    if (dialog.exec() == QDialog::Accepted) {
        loadProducts(searchBar->text());
    }
}


void ProductPage::onStockInClicked() {
    int productId = selectedProductId();

    if (productId == -1) {
        QMessageBox::warning(this, "No Product Selected", "Please select a product first.");
        return;
    }

    StockMovementDialog dialog(MovementType::STOCK_IN, this);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    ServiceResult result = inventoryService.addStockMovement(
        productId,
        MovementType::STOCK_IN,
        dialog.selectedReason(),
        dialog.quantity()
    );

    if (!result.success) {
        QMessageBox::warning(this, "Stock In Failed", QString::fromStdString(result.message));
        return;
    }

    loadProducts(searchBar->text());
}

void ProductPage::onStockOutClicked() {
    int productId = selectedProductId();

    if (productId == -1) {
        QMessageBox::warning(this, "No Product Selected", "Please select a product first.");
        return;
    }

    StockMovementDialog dialog(MovementType::STOCK_OUT, this);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    ServiceResult result = inventoryService.addStockMovement(
        productId,
        MovementType::STOCK_OUT,
        dialog.selectedReason(),
        dialog.quantity()
    );

    if (!result.success) {
        QMessageBox::warning(this, "Stock Out Failed", QString::fromStdString(result.message));
        return;
    }

    loadProducts(searchBar->text());
}