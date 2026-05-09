#ifndef PRODUCT_REPOSITORY_H
#define PRODUCT_REPOSITORY_H

#include <string>
#include <vector>
#include "Database.h"
#include "Product.h"

class ProductRepository {
private:
    Database& database;

public:
    explicit ProductRepository(Database& database);

    void insertProduct(const std::string& name,
                       const std::string& barcode,
                       Unit unit,
                       double buyingPrice,
                       double sellingPrice);

    void searchProduct(const std::string& keyword);

    void updateProduct(int productId,
                       const std::string& name,
                       const std::string& barcode,
                       Unit unit,
                       double buyingPrice,
                       double sellingPrice);

    double getProductQuantity(int productId);

    void updateProductQuantity(int productId, double quantityChange);
    void printAllProducts();
    void printProductDetails(int productId);
    double getMinimumQuantity(int productId);
    ProductStatus getProductStatus(int productId);
    void updateMinimumQuantity(int productId, double minimumQuantity);
    void updateProductStatus(int productId, ProductStatus status);
    void printLowStockProducts();

};

#endif
