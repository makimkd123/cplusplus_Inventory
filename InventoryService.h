#ifndef INVENTORYSERVICE_H
#define INVENTORYSERVICE_H

#include "Database.h"
#include "StockMovement.h"
#include "Product.h"
class InventoryService {
    private:
        Database& db;

    public:
    InventoryService(Database& database);

    void showProducts();
    void showProductDetails(int productId);
    void showStockMovements(int productId);
    void addStockMovement(
        int productId,
        MovementType type,
        MovementReason reason,
        double amount);
    void addProduct(const std::string& name,
        const std::string& barcode,
        Unit unit,
        double buyingPrice,
        double sellingPrice);
    void searchProducts(const std::string & keyword);
    void editProduct(int productId, 
        const std::string& name, 
        const std::string& barcode,
    Unit unit,
    double buyingPrice,
    double sellingPrice);

};


#endif