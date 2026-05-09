#include "InventoryService.h"

#include <iostream>
#include <exception>

InventoryService::InventoryService(Database& database)
    : productRepository(database),
      stockMovementRepository(database),
      supplierRepository(database),
      categoryRepository(database)
{
}

void InventoryService::showProducts() {
    productRepository.printAllProducts();
}

void InventoryService::showProductDetails(int productId) {
    productRepository.printProductDetails(productId);
}

void InventoryService::showStockMovements(int productId) {
    stockMovementRepository.printMovementsByProductId(productId);
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

    double currentQuantity = productRepository.getProductQuantity(productId);

    if (currentQuantity < 0) {
        std::cout << "Product not found.\n";
        return;
    }

    ProductStatus status = productRepository.getProductStatus(productId);

    if (type == MovementType::STOCK_IN && status != ProductStatus::ACTIVE) {
        std::cout << "Only ACTIVE products can receive stock.\n";
        return;
    }

    if (type == MovementType::STOCK_OUT &&
        status != ProductStatus::ACTIVE &&
        status != ProductStatus::INACTIVE) {
        std::cout << "BLOCKED products cannot be sold.\n";
        return;
    }

    if (type == MovementType::STOCK_OUT && amount > currentQuantity) {
        std::cout << "Not enough stock available.\n";
        return;
    }

    stockMovementRepository.insertStockMovement(productId, type, reason, amount);

    if (type == MovementType::STOCK_IN) {
        productRepository.updateProductQuantity(productId, amount);
    }
    else if (type == MovementType::STOCK_OUT) {
        productRepository.updateProductQuantity(productId, -amount);
    }

    std::cout << "Stock movement added successfully.\n";
}

void InventoryService::addProduct(
    const std::string& name,
    const std::string& barcode,
    Unit unit,
    double buyingPrice,
    double sellingPrice
) {
    try {
        Product product(name, barcode, unit);
        product.setBuyingPrice(buyingPrice);
        product.setSellingPrice(sellingPrice);

        productRepository.insertProduct(
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

void InventoryService::searchProducts(const std::string& keyword) {
    productRepository.searchProduct(keyword);
}

void InventoryService::editProduct(
    int productId,
    const std::string& name,
    const std::string& barcode,
    Unit unit,
    double buyingPrice,
    double sellingPrice
) {
    try {
        Product product(name, barcode, unit);
        product.setBuyingPrice(buyingPrice);
        product.setSellingPrice(sellingPrice);

        productRepository.updateProduct(
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

void InventoryService::updateMinimumQuantity(int productId, double minimumQuantity) {
    productRepository.updateMinimumQuantity(productId, minimumQuantity);
}

void InventoryService::showLowStockProducts() {
    productRepository.printLowStockProducts();
}
void InventoryService::changeProductStatus(int productId, ProductStatus status) {
    productRepository.updateProductStatus(productId, status);
}

void InventoryService::showSuppliers() {
    supplierRepository.printAllSuppliers();
}

void InventoryService::addSupplier(
    const std::string& name,
    const std::string& contactName,
    const std::string& phone,
    const std::string& email,
    const std::string& address
) {
    Supplier supplier(name, contactName, phone, email, address);
    supplierRepository.insertSupplier(supplier);
}

void InventoryService::searchSuppliers(const std::string& keyword) {
    supplierRepository.searchSupplier(keyword);
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

    supplierRepository.deactivateSupplier(supplierId);
    std::cout << "Supplier deactivated successfully.\n";
}

void InventoryService::showCategories() {
    std::vector<Category> categories = categoryRepository.getAllCategories();

    if (categories.empty()) {
        std::cout << "No categories found.\n";
        return;
    }

    for (const Category& category : categories) {
        std::wcout << L"ID: " << category.getCategoryId()
                   << L" | Name: " << category.getName()
                   << L" | Description: " << category.getDescription()
                   << L" | Active: " << (category.getIsActive() ? L"Yes" : L"No")
                   << std::endl;
    }
}

void InventoryService::addCategory(
    const std::wstring& name,
    const std::wstring& description
) {
    Category category(0, name, description, true);
    categoryRepository.addCategory(category);
}

void InventoryService::updateCategory(
    int categoryId,
    const std::wstring& name,
    const std::wstring& description,
    bool isActive
) {
    Category category(categoryId, name, description, isActive);
    categoryRepository.updateCategory(category);
}

void InventoryService::deactivateCategory(int categoryId) {
    categoryRepository.deactivateCategory(categoryId);
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

    supplierRepository.updateSupplier(supplier);
}