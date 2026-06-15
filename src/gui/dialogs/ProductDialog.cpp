#include "gui/dialogs/ProductDialog.h"

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStringList>

#include "utils/Utils.h"

ProductDialog::ProductDialog(
    InventoryService& inventoryService,
    QWidget* parent
)
    : QDialog(parent),
      inventoryService(inventoryService),
      productId(std::nullopt)
{
    setupUi();
    loadSuppliers();
    loadCategories();

    setWindowTitle("Add Product");
    resize(500, 450);
}

ProductDialog::ProductDialog(
    InventoryService& inventoryService,
    int productId,
    QWidget* parent
)
    : QDialog(parent),
      inventoryService(inventoryService),
      productId(productId)
{
    setupUi();
    loadSuppliers();
    loadCategories();
    loadProductData();

    setWindowTitle("Edit Product");
}

void ProductDialog::setupUi() {
    nameInput = new QLineEdit(this);
    barcodeInput = new QLineEdit(this);

    unitComboBox = new QComboBox(this);
    unitComboBox->addItem("Kilogram", static_cast<int>(Unit::KG));
    unitComboBox->addItem("Liter", static_cast<int>(Unit::LITER));
    unitComboBox->addItem("Pieces", static_cast<int>(Unit::PIECES));

    supplierComboBox = new QComboBox(this);
    supplierComboBox->setEditable(true);
    supplierComboBox->setInsertPolicy(QComboBox::NoInsert);

    primaryCategoryComboBox = new QComboBox(this);
    primaryCategoryComboBox->setEditable(true);
    primaryCategoryComboBox->setInsertPolicy(QComboBox::NoInsert);

    subCategoryComboBox = new QComboBox(this);
    subCategoryComboBox->setEditable(true);
    subCategoryComboBox->setInsertPolicy(QComboBox::NoInsert);

    buyingPriceInput = new QDoubleSpinBox(this);
    sellingPriceInput = new QDoubleSpinBox(this);
    taxInput = new QDoubleSpinBox(this);

    heightInput = new QDoubleSpinBox(this);
    widthInput = new QDoubleSpinBox(this);
    depthInput = new QDoubleSpinBox(this);

    minimumQuantityInput = new QDoubleSpinBox(this);


    buyingPriceInput->setMaximum(1000000.0);
    sellingPriceInput->setMaximum(1000000.0);
    taxInput->setMaximum(100.0);

    heightInput->setMaximum(1000000.0);
    widthInput->setMaximum(1000000.0);
    depthInput->setMaximum(1000000.0);
    minimumQuantityInput->setMaximum(1000000);
    buyingPriceInput->setDecimals(2);
    sellingPriceInput->setDecimals(2);
    taxInput->setDecimals(2);
    minimumQuantityInput->setDecimals(2);

    heightInput->setDecimals(2);
    widthInput->setDecimals(2);
    depthInput->setDecimals(2);

    saveButton = new QPushButton("Save", this);
    cancelButton = new QPushButton("Cancel", this);

    QFormLayout* formLayout = new QFormLayout();

    formLayout->addRow("Name:", nameInput);
    formLayout->addRow("Barcode:", barcodeInput);
    formLayout->addRow("Unit:", unitComboBox);
    formLayout->addRow("Buying Price:", buyingPriceInput);
    formLayout->addRow("Selling Price:", sellingPriceInput);
    formLayout->addRow("Tax:", taxInput);
    formLayout->addRow("Height:", heightInput);
    formLayout->addRow("Width:", widthInput);
    formLayout->addRow("Depth:", depthInput);
    formLayout->addRow("Supplier:", supplierComboBox);
    formLayout->addRow("Primary Category:", primaryCategoryComboBox);
    formLayout->addRow("Subcategory:", subCategoryComboBox);
    formLayout->addRow("Minimum Quantity:", minimumQuantityInput);


    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    connect(
        saveButton,
        &QPushButton::clicked,
        this,
        &ProductDialog::onSaveClicked
    );

    connect(
        cancelButton,
        &QPushButton::clicked,
        this,
        &ProductDialog::reject
    );
}

void ProductDialog::loadSuppliers() {
    supplierComboBox->clear();

    std::vector<Supplier> suppliers = inventoryService.getSuppliers();

    for (const Supplier& supplier : suppliers) {
        supplierComboBox->addItem(
            QString::fromStdString(supplier.getName()),
            supplier.getId()
        );
    }
}

void ProductDialog::loadCategories() {
    primaryCategoryComboBox->clear();
    subCategoryComboBox->clear();

    subCategoryComboBox->addItem("None", -1);

    std::vector<Category> categories = inventoryService.getCategories();

    for (const Category& category : categories) {
        primaryCategoryComboBox->addItem(
            QString::fromStdString(category.getName()),
            category.getCategoryId()
        );

        subCategoryComboBox->addItem(
            QString::fromStdString(category.getName()),
            category.getCategoryId()
        );
    }
}

