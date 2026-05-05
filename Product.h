#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <vector>
#include "StockMovement.h"




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

    std::vector<StockMovement> movements;

public:
    Product();
    Product(const std::string& name,
        const std::string& barcode,
        Unit unit);

    std::string getName() const;
    std::string getBarcode() const;
    double getQuantity() const;
    Unit getUnit() const;
    double getBuyingPrice() const;
    double getSellingPrice() const;
    const std::vector<StockMovement>& getMovements() const;

    void addStock(double amount);
    void removeStock(double amount);
    void setBuyingPrice(double price);
    void setSellingPrice(double price);
    bool isLowStock() const;
    void setMinimumQuantity(double amount);
    void setUnit (Unit unit);
};

#endif