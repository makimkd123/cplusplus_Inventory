#pragma once

#include <string>
#include <vector>

#include "repositories/ReportRepository.h"
#include "repositories/StockMovementRepository.h"
#include "models/ReportModel.h"

class ReportService {
private:
    ReportRepository& reportRepository;
    StockMovementRepository& stockMovementRepository;

public:
    ReportService(ReportRepository& reportRepository,
    StockMovementRepository& stockMovementRepository);

    std::vector<LowStockReportItem> getLowStockReport();

    InventoryValueReport getInventoryValueReport();

    std::vector<MovementHistoryItem> getMovementHistoryReport(
        const std::string& startDate,
        const std::string& endDate
    );

    std::vector<StockMovement> getProductMovementHistory(int productId);
};