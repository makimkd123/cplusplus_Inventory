#include "ReportService.h"

ReportService::ReportService(ReportRepository& repo)
    : reportRepository(repo) {}

std::vector<LowStockReportItem> ReportService::getLowStockReport() {
    return reportRepository.getLowStockProducts();
}

InventoryValueReport ReportService::getInventoryValueReport() {
    return reportRepository.getInventoryValue();
}

std::vector<MovementHistoryItem> ReportService::getMovementHistoryReport(
    const std::string& startDate,
    const std::string& endDate
) {
    return reportRepository.getMovementHistory(startDate, endDate);
}