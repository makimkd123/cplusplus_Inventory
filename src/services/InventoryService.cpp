#include "InventoryService.h"

#include <exception>

InventoryService::InventoryService(Database& database)
    : database(database),
      productRepository(database),
      stockMovementRepository(database),
      supplierRepository(database),
      categoryRepository(database)
{
}

static bool isValidMovementCombination(
    MovementType type,
    MovementReason reason
) {
    if (type == MovementType::STOCK_IN) {
        return reason == MovementReason::BOUGHT ||
               reason == MovementReason::OTHER;
    }

    if (type == MovementType::STOCK_OUT) {
        return reason == MovementReason::SOLD ||
               reason == MovementReason::DAMAGED ||
               reason == MovementReason::OTHER;
    }

    if (type == MovementType::ADJUSTMENT) {
        return reason == MovementReason::OTHER;
    }

    return false;
}

std::optional<Product> InventoryService::getProductDetails(int productId) {
    return productRepository.getProductById(productId);
}

std::vector<StockMovement> InventoryService::getStockMovements(int productId) {
    return stockMovementRepository.getMovementsByProductId(productId);
}

ServiceResult InventoryService::addStockMovement(
    int productId,
    MovementType type,
    MovementReason reason,
    double amount
) {
    try {
        if (!productRepository.productExists(productId)) {
            return ServiceResult::Failure("Product does not exist.");
        }

        if (amount <= 0) {
            return ServiceResult::Failure("Amount must be greater than 0.");
        }

        if (!isValidMovementCombination(type, reason)) {
            return ServiceResult::Failure("Invalid reason for selected movement type.");
        }

        auto quantity = productRepository.getProductQuantity(productId);

        if (!quantity.has_value()) {
            return ServiceResult::Failure("Could not retrieve product quantity.");
        }

        auto status = productRepository.getProductStatus(productId);

        if (!status.has_value()) {
            return ServiceResult::Failure("Could not retrieve product status.");
        }

        if (*status == ProductStatus::BLOCKED) {
            return ServiceResult::Failure("Cannot modify stock for blocked product.");
        }

        if (type == MovementType::STOCK_OUT && amount > *quantity) {
            return ServiceResult::Failure("Not enough stock available.");
        }

        double quantityChange = 0;

        if (type == MovementType::STOCK_IN) {
            quantityChange = amount;
        }
        else if (type == MovementType::STOCK_OUT) {
            quantityChange = -amount;
        }
        else if (type == MovementType::ADJUSTMENT) {
            quantityChange = amount;
        }

        database.beginTransaction();

        bool quantityUpdated =
            productRepository.updateProductQuantity(productId, quantityChange);

        if (!quantityUpdated) {
            database.rollbackTransaction();
            return ServiceResult::Failure("Failed to update product quantity.");
        }

        bool movementInserted =
            stockMovementRepository.insertStockMovement(
                productId,
                type,
                reason,
                amount
            );

        if (!movementInserted) {
            database.rollbackTransaction();
            return ServiceResult::Failure("Failed to insert stock movement.");
        }

        database.commitTransaction();

        return ServiceResult::Success("Stock movement added successfully.");
    }
    catch (const std::exception& e) {
        database.rollbackTransaction();

        return ServiceResult::Failure(
            std::string("Failed to add stock movement: ") + e.what()
        );
    }
}

