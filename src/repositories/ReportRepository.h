#pragma once

#include <vector>
#include <string>

#include "database/Database.h"
#include "models/ReportModel.h"

class ReportRepository {
private:
    Database& database;

public:
    ReportRepository(Database& db);

    std::vector<LowStockReportItem> getLowStockProducts();

    InventoryValueReport getInventoryValue();

    std::vector<MovementHistoryItem> getMovementHistory(
        const std::string& startDate,
        const std::string& endDate
    );
};