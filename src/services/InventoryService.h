#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <string>
#include "database/Database.h"
#include "models/Product.h"
#include "models/StockMovement.h"
#include "models/Supplier.h"
#include "repositories/ProductRepository.h"
#include "repositories/StockMovementRepository.h"
#include "repositories/SupplierRepository.h"
#include "repositories/CategoryRepository.h"
#include "models/Category.h"

class InventoryService {
private:
    ProductRepository productRepository;
    StockMovementRepository stockMovementRepository;
    SupplierRepository supplierRepository;
    CategoryRepository categoryRepository;

public:
    explicit InventoryService(Database& database);

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
        double sellingPrice,
        double tax,
        double height,
        double width,
        double depth,
        int primaryCategoryId,
        int supplierId,
        int subCategoryId
    );

    void editProduct(
        int productId,
        const std::string& name,
        const std::string& barcode,
        Unit unit,
        double buyingPrice,
        double sellingPrice,
        double tax,
        double height,
        double width,
        double depth,
        int primaryCategoryId,
        int supplierId,
        int subCategoryId
    );

    void searchProducts(const std::string& keyword);

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
        const std::string& name,
        const std::string& description
    );

    void updateCategory(
        int categoryId,
        const std::string& name,
        const std::string& description,
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