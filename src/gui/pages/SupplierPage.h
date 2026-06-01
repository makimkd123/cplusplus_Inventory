#ifndef SUPPLIERPAGE_H
#define SUPPLIERPAGE_H


#include <QWidget>

#include "services/InventoryService.h"

class QTableWidget;
class QPushButton;
class QLineEdit;


class SupplierPage : public QWidget {
    Q_OBJECT

private:
    InventoryService& inventoryService;

    QLineEdit* searchBar;
    QTableWidget* supplierTable;

    QPushButton* refreshButton;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deactivateButton;

    void loadSuppliers(const QString& keyword = "");
    int selectedSupplierId() const;
    void updateButtonStates();

private slots:
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onSelectionChanged();

    void onAddClicked();
    void onEditClicked();
    void onDeactivateClicked();


public:
    explicit SupplierPage(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );
};

#endif