ServiceResult InventoryService::addProduct(
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
) {
    try {
        Product product(name, barcode, unit);

        product.setBuyingPrice(buyingPrice);
        product.setSellingPrice(sellingPrice);
        product.setTax(tax);
        product.setHeight(height);
        product.setWidth(width);
        product.setDepth(depth);
        product.setCategory(primaryCategoryId);
        product.setSupplierId(supplierId);
        product.setSubCategory(subCategoryId);

        if (productRepository.barcodeExists(barcode)) {
            return ServiceResult::Failure("Barcode already exists.");
        }

        if (!categoryRepository.categoryExists(primaryCategoryId)) {
            return ServiceResult::Failure("Primary category does not exist.");
        }

        if (!supplierRepository.supplierExists(supplierId)) {
            return ServiceResult::Failure("Supplier does not exist.");
        }

        bool supplierIsActive;

        if (!supplierRepository.getSupplierActiveStatus(supplierId, supplierIsActive)) {
            return ServiceResult::Failure("Supplier does not exist.");
        }

        if (!supplierIsActive) {
            return ServiceResult::Failure("Cannot add product with inactive supplier.");
        }

        bool primaryCategoryIsActive;

        if (!categoryRepository.getCategoryActiveStatus(primaryCategoryId, primaryCategoryIsActive)) {
            return ServiceResult::Failure("Primary category does not exist.");
        }

        if (!primaryCategoryIsActive) {
            return ServiceResult::Failure("Cannot add product with inactive primary category.");
        }

        if (subCategoryId != -1) {
            if (!categoryRepository.categoryExists(subCategoryId)) {
                return ServiceResult::Failure("Subcategory does not exist.");
            }

            bool subCategoryIsActive;

            if (!categoryRepository.getCategoryActiveStatus(subCategoryId, subCategoryIsActive)) {
                return ServiceResult::Failure("Subcategory does not exist.");
            }

            if (!subCategoryIsActive) {
                return ServiceResult::Failure("Cannot add product with inactive subcategory.");
            }
        }

        productRepository.insertProduct(
            product.getName(),
            product.getBarcode(),
            product.getUnit(),
            product.getBuyingPrice(),
            product.getSellingPrice(),
            product.getTax(),
            product.getHeight(),
            product.getWidth(),
            product.getDepth(),
            product.getCategory(),
            product.getSupplierId(),
            product.getSubCategory()
        );

        return ServiceResult::Success("Product added successfully.");
    }
    catch (const std::exception& e) {
        return ServiceResult::Failure(
            std::string("Failed to add product: ") + e.what()
        );
    }
}

ServiceResult InventoryService::editProduct(
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
) {
    try {
        if (!productRepository.productExists(productId)) {
            return ServiceResult::Failure("Product does not exist.");
        }

        Product product(name, barcode, unit);

        product.setBuyingPrice(buyingPrice);
        product.setSellingPrice(sellingPrice);
        product.setTax(tax);
        product.setHeight(height);
        product.setWidth(width);
        product.setDepth(depth);
        product.setCategory(primaryCategoryId);
        product.setSupplierId(supplierId);
        product.setSubCategory(subCategoryId);

        if (!categoryRepository.categoryExists(primaryCategoryId)) {
            return ServiceResult::Failure("Primary category does not exist.");
        }

        if (!supplierRepository.supplierExists(supplierId)) {
            return ServiceResult::Failure("Supplier does not exist.");
        }

        bool supplierIsActive;

        if (!supplierRepository.getSupplierActiveStatus(supplierId, supplierIsActive)) {
            return ServiceResult::Failure("Supplier does not exist.");
        }

        if (!supplierIsActive) {
            return ServiceResult::Failure("Cannot update product with inactive supplier.");
        }

        bool primaryCategoryIsActive;

        if (!categoryRepository.getCategoryActiveStatus(primaryCategoryId, primaryCategoryIsActive)) {
            return ServiceResult::Failure("Primary category does not exist.");
        }

        if (!primaryCategoryIsActive) {
            return ServiceResult::Failure("Cannot update product with inactive primary category.");
        }

        if (subCategoryId != -1) {
            if (!categoryRepository.categoryExists(subCategoryId)) {
                return ServiceResult::Failure("Subcategory does not exist.");
            }

            bool subCategoryIsActive;

            if (!categoryRepository.getCategoryActiveStatus(subCategoryId, subCategoryIsActive)) {
                return ServiceResult::Failure("Subcategory does not exist.");
            }

            if (!subCategoryIsActive) {
                return ServiceResult::Failure("Cannot update product with inactive subcategory.");
            }
        }

        productRepository.updateProduct(
            productId,
            product.getName(),
            product.getBarcode(),
            product.getUnit(),
            product.getBuyingPrice(),
            product.getSellingPrice(),
            product.getTax(),
            product.getHeight(),
            product.getWidth(),
            product.getDepth(),
            product.getCategory(),
            product.getSupplierId(),
            product.getSubCategory()
        );

        return ServiceResult::Success("Product updated successfully.");
    }
    catch (const std::exception& e) {
        return ServiceResult::Failure(
            std::string("Failed to update product: ") + e.what()
        );
    }
}

