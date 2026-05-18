#pragma once

#include <string>

#include "repositories/ReportRepository.h"

class ReportService {
private:
    ReportRepository& reportRepository;

public:
    ReportService(ReportRepository& repo);

    void showLowStockReport();

    void showInventoryValueReport();

    void showMovementHistoryReport(
        const std::string& startDate,
        const std::string& endDate
    );
};