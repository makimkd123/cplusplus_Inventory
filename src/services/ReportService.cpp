#include "ReportService.h"

#include <iostream>
#include <iomanip>

ReportService::ReportService(ReportRepository& repo)
    : reportRepository(repo) {}

void ReportService::showLowStockReport() {
    std::vector<LowStockReportItem> items =
        reportRepository.getLowStockProducts();

    std::cout << "\n=== LOW STOCK REPORT ===\n";

    if (items.empty()) {
        std::cout << "No low stock products found.\n";
        return;
    }

    for (const auto& item : items) {
        std::cout << "Product ID: " << item.productId << '\n';
        std::cout << "Name: " << item.name << '\n';
        std::cout << "Quantity: " << item.quantity << '\n';
        std::cout << "Minimum Quantity: " << item.minQuantity << '\n';
        std::cout << "-------------------------\n";
    }
}

void ReportService::showInventoryValueReport() {
    InventoryValueReport report =
        reportRepository.getInventoryValue();

    std::cout << "\n=== INVENTORY VALUE REPORT ===\n";

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Total buying value: "
              << report.totalBuyingValue << '\n';

    std::cout << "Total selling value: "
              << report.totalSellingValue << '\n';

    std::cout << "Potential profit: "
              << report.potentialProfit << '\n';
}

void ReportService::showMovementHistoryReport(
    const std::string& startDate,
    const std::string& endDate
) {
    try {
        auto movements =
            reportRepository.getMovementHistory(startDate, endDate);

        if (movements.empty()) {
            std::cout << "No movement history found for this period.\n";
            return;
        }

            for (const auto& item : movements) {
        std::cout << "Movement ID: " << item.movementId << '\n';
        std::cout << "Product ID: " << item.productId << '\n';
        std::cout << "Product Name: " << item.productName << '\n';
        std::cout << "Amount: " << item.quantity << '\n';
        std::cout << "Movement Type: " << item.movementType << '\n';
        std::cout << "Reason: " << item.reason << '\n';
        std::cout << "Date: " << item.movementDate << '\n';
        std::cout << "-------------------------\n";
    }
    }
    catch (const std::exception& ex) {
        std::cout << "Could not retrieve movement history report: "
                  << ex.what() << '\n';
    }
}