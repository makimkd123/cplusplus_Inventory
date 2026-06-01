#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include <optional>

#include "services/InventoryService.h"

class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class QPushButton;

class ProductDialog : public QDialog {
    Q_OBJECT

private:
    InventoryService& inventoryService;
    std::optional<int> productId;

    QLineEdit* nameInput;
    QLineEdit* barcodeInput;

    QComboBox* unitComboBox;
    QComboBox* supplierComboBox;
    QComboBox* primaryCategoryComboBox;
    QComboBox* subCategoryComboBox;

    QDoubleSpinBox* buyingPriceInput;
    QDoubleSpinBox* sellingPriceInput;
    QDoubleSpinBox* taxInput;

    QDoubleSpinBox* heightInput;
    QDoubleSpinBox* widthInput;
    QDoubleSpinBox* depthInput;

    QDoubleSpinBox* minimumQuantityInput;

    QPushButton* saveButton;
    QPushButton* cancelButton;

    void setupUi();
    void loadSuppliers();
    void loadCategories();
    void loadProductData();

    Unit selectedUnit() const;
    int selectedSupplierId() const;
    int selectedPrimaryCategoryId() const;
    int selectedSubCategoryId() const;

private slots:
    void onSaveClicked();

public:
    explicit ProductDialog(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );

    explicit ProductDialog(
        InventoryService& inventoryService,
        int productId,
        QWidget* parent = nullptr
    );
};

#endif