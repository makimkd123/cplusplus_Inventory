#pragma once

#include <string>

struct LowStockReportItem {
    int productId;
    std::string name;
    double quantity;
    double minQuantity;
};

struct InventoryValueReport {
    double totalBuyingValue;
    double totalSellingValue;
    double potentialProfit;
};

struct MovementHistoryItem {
    int movementId;
    int productId;
    std::string productName;
    double quantity;
    std::string movementType;
    std::string reason;
    std::string movementDate;
};