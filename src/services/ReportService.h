#pragma once

#include <string>
#include <vector>

#include "repositories/ReportRepository.h"
#include "models/ReportModel.h"

class ReportService {
private:
    ReportRepository& reportRepository;

public:
    ReportService(ReportRepository& repo);

    std::vector<LowStockReportItem> getLowStockReport();

    InventoryValueReport getInventoryValueReport();

    std::vector<MovementHistoryItem> getMovementHistoryReport(
        const std::string& startDate,
        const std::string& endDate
    );
};