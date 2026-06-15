#include "gui/pages/SupplierPage.h"
#include "gui/dialogs/SupplierDialog.h"


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
#include <QAbstractItemView>
#include <QDialog>
#include <unordered_set>
#include <optional>
#include <vector>

#include "utils/Utils.h"


SupplierPage::SupplierPage(
    InventoryService& inventoryService,
    QWidget* parent
)
    : QWidget(parent),
      inventoryService(inventoryService)
{

    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search suppliers...");

    supplierTable = new QTableWidget(this);
    supplierTable->verticalHeader()->setVisible(false);
    supplierTable->setColumnCount(7);

    supplierTable->setHorizontalHeaderLabels({
        "SupplierID",
        "Name",
        "ContactName",
        "Phone",
        "Email",
        "Address",
        "IsActive"
    });

    supplierTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    supplierTable->setSelectionMode(QAbstractItemView::SingleSelection);
    supplierTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    supplierTable->horizontalHeader()->setStretchLastSection(true);

    refreshButton = new QPushButton("Refresh", this);
    addButton = new QPushButton("Add Supplier", this);
    editButton = new QPushButton("Edit Supplier", this);
    deactivateButton = new QPushButton("Deactive Supplier", this);


    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deactivateButton);    


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(searchBar);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(supplierTable);

    setLayout(mainLayout);

    connect(refreshButton,&QPushButton::clicked,this,&SupplierPage::onRefreshClicked);

    connect(searchBar,&QLineEdit::textChanged,this,&SupplierPage::onSearchTextChanged);

    connect(supplierTable,&QTableWidget::itemSelectionChanged,this,&SupplierPage::onSelectionChanged);

    connect(addButton,&QPushButton::clicked,this,&SupplierPage::onAddClicked);

    connect(editButton,&QPushButton::clicked,this,&SupplierPage::onEditClicked);

    connect(deactivateButton,&QPushButton::clicked,this,&SupplierPage::onDeactivateClicked);


    updateButtonStates();
    loadSuppliers();

}

void SupplierPage::loadSuppliers(const QString& keyword){
    supplierTable->setRowCount(0);

    try{
        std::vector<Supplier> suppliers = inventoryService.searchSuppliers(keyword.toStdString());
        for (const Supplier& supplier : suppliers){
            int row = supplierTable->rowCount();
            supplierTable->insertRow(row);

            supplierTable->setItem(row,0,new QTableWidgetItem(QString::number(supplier.getId())));
            supplierTable->setItem(row,1,new QTableWidgetItem(QString::fromStdString(supplier.getName())));
            supplierTable->setItem(row,2,new QTableWidgetItem(QString::fromStdString(supplier.getContactName())));            
            supplierTable->setItem(row,3,new QTableWidgetItem(QString::fromStdString(supplier.getPhone())));            
            supplierTable->setItem(row,4,new QTableWidgetItem(QString::fromStdString(supplier.getEmail())));
            supplierTable->setItem(row,5,new QTableWidgetItem(QString::fromStdString(supplier.getAddress())));
            supplierTable->setItem(row,6,new QTableWidgetItem(supplier.isActive() ? "ACTIVE" : "INACTIVE"));

        }

        supplierTable->resizeColumnsToContents();
        updateButtonStates();
    }
    catch (const std::exception& e){
        QMessageBox::critical(this,"Load Suppliers Failed",e.what());
    }
};

void SupplierPage::onRefreshClicked(){
    loadSuppliers(searchBar->text());
}

void SupplierPage::onSearchTextChanged(const QString& text){
    loadSuppliers(text);
}

void SupplierPage::onSelectionChanged(){
    updateButtonStates();
}

void SupplierPage::updateButtonStates(){
    bool hasSelection = supplierTable->currentRow() >=0;
    editButton->setEnabled(hasSelection);
    deactivateButton->setEnabled(hasSelection);
}

int SupplierPage::selectedSupplierId()const{
    int row = supplierTable->currentRow();
    if (row<0){
        return -1;
    }

    QTableWidgetItem* idItem = supplierTable->item(row,0);

    if (idItem == nullptr){
        return -1;
    }

    return idItem->text().toInt();
}

void SupplierPage::onAddClicked(){
    SupplierDialog dialog(inventoryService, this);

    if (dialog.exec()==QDialog::Accepted){
        loadSuppliers(searchBar->text());
    }
}

void SupplierPage::onEditClicked(){
    int supplierId = selectedSupplierId();

    if (supplierId == -1){
        QMessageBox::warning(this,"No Product Selected","Please select a product first");
        return;
    }

    SupplierDialog dialog(inventoryService,supplierId,this);

    if (dialog.exec()==QDialog::Accepted){
        loadSuppliers(searchBar->text());
    }
}

void SupplierPage::onDeactivateClicked() {
    int supplierId = selectedSupplierId();

    if (supplierId == -1) {
        QMessageBox::warning(this, "No selection", "Please select a supplier.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deactivation",
        "Are you sure you want to deactivate this supplier?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) {
        return;
    }

    ServiceResult result = inventoryService.deactivateSupplier(supplierId);

    if (result.success) {
        QMessageBox::information(this, "Success", QString::fromStdString(result.message));
        loadSuppliers(searchBar->text());
    } else {
        QMessageBox::warning(this, "Error", QString::fromStdString(result.message));
    }
}
