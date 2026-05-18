#ifndef STOCK_MOVEMENT_REPOSITORY_H
#define STOCK_MOVEMENT_REPOSITORY_H

#include <string>
#include <vector>
#include "database/Database.h"
#include "models/StockMovement.h"


class StockMovementRepository {
private:
    Database& database;

public:
    explicit StockMovementRepository(Database& database);

    bool insertStockMovement(int productId,
                             MovementType type,
                             MovementReason reason,
                             double amount);

    std::vector<StockMovement> getMovementsByProductId(int productId);
};

#endif