void ProductDialog::loadProductData() {
    if (!productId.has_value()) {
        return;
    }

    std::optional<Product> product =
        inventoryService.getProductDetails(*productId);

    if (!product.has_value()) {
        QMessageBox::warning(
            this,
            "Product Not Found",
            "Could not load product data."
        );
        reject();
        return;
    }

    nameInput->setText(QString::fromStdString(product->getName()));
    barcodeInput->setText(QString::fromStdString(product->getBarcode()));

    buyingPriceInput->setValue(product->getBuyingPrice());
    sellingPriceInput->setValue(product->getSellingPrice());
    taxInput->setValue(product->getTax());

    heightInput->setValue(product->getHeight());
    widthInput->setValue(product->getWidth());
    depthInput->setValue(product->getDepth());
    minimumQuantityInput->setValue(product->getMinimumQuantity());

    int unitIndex = unitComboBox->findData(
        static_cast<int>(product->getUnit())
    );

    if (unitIndex != -1) {
        unitComboBox->setCurrentIndex(unitIndex);
    }

    int supplierIndex = supplierComboBox->findData(
        product->getSupplierId()
    );

    if (supplierIndex != -1) {
        supplierComboBox->setCurrentIndex(supplierIndex);
    }

    int primaryCategoryIndex = primaryCategoryComboBox->findData(
        product->getCategory()
    );

    if (primaryCategoryIndex != -1) {
        primaryCategoryComboBox->setCurrentIndex(primaryCategoryIndex);
    }

    int subCategoryIndex = subCategoryComboBox->findData(
        product->getSubCategory()
    );

    if (subCategoryIndex != -1) {
        subCategoryComboBox->setCurrentIndex(subCategoryIndex);
    }
}

Unit ProductDialog::selectedUnit() const {
    return static_cast<Unit>(
        unitComboBox->currentData().toInt()
    );
}


void ProductDialog::onSaveClicked() {
    if (nameInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Name is required.");
        return;
    }

    if (barcodeInput->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Barcode is required.");
        return;
    }

    if (selectedSupplierId() == -1) {
        QMessageBox::warning(this, "Validation Error", "Please select a valid supplier.");
        return;
    }

    if (selectedPrimaryCategoryId() == -1) {
        QMessageBox::warning(this, "Validation Error", "Please select a valid primary category.");
        return;
    }

    if (selectedSubCategoryId() == -1 &&
        subCategoryComboBox->currentText().trimmed() != "None") {
        QMessageBox::warning(this, "Validation Error", "Please select a valid subcategory.");
        return;
    }

    ServiceResult result = ServiceResult::Failure("Unknown error.");

    if (productId.has_value()) {
        result = inventoryService.editProduct(
            *productId,
            nameInput->text().toStdString(),
            barcodeInput->text().toStdString(),
            selectedUnit(),
            buyingPriceInput->value(),
            sellingPriceInput->value(),
            taxInput->value(),
            heightInput->value(),
            widthInput->value(),
            depthInput->value(),
            selectedPrimaryCategoryId(),
            selectedSupplierId(),
            selectedSubCategoryId()
        );

        if (!result.success) {
            QMessageBox::warning(
                this,
                "Save Product Failed",
                QString::fromStdString(result.message)
            );
            return;
        }

        ServiceResult minResult =
            inventoryService.updateMinimumQuantity(
                *productId,
                minimumQuantityInput->value()
            );

        if (!minResult.success) {
            QMessageBox::warning(
                this,
                "Minimum Quantity Update Failed",
                QString::fromStdString(minResult.message)
            );
            return;
        }
    }
    else {
        result = inventoryService.addProduct(
            nameInput->text().toStdString(),
            barcodeInput->text().toStdString(),
            selectedUnit(),
            buyingPriceInput->value(),
            sellingPriceInput->value(),
            taxInput->value(),
            heightInput->value(),
            widthInput->value(),
            depthInput->value(),
            selectedPrimaryCategoryId(),
            selectedSupplierId(),
            selectedSubCategoryId(),
            minimumQuantityInput->value()
        );

        if (!result.success) {
            QMessageBox::warning(
                this,
                "Save Product Failed",
                QString::fromStdString(result.message)
            );
            return;
        }
    }

    accept();
}

int ProductDialog::selectedSupplierId() const {
    QString typedText = supplierComboBox->currentText().trimmed();

    int index = supplierComboBox->findText(
        typedText,
        Qt::MatchFixedString
    );

    if (index == -1) {
        return -1;
    }

    return supplierComboBox->itemData(index).toInt();
}

int ProductDialog::selectedPrimaryCategoryId() const {
    QString typedText = primaryCategoryComboBox->currentText().trimmed();

    int index = primaryCategoryComboBox->findText(
        typedText,
        Qt::MatchFixedString
    );

    if (index == -1) {
        return -1;
    }

    return primaryCategoryComboBox->itemData(index).toInt();
}

int ProductDialog::selectedSubCategoryId() const {
    QString typedText = subCategoryComboBox->currentText().trimmed();

    int index = subCategoryComboBox->findText(
        typedText,
        Qt::MatchFixedString
    );

    if (index == -1) {
        return -1;
    }

    return subCategoryComboBox->itemData(index).toInt();
}