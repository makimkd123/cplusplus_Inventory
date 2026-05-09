#ifndef STOCK_MOVEMENT_REPOSITORY_H
#define STOCK_MOVEMENT_REPOSITORY_H

#include <string>
#include "Database.h"
#include "StockMovement.h"

class StockMovementRepository {
private:
    Database& database;

public:
    explicit StockMovementRepository(Database& database);

    void insertStockMovement(int productId,
                             MovementType type,
                             MovementReason reason,
                             double amount);

    void printStockMovements(const std::wstring& query);
    void printMovementsByProductId(int productId);
};

#endif
