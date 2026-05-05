#include "InventoryService.h"
#include <iostream>
#include "StockMovement.h"
#include "Utils.h"
#include <string>
#include "Product.h"
#include <exception>
InventoryService::InventoryService(Database& database):db(database){

}

void InventoryService::showProducts(){
    db.executeQuery(
        L"SELECT ProductID, Name, Barcode, Unit, Quantity FROM PRODUCTS;"
    );
}

void InventoryService::showProductDetails(int productId){
    std::wstring query =
        L"SELECT ProductID, Name, Barcode, Unit, Quantity "
        L"FROM Products WHERE ProductID = " + std::to_wstring(productId) + L";";

    db.executeQuery(query);

}

void InventoryService::showStockMovements(int productId){
    std::wstring query =
        L"SELECT MovementType, Reason, Amount "
        L"FROM StockMovements WHERE ProductID = " + std::to_wstring(productId) + L";";

    db.executeStockMovementQuery(query);
}

void InventoryService::addStockMovement(
    int productId,
    MovementType type,
    MovementReason reason,
    double amount
) {
    if (amount <= 0) {
        std::cout << "Amount must be greater than 0.\n";
        return;
    }

    double currentQuantity = db.getProductQuantity(productId);

    if (currentQuantity < 0) {
        std::cout << "Product not found.\n";
        return;
    }

    if (type == MovementType::STOCK_OUT && amount > currentQuantity) {
        std::cout << "Not enough stock available.\n";
        return;
    }

    db.insertStockMovement(productId, type, reason, amount);

    if (type == MovementType::STOCK_IN) {
        db.updateProductQuantity(productId, amount);
    }
    else if (type == MovementType::STOCK_OUT) {
        db.updateProductQuantity(productId, -amount);
    }

    std::cout << "Stock movement added successfully.\n";
}

void InventoryService::addProduct(const std::string& name,
                                  const std::string& barcode,
                                  Unit unit,
                                  double buyingPrice,
                                  double sellingPrice)
{
    try {
        Product product(name, barcode, unit);
        product.setBuyingPrice(buyingPrice);
        product.setSellingPrice(sellingPrice);

        db.insertProduct(
            product.getName(),
            product.getBarcode(),
            product.getUnit(),
            product.getBuyingPrice(),
            product.getSellingPrice()
        );

        std::cout << "Product added successfully.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Failed to add product: " << e.what() << std::endl;
    }
}

void InventoryService::searchProducts(const std::string & keyword){
    db.searchProduct(keyword);    
}

void InventoryService::editProduct(int productId, 
        const std::string& name, 
        const std::string& barcode,
    Unit unit,
    double buyingPrice,
    double sellingPrice){

    try {
        Product product(name, barcode, unit);
        product.setBuyingPrice(buyingPrice);
        product.setSellingPrice(sellingPrice);

        db.updateProduct(
            productId,
            product.getName(),
            product.getBarcode(),
            product.getUnit(),
            product.getBuyingPrice(),
            product.getSellingPrice()
        );

        std::cout << "Product updated successfully.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Failed to update product: " << e.what() << std::endl;
    }


}
    