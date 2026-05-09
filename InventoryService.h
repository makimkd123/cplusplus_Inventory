#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <string>
#include "Database.h"
#include "Product.h"
#include "StockMovement.h"
#include "Supplier.h"
#include "ProductRepository.h"
#include "StockMovementRepository.h"
#include "SupplierRepository.h"
#include "CategoryRepository.h"
#include "Category.h"

class InventoryService {
private:
    ProductRepository productRepository;
    StockMovementRepository stockMovementRepository;
    SupplierRepository supplierRepository;
    CategoryRepository categoryRepository;

public:
    explicit InventoryService(Database& database);

    void showProducts();
    void showProductDetails(int productId);
    void showStockMovements(int productId);

    void addStockMovement(
        int productId,
        MovementType type,
        MovementReason reason,
        double amount
    );

    void addProduct(
        const std::string& name,
        const std::string& barcode,
        Unit unit,
        double buyingPrice,
        double sellingPrice
    );

    void searchProducts(const std::string& keyword);

    void editProduct(
        int productId,
        const std::string& name,
        const std::string& barcode,
        Unit unit,
        double buyingPrice,
        double sellingPrice
    );

    void showLowStockProducts();
    void updateMinimumQuantity(int productId, double minimumQuantity);
    void changeProductStatus(int productId, ProductStatus status);

    void showSuppliers();

    void addSupplier(
        const std::string& name,
        const std::string& contactName,
        const std::string& phone,
        const std::string& email,
        const std::string& address
    );

    void searchSuppliers(const std::string& keyword);

    void deactivateSupplier(int supplierId);

    void showCategories();

    void addCategory(
        const std::wstring& name,
        const std::wstring& description
    );

    void updateCategory(
        int categoryId,
        const std::wstring& name,
        const std::wstring& description,
        bool isActive
    );

    void deactivateCategory(int categoryId);
    
    void updateSupplier(
        int supplierId,
        const std::string& name,
        const std::string& contactName,
        const std::string& phone,
        const std::string& email,
        const std::string& address,
        bool isActive
    );

};

#endif