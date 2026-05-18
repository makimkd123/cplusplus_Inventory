#include "InventoryService.h"
#include "utils/utils.h"
#include <iostream>
#include <exception>


InventoryService::InventoryService(Database& database)
    : productRepository(database),
      stockMovementRepository(database),
      supplierRepository(database),
      categoryRepository(database)
{
}

bool isValidMovementCombination(
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

void InventoryService::showProductDetails(int productId) {
    std::optional<Product> product = productRepository.getProductById(productId);

    if (!product.has_value()) {
        std::cout << "Product not found.\n";
        return;
    }

    Product p = product.value();

    std::cout << "\nProduct Details\n";
    std::cout << "-------------------------\n";
    std::cout << "ID: " << p.getId() << '\n';
    std::cout << "Name: " << p.getName() << '\n';
    std::cout << "Barcode: " << p.getBarcode() << '\n';
    std::cout << "Quantity: " << p.getQuantity() << '\n';
}

void InventoryService::showStockMovements(int productId) {

    std::vector<StockMovement> movements =
        stockMovementRepository.getMovementsByProductId(productId);

    if (movements.empty()) {
        std::cout << "No stock movements found.\n";
        return;
    }

    for (const StockMovement& movement : movements) {

        std::cout
            << "Type: "
            << toStringNarrow(movement.getType())

            << " | Reason: "
            << toStringNarrow(movement.getReason())

            << " | Amount: "
            << movement.getAmount()

            << std::endl;
    }
}

void InventoryService::addStockMovement(
    int productId,
    MovementType type,
    MovementReason reason,
    double amount
) {
    if (!productRepository.productExists(productId)) {
        std::cout << "Product does not exist.\n";
        return;
    }

    if (amount <= 0) {
        std::cout << "Amount must be greater than 0.\n";
        return;
    }

    if (!isValidMovementCombination(type, reason)) {
        std::cout << "Invalid reason for selected movement type.\n";
        return;
    }

    auto quantity = productRepository.getProductQuantity(productId);

    if (!quantity.has_value()) {
        std::cout << "Could not retrieve product quantity.\n";
        return;
    }

    double currentQuantity = *quantity;

    auto status = productRepository.getProductStatus(productId);

    if (!status.has_value()) {
        std::cout << "Could not retrieve product status.\n";
        return;
    }

    if (*status == ProductStatus::BLOCKED) {
        std::cout << "Cannot modify stock for blocked product.\n";
        return;
    }

    if (type == MovementType::STOCK_IN &&
        *status != ProductStatus::ACTIVE) {
        std::cout << "Only ACTIVE products can receive stock.\n";
        return;
    }

    if (type == MovementType::STOCK_OUT &&
        *status != ProductStatus::ACTIVE &&
        *status != ProductStatus::INACTIVE) {
        std::cout << "Only ACTIVE or INACTIVE products can have stock removed.\n";
        return;
    }

    if (type == MovementType::ADJUSTMENT &&
        *status != ProductStatus::ACTIVE) {
        std::cout << "Only ACTIVE products can be adjusted.\n";
        return;
    }

    if (type == MovementType::STOCK_OUT &&
        amount > currentQuantity) {
        std::cout << "Not enough stock available.\n";
        return;
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

    bool quantityUpdated =
        productRepository.updateProductQuantity(
            productId,
            quantityChange
        );

    if (!quantityUpdated) {
        std::cout << "Failed to update product quantity.\n";
        return;
    }

    bool movementInserted =
        stockMovementRepository.insertStockMovement(
            productId,
            type,
            reason,
            amount
        );

    if (!movementInserted) {
        std::cout << "Failed to insert stock movement.\n";
        return;
    }

    std::cout << "Stock movement added successfully.\n";
}

void InventoryService::addProduct(
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
            throw std::invalid_argument("Barcode already exists");
        }

        if (!categoryRepository.categoryExists(primaryCategoryId)) {
            throw std::invalid_argument("Primary category does not exist");
        }

        if (!supplierRepository.supplierExists(supplierId)) {
            throw std::invalid_argument("Supplier does not exist");
        }

        bool supplierIsActive;

        if (!supplierRepository.getSupplierActiveStatus(supplierId, supplierIsActive)) {
            throw std::invalid_argument("Supplier does not exist");
        }

        if (!supplierIsActive) {
            throw std::invalid_argument("Cannot add product with inactive supplier");
        }

        bool primaryCategoryIsActive;

        if (!categoryRepository.getCategoryActiveStatus(primaryCategoryId, primaryCategoryIsActive)) {
            throw std::invalid_argument("Primary category does not exist");
        }

        if (!primaryCategoryIsActive) {
            throw std::invalid_argument("Cannot add product with inactive primary category");
        }

        if (subCategoryId != -1) {
            bool subCategoryIsActive;

            if (!categoryRepository.getCategoryActiveStatus(subCategoryId, subCategoryIsActive)) {
                throw std::invalid_argument("Subcategory does not exist");
            }

            if (!subCategoryIsActive) {
                throw std::invalid_argument("Cannot add product with inactive subcategory");
            }
        }

        if (subCategoryId != -1 && !categoryRepository.categoryExists(subCategoryId)) {
            throw std::invalid_argument("Subcategory does not exist");
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

        std::cout << "Product added successfully.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Failed to add product: " << e.what() << std::endl;
    }
}

void InventoryService::editProduct(
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
            throw std::invalid_argument("Product does not exist");
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
            throw std::invalid_argument("Primary category does not exist");
        }

        if (!supplierRepository.supplierExists(supplierId)) {
            throw std::invalid_argument("Supplier does not exist");
        }

        bool supplierIsActive;

        if (!supplierRepository.getSupplierActiveStatus(supplierId, supplierIsActive)) {
            throw std::invalid_argument("Supplier does not exist");
        }

        if (!supplierIsActive) {
            throw std::invalid_argument("Cannot add product with inactive supplier");
        }

        bool primaryCategoryIsActive;

        if (!categoryRepository.getCategoryActiveStatus(primaryCategoryId, primaryCategoryIsActive)) {
            throw std::invalid_argument("Primary category does not exist");
        }

        if (!primaryCategoryIsActive) {
            throw std::invalid_argument("Cannot add product with inactive primary category");
        }

        if (subCategoryId != -1) {
            bool subCategoryIsActive;

            if (!categoryRepository.getCategoryActiveStatus(subCategoryId, subCategoryIsActive)) {
                throw std::invalid_argument("Subcategory does not exist");
            }

            if (!subCategoryIsActive) {
                throw std::invalid_argument("Cannot add product with inactive subcategory");
            }
        }


        if (subCategoryId != -1 && !categoryRepository.categoryExists(subCategoryId)) {
            throw std::invalid_argument("Subcategory does not exist");
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

        std::cout << "Product updated successfully.\n";
    }
    catch (const std::exception& e) {
        std::cout << "Failed to update product: " << e.what() << std::endl;
    }
}

