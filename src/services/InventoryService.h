#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <optional>
#include <string>
#include <vector>

#include "database/Database.h"
#include "models/Product.h"
#include "models/StockMovement.h"
#include "models/Supplier.h"
#include "models/Category.h"
#include "repositories/ProductRepository.h"
#include "repositories/StockMovementRepository.h"
#include "repositories/SupplierRepository.h"
#include "repositories/CategoryRepository.h"
#include "ServiceResult.h"

class InventoryService {
private:
    Database& database;
    ProductRepository productRepository;
    StockMovementRepository stockMovementRepository;
    SupplierRepository supplierRepository;
    CategoryRepository categoryRepository;

public:
    explicit InventoryService(Database& database);

    std::optional<Product> getProductDetails(int productId);
    std::vector<StockMovement> getStockMovements(int productId);

    ServiceResult addStockMovement(
        int productId,
        MovementType type,
        MovementReason reason,
        double amount
    );

    ServiceResult addProduct(
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

    ServiceResult editProduct(
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

    std::vector<Product> searchProducts(const std::string& keyword);
    std::vector<Product> getLowStockProducts();

    ServiceResult updateMinimumQuantity(int productId, double minimumQuantity);
    ServiceResult changeProductStatus(int productId, ProductStatus status);

    std::vector<Supplier> getSuppliers();

    ServiceResult addSupplier(
        const std::string& name,
        const std::string& contactName,
        const std::string& phone,
        const std::string& email,
        const std::string& address
    );

    std::vector<Supplier> searchSuppliers(const std::string& keyword);

    ServiceResult deactivateSupplier(int supplierId);

    std::vector<Category> getCategories();

    ServiceResult addCategory(
        const std::string& name,
        const std::string& description
    );

    ServiceResult updateCategory(
        int categoryId,
        const std::string& name,
        const std::string& description,
        bool isActive
    );

    ServiceResult deactivateCategory(int categoryId);

    ServiceResult updateSupplier(
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