std::vector<Product> InventoryService::searchProducts(const std::string& keyword) {
    return productRepository.searchProduct(keyword);
}

std::vector<Product> InventoryService::getLowStockProducts() {
    return productRepository.getLowStockProducts();
}

ServiceResult InventoryService::updateMinimumQuantity(
    int productId,
    double minimumQuantity
) {
    bool success =
        productRepository.updateMinimumQuantity(
            productId,
            minimumQuantity
        );

    if (!success) {
        return ServiceResult::Failure("Failed to update minimum quantity.");
    }

    return ServiceResult::Success("Minimum quantity updated successfully.");
}

ServiceResult InventoryService::changeProductStatus(
    int productId,
    ProductStatus status
) {
    bool success =
        productRepository.updateProductStatus(
            productId,
            status
        );

    if (!success) {
        return ServiceResult::Failure("Failed to update product status.");
    }

    return ServiceResult::Success("Product status updated successfully.");
}

std::vector<Supplier> InventoryService::getSuppliers() {
    return supplierRepository.getAllSuppliers();
}

ServiceResult InventoryService::addSupplier(
    const std::string& name,
    const std::string& contactName,
    const std::string& phone,
    const std::string& email,
    const std::string& address
) {
    Supplier supplier(
        name,
        contactName,
        phone,
        email,
        address
    );

    bool success =
        supplierRepository.insertSupplier(supplier);

    if (!success) {
        return ServiceResult::Failure("Failed to add supplier.");
    }

    return ServiceResult::Success("Supplier added successfully.");
}

std::vector<Supplier> InventoryService::searchSuppliers(const std::string& keyword) {
    return supplierRepository.searchSuppliers(keyword);
}

ServiceResult InventoryService::deactivateSupplier(int supplierId) {
    bool isActive;

    if (!supplierRepository.getSupplierActiveStatus(supplierId, isActive)) {
        return ServiceResult::Failure("Supplier not found.");
    }

    if (!isActive) {
        return ServiceResult::Failure("Supplier is already inactive.");
    }

    bool success =
        supplierRepository.deactivateSupplier(supplierId);

    if (!success) {
        return ServiceResult::Failure("Failed to deactivate supplier.");
    }

    return ServiceResult::Success("Supplier deactivated successfully.");
}

std::vector<Category> InventoryService::getCategories() {
    return categoryRepository.getAllCategories();
}

ServiceResult InventoryService::addCategory(
    const std::string& name,
    const std::string& description
) {
    Category category(0, name, description, true);

    bool success =
        categoryRepository.addCategory(category);

    if (!success) {
        return ServiceResult::Failure("Failed to add category.");
    }

    return ServiceResult::Success("Category added successfully.");
}

ServiceResult InventoryService::updateCategory(
    int categoryId,
    const std::string& name,
    const std::string& description,
    bool isActive
) {
    Category category(
        categoryId,
        name,
        description,
        isActive
    );

    bool success =
        categoryRepository.updateCategory(category);

    if (!success) {
        return ServiceResult::Failure("Failed to update category.");
    }

    return ServiceResult::Success("Category updated successfully.");
}

ServiceResult InventoryService::deactivateCategory(int categoryId) {
    bool success =
        categoryRepository.deactivateCategory(categoryId);

    if (!success) {
        return ServiceResult::Failure("Failed to deactivate category.");
    }

    return ServiceResult::Success("Category deactivated successfully.");
}

ServiceResult InventoryService::updateSupplier(
    int supplierId,
    const std::string& name,
    const std::string& contactName,
    const std::string& phone,
    const std::string& email,
    const std::string& address,
    bool isActive
) {
    Supplier supplier(
        supplierId,
        name,
        contactName,
        phone,
        email,
        address,
        isActive
    );

    bool success =
        supplierRepository.updateSupplier(supplier);

    if (!success) {
        return ServiceResult::Failure("Failed to update supplier.");
    }

    return ServiceResult::Success("Supplier updated successfully.");
}