void InventoryService::searchProducts(const std::string& keyword) {
    std::vector<Product> products = productRepository.searchProduct(keyword);

    if (products.empty()) {
        std::cout << "No products found.\n";
        return;
    }

    for (const Product& product : products) {
        std::cout << "ID: " << product.getId() << '\n';
        std::cout << "Name: " << product.getName() << '\n';
        std::cout << "Barcode: " << product.getBarcode() << '\n';
        std::cout << "Quantity: " << product.getQuantity() << '\n';
        std::cout << "-------------------------\n";
    }
}


void InventoryService::updateMinimumQuantity(int productId,double minimumQuantity){
    bool success = productRepository.updateMinimumQuantity(productId,minimumQuantity);

    if (!success) {
        std::cout << "Failed to update minimum quantity.\n";
        return;
    }
    std::cout << "Minimum quantity updated successfully.\n";
}

void InventoryService::showLowStockProducts() {

    std::vector<Product> products =
        productRepository.getLowStockProducts();

    if (products.empty()) {
        std::cout << "No low stock products found.\n";
        return;
    }

    std::cout << "\nLow Stock Products\n";
    std::cout << "----------------------------------------\n";

    for (const Product& product : products) {

        std::cout << "ID: " << product.getId() << '\n';

        std::cout << "Name: "
                  << product.getName() << '\n';

        std::cout << "Barcode: "
                  << product.getBarcode() << '\n';

        std::cout << "Quantity: "
                  << product.getQuantity() << '\n';

        std::cout << "Minimum Quantity: "
                  << product.getMinimumQuantity() << '\n';

        std::cout << "----------------------------------------\n";
    }
}
void InventoryService::changeProductStatus(
    int productId,
    ProductStatus status
) {
    bool success =
        productRepository.updateProductStatus(
            productId,
            status
        );

    if (!success) {
        std::cout << "Failed to update product status.\n";
        return;
    }

    std::cout << "Product status updated successfully.\n";
}

