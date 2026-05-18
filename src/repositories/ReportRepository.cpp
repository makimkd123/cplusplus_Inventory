#include "ReportRepository.h"

#include <iostream>
#include <sqlext.h>
#include <stdexcept>

ReportRepository::ReportRepository(Database& db)
    : database(db) {}
std::vector<LowStockReportItem>
ReportRepository::getLowStockProducts() {
    std::vector<LowStockReportItem> products;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT ProductID, Name, Quantity, MinQuantity "
        L"FROM Products "
        L"WHERE Quantity <= MinQuantity;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw std::runtime_error("Failed to retrieve low stock report.");
    }

    SQLINTEGER productId;
    SQLWCHAR nameBuffer[100];
    double quantity;
    double minQuantity;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_SLONG, &productId, 0, nullptr);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nameBuffer, sizeof(nameBuffer), nullptr);
        SQLGetData(stmt, 3, SQL_C_DOUBLE, &quantity, 0, nullptr);
        SQLGetData(stmt, 4, SQL_C_DOUBLE, &minQuantity, 0, nullptr);

        std::wstring wName(nameBuffer);
        std::string name(wName.begin(), wName.end());

        LowStockReportItem item;
        item.productId = productId;
        item.name = name;
        item.quantity = quantity;
        item.minQuantity = minQuantity;

        products.push_back(item);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return products;
}
InventoryValueReport ReportRepository::getInventoryValue() {
    InventoryValueReport report;

    report.totalBuyingValue = 0.0;
    report.totalSellingValue = 0.0;
    report.potentialProfit = 0.0;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT "
        L"ISNULL(SUM(Quantity * BuyingPrice), 0), "
        L"ISNULL(SUM(Quantity * SellingPrice), 0), "
        L"ISNULL(SUM(Quantity * SellingPrice), 0) - "
        L"ISNULL(SUM(Quantity * BuyingPrice), 0) "
        L"FROM Products "
        L"WHERE Status != 'BLOCKED';";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw std::runtime_error("Failed to retrieve inventory value report.");
    }

    ret = SQLFetch(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw std::runtime_error("Inventory value report returned no row.");
    }

    SQLGetData(stmt, 1, SQL_C_DOUBLE, &report.totalBuyingValue, 0, nullptr);
    SQLGetData(stmt, 2, SQL_C_DOUBLE, &report.totalSellingValue, 0, nullptr);
    SQLGetData(stmt, 3, SQL_C_DOUBLE, &report.potentialProfit, 0, nullptr);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return report;
}

std::vector<MovementHistoryItem> ReportRepository::getMovementHistory(
    const std::string& startDate,
    const std::string& endDate
) {
    std::vector<MovementHistoryItem> movements;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT "
        L"sm.MovementID, "
        L"sm.ProductID, "
        L"p.Name, "
        L"sm.Amount, "
        L"sm.MovementType, "
        L"sm.Reason, "
        L"CONVERT(VARCHAR(19), sm.CreatedAt, 120) "
        L"FROM StockMovements sm "
        L"JOIN Products p ON sm.ProductID = p.ProductID "
        L"WHERE sm.CreatedAt >= CONVERT(DATETIME, ?, 120) "
        L"AND sm.CreatedAt < DATEADD(day, 1, CONVERT(DATETIME, ?, 120)) "
        L"ORDER BY sm.CreatedAt DESC;";

    SQLPrepareW(stmt, query, SQL_NTS);
    std::wstring wStartDate(startDate.begin(), startDate.end());
    std::wstring wEndDate(endDate.begin(), endDate.end());

    SQLLEN startDateInd = SQL_NTS;
    SQLLEN endDateInd = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     30, 0, (SQLPOINTER)wStartDate.c_str(), 0, &startDateInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     30, 0, (SQLPOINTER)wEndDate.c_str(), 0, &endDateInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw std::runtime_error("Failed to retrieve movement history report.");
    }

    SQLINTEGER movementId;
    SQLINTEGER productId;
    SQLWCHAR productNameBuffer[100];
    double quantity;
    SQLWCHAR movementTypeBuffer[50];
    SQLWCHAR reasonBuffer[50];
    SQLCHAR movementDateBuffer[30];

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_SLONG, &movementId, 0, nullptr);
        SQLGetData(stmt, 2, SQL_C_SLONG, &productId, 0, nullptr);
        SQLGetData(stmt, 3, SQL_C_WCHAR, productNameBuffer, sizeof(productNameBuffer), nullptr);
        SQLGetData(stmt, 4, SQL_C_DOUBLE, &quantity, 0, nullptr);
        SQLGetData(stmt, 5, SQL_C_WCHAR, movementTypeBuffer, sizeof(movementTypeBuffer), nullptr);
        SQLGetData(stmt, 6, SQL_C_WCHAR, reasonBuffer, sizeof(reasonBuffer), nullptr);
        SQLGetData(stmt, 7, SQL_C_CHAR, movementDateBuffer, sizeof(movementDateBuffer), nullptr);

        std::wstring wProductName(productNameBuffer);
        std::wstring wMovementType(movementTypeBuffer);
        std::wstring wReason(reasonBuffer);

        MovementHistoryItem item;

        item.movementId = movementId;
        item.productId = productId;
        item.productName = std::string(wProductName.begin(), wProductName.end());
        item.quantity = quantity;
        item.movementType = std::string(wMovementType.begin(), wMovementType.end());
        item.reason = std::string(wReason.begin(), wReason.end());
        item.movementDate = std::string(reinterpret_cast<char*>(movementDateBuffer));

        movements.push_back(item);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return movements;
}