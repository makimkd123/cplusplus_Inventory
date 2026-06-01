#ifndef CATEGORYDIALOG_H
#define CATEGORYDIALOG_H

#include <QDialog>

#include "services/InventoryService.h"

class QLineEdit;
class QCheckBox;

class CategoryDialog : public QDialog {
    Q_OBJECT

private:
    InventoryService& inventoryService;
    int categoryId;
    bool editMode;

    QLineEdit* nameEdit;
    QLineEdit* descriptionEdit;
    QCheckBox* isActiveCheck;

    void loadCategory();
    void saveCategory();

private slots:
    void onSaveClicked();

public:
    explicit CategoryDialog(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );

    explicit CategoryDialog(
        InventoryService& inventoryService,
        int categoryId,
        QWidget* parent = nullptr
    );
};

#endif