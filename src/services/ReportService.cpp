#include "ReportService.h"

ReportService::ReportService(
    ReportRepository& reportRepository,
    StockMovementRepository& stockMovementRepository
)
    : reportRepository(reportRepository),
      stockMovementRepository(stockMovementRepository)
{
}

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

std::vector<StockMovement> ReportService::getProductMovementHistory(int productId) {
    return stockMovementRepository.getMovementsByProductId(productId);
}