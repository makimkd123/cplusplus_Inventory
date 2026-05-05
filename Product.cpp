#include "Product.h"
#include <stdexcept>

Product::Product()
    : id(0),
      quantity(0.0),
      minQuantity(0.0),
      unit(Unit::PIECES),
      category(""),
      subCategory(""),
      buyingPrice(0.0),
      sellingPrice(0.0),
      tax(0.0),
      height(0.0),
      width(0.0),
      depth(0.0),
      name(""),
      barcode(""),
      supplierId(0),
      internalCode(0)
{
}


Product::Product(const std::string& name,
                 const std::string& barcode,
                 Unit unit)
    : Product()
{
    if(name.empty()) {
        throw std::invalid_argument("Product name cannot be empty");
    }

    if(barcode.empty()) {
        throw std::invalid_argument("Barcode cannot be empty");
    }

    this->name = name;
    this->barcode = barcode;
    this->unit=unit;
}

std::string Product::getName() const {
    return name;
}

std::string Product::getBarcode() const {
    return barcode;
}

double Product::getQuantity() const {
    return quantity;
}

Unit Product::getUnit() const {
    return unit;
}

double Product::getBuyingPrice() const {
    return buyingPrice;
}

double Product::getSellingPrice() const {
    return sellingPrice;
}

const std::vector<StockMovement>& Product::getMovements() const {
    return movements;
}

void Product::addStock(double amount) {
    if(amount <= 0.0) {
        throw std::invalid_argument("Amount must be positive");
    }

    quantity += amount;
    movements.emplace_back(amount,MovementType::STOCK_IN, MovementReason::BOUGHT);
}

void Product::removeStock(double amount) {
    if(amount <= 0.0) {
        throw std::invalid_argument("Amount must be positive");
    }

    if(amount > quantity) {
        throw std::invalid_argument("Amount is greater than available quantity");
    }

    quantity -= amount;
    movements.emplace_back(amount, MovementType::STOCK_OUT, MovementReason::SOLD);
}

void Product::setBuyingPrice(double price) {
    if(price <= 0.0) {
        throw std::invalid_argument("Buying price must be positive");
    }

    buyingPrice = price;
}

void Product::setSellingPrice(double price) {
    if(price <= 0.0) {
        throw std::invalid_argument("Selling price must be positive");
    }

    sellingPrice = price;
}

bool Product::isLowStock() const {
    return quantity <= minQuantity;
}

void Product::setMinimumQuantity(double amount) {
    if(amount < 0.0) {
        throw std::invalid_argument("Minimum Quantity cannot be negative");
    }

    minQuantity = amount;
}