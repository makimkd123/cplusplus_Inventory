// SupplierDialog.h
#ifndef SUPPLIERDIALOG_H
#define SUPPLIERDIALOG_H

#include <QDialog>
#include "services/InventoryService.h"

class QLineEdit;
class QCheckBox;

class SupplierDialog : public QDialog {
    Q_OBJECT

private:
    InventoryService& inventoryService;
    int supplierId;
    bool editMode;

    QLineEdit* nameEdit;
    QLineEdit* contactNameEdit;
    QLineEdit* phoneEdit;
    QLineEdit* emailEdit;
    QLineEdit* addressEdit;
    QCheckBox* isActiveCheck;

    void loadSupplier();
    void saveSupplier();

private slots:
    void onSaveClicked();

public:
    explicit SupplierDialog(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );

    explicit SupplierDialog(
        InventoryService& inventoryService,
        int supplierId,
        QWidget* parent = nullptr
    );
};

#endif