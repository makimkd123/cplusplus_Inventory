#ifndef PRODUCTPAGE_H
#define PRODUCTPAGE_H

#include <QWidget>

#include "services/InventoryService.h"

class QTableWidget;
class QPushButton;
class QLineEdit;

class ProductPage : public QWidget {
    Q_OBJECT

private:
    InventoryService& inventoryService;

    QLineEdit* searchBar;
    QTableWidget* productTable;

    QPushButton* refreshButton;
    QPushButton* detailsButton;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* stockInButton;
    QPushButton* stockOutButton;

    void loadProducts(const QString& keyword = "");
    int selectedProductId() const;
    void updateButtonStates();

private slots:
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onSelectionChanged();

    void onDetailsClicked();
    void onAddClicked();
    void onEditClicked();
    void onStockInClicked();
    void onStockOutClicked();

public:
    explicit ProductPage(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );
};

#endif