#ifndef CATEGORYPAGE_H
#define CATEGORYPAGE_H

#include <QWidget>

#include "services/InventoryService.h"

class QTableWidget;
class QPushButton;
class QLineEdit;

class CategoryPage : public QWidget {
    Q_OBJECT

private:
    InventoryService& inventoryService;

    QLineEdit* searchBar;
    QTableWidget* categoryTable;

    QPushButton* refreshButton;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deactivateButton;

    void loadCategories(const QString& keyword = "");
    int selectedCategoryId() const;
    void updateButtonStates();

private slots:
    void onRefreshClicked();
    void onSearchTextChanged(const QString& text);
    void onSelectionChanged();

    void onAddClicked();
    void onEditClicked();
    void onDeactivateClicked();

public:
    explicit CategoryPage(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );
};

#endif