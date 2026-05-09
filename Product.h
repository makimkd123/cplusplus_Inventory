#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <vector>
#include "StockMovement.h"



enum class ProductStatus {
    ACTIVE,
    BLOCKED,
    INACTIVE
};

enum class Unit{
    KG,
    LITER,
    PIECES
};

class Product {
private:
    int id;

    double quantity;
    double minQuantity;
    Unit unit;
    std::string category;
    std::string subCategory;

    double buyingPrice;
    double sellingPrice;
    double tax;

    double height;
    double width;
    double depth;

    std::string name;
    std::string barcode;
    int supplierId;
    int internalCode;
    ProductStatus status;
    std::vector<StockMovement> movements;

public:
    Product();
    Product(const std::string& name,
        const std::string& barcode,
        Unit unit);

    int getId() const;
    std::string getName() const;
    std::string getBarcode() const;
    double getQuantity() const;
    Unit getUnit() const;
    double getBuyingPrice() const;
    double getSellingPrice() const;
    double getMinimumQuantity() const;
    double getTax() const;
    std::string getCategory() const;
    std::string getSubCategory() const;
    int getSupplierId() const;
    int getInternalCode() const;
    ProductStatus getStatus() const;

    const std::vector<StockMovement>& getMovements() const;

    void setId(int id);
    void setQuantity(double quantity);
    void setBuyingPrice(double price);
    void setSellingPrice(double price);
    void setMinimumQuantity(double amount);
    void setUnit(Unit unit);
    void setTax(double tax);
    void setCategory(const std::string& category);
    void setSubCategory(const std::string& subCategory);
    void setSupplierId(int supplierId);
    void setInternalCode(int internalCode);
    void setStatus(ProductStatus status);


    void addStock(double amount);
    void removeStock(double amount);
    bool isLowStock() const;
    bool canStockIn() const;
    bool canStockOut() const;
};

#endif