void InventoryService::showSuppliers() {

    std::vector<Supplier> suppliers =
        supplierRepository.getAllSuppliers();

    if (suppliers.empty()) {
        std::cout << "No suppliers found.\n";
        return;
    }

    std::cout << "\nID | Name | Contact | Phone | Email | Active\n";
    std::cout << "------------------------------------------------------------\n";

    for (const Supplier& supplier : suppliers) {

        std::cout << supplier.getId() << " | "
                  << supplier.getName() << " | "
                  << supplier.getContactName() << " | "
                  << supplier.getPhone() << " | "
                  << supplier.getEmail() << " | "
                  << (supplier.isActive() ? "YES" : "NO")
                  << std::endl;
    }
}

void InventoryService::addSupplier(
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
        std::cout << "Failed to add supplier.\n";
        return;
    }

    std::cout << "Supplier added successfully.\n";
}

void InventoryService::searchSuppliers(const std::string& keyword) {

    std::vector<Supplier> suppliers =
        supplierRepository.searchSuppliers(keyword);

    if (suppliers.empty()) {
        std::cout << "No suppliers found.\n";
        return;
    }

    std::cout << "\nID | Name | Contact | Phone | Email | Active\n";
    std::cout << "------------------------------------------------------------\n";

    for (const Supplier& supplier : suppliers) {

        std::cout << supplier.getId() << " | "
                  << supplier.getName() << " | "
                  << supplier.getContactName() << " | "
                  << supplier.getPhone() << " | "
                  << supplier.getEmail() << " | "
                  << (supplier.isActive() ? "YES" : "NO")
                  << std::endl;
    }
}

void InventoryService::deactivateSupplier(int supplierId) {
    bool isActive;

    if (!supplierRepository.getSupplierActiveStatus(supplierId, isActive)) {
        std::cout << "Supplier not found.\n";
        return;
    }

    if (!isActive) {
        std::cout << "Supplier is already inactive.\n";
        return;
    }

    bool success =
        supplierRepository.deactivateSupplier(supplierId);

    if (!success) {
        std::cout << "Failed to deactivate supplier.\n";
        return;
    }

    std::cout << "Supplier deactivated successfully.\n";
}

void InventoryService::showCategories() {
    std::vector<Category> categories = categoryRepository.getAllCategories();

    if (categories.empty()) {
        std::cout << "No categories found.\n";
        return;
    }

    for (const Category& category : categories) {
        std::cout << "ID: " << category.getCategoryId()
                   << " | Name: " << category.getName()
                   << " | Description: " << category.getDescription()
                   << " | Active: " << (category.getIsActive() ? L"Yes" : L"No")
                   << std::endl;
    }
}

void InventoryService::addCategory(
    const std::string& name,
    const std::string& description
) {
    Category category(0, name, description, true);

    bool success =
        categoryRepository.addCategory(category);

    if (!success) {
        std::cout << "Failed to add category.\n";
        return;
    }

    std::cout << "Category added successfully.\n";
}

void InventoryService::updateCategory(
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
        std::cout << "Failed to update category.\n";
        return;
    }

    std::cout << "Category updated successfully.\n";
}

void InventoryService::deactivateCategory(int categoryId) {

    bool success =
        categoryRepository.deactivateCategory(categoryId);

    if (!success) {
        std::cout << "Failed to deactivate category.\n";
        return;
    }

    std::cout << "Category deactivated successfully.\n";
}

void InventoryService::updateSupplier(
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
        std::cout << "Failed to update supplier.\n";
        return;
    }

    std::cout << "Supplier updated successfully.\n";
}