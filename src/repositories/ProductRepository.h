#ifndef PRODUCT_REPOSITORY_H
#define PRODUCT_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "database/Database.h"
#include "models/Product.h"

class ProductRepository {
private:
    Database& database;

public:
    explicit ProductRepository(Database& database);

    void insertProduct(const std::string& name,
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
                                        int subCategoryId);


    std::vector<Product> searchProduct(const std::string& keyword);

    void updateProduct(int productId,
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
                                        int subCategoryId);

    std::optional<double> getProductQuantity(int productId);

    bool updateProductQuantity(int productId, double quantityChange);

    std::optional<Product> getProductById(int productId);

    double getMinimumQuantity(int productId);


    std::optional<ProductStatus> getProductStatus(int productId);
    bool updateMinimumQuantity(int productId, double minimumQuantity);
    bool updateProductStatus(int productId, ProductStatus status);
    std::vector<Product> getLowStockProducts();


    bool barcodeExists(const std::string& barcode);

    bool productExists(int productId);

};

#endif
