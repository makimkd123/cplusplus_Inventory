#include <iostream>
#include "StockMovementRepository.h"
#include "Utils.h"

StockMovementRepository::StockMovementRepository(Database& database)
    : database(database) {}

void StockMovementRepository::insertStockMovement(int productId,
                                                  MovementType type,
                                                  MovementReason reason,
                                                  double amount) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"INSERT INTO StockMovements "
        L"(ProductID, MovementType, Reason, Amount) "
        L"VALUES (?, ?, ?, ?);";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::wstring typeStr = toString(type);
    std::wstring reasonStr = toString(reason);

    SQLLEN productIdInd = 0;
    SQLLEN typeInd = SQL_NTS;
    SQLLEN reasonInd = SQL_NTS;
    SQLLEN amountInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &productId, 0, &productIdInd);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     20, 0, (SQLPOINTER)typeStr.c_str(), 0, &typeInd);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     50, 0, (SQLPOINTER)reasonStr.c_str(), 0, &reasonInd);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &amount, 0, &amountInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to insert stock movement.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void StockMovementRepository::printStockMovements(const std::wstring& query) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Query failed.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLWCHAR movementTypeBuffer[50];
    SQLWCHAR reasonBuffer[50];
    double amount;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, movementTypeBuffer, sizeof(movementTypeBuffer), NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, reasonBuffer, sizeof(reasonBuffer), NULL);
        SQLGetData(stmt, 3, SQL_C_DOUBLE, &amount, 0, NULL);

        MovementType type = toMovementType(std::wstring(movementTypeBuffer));
        MovementReason reason = toMovementReason(std::wstring(reasonBuffer));

        std::wcout << toString(type) << L" | "
                   << toString(reason) << L" | "
                   << amount << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
void StockMovementRepository::printMovementsByProductId(int productId) {
    std::wstring query =
        L"SELECT MovementType, Reason, Amount "
        L"FROM StockMovements WHERE ProductID = " + std::to_wstring(productId) + L";";

    database.executeStockMovementQuery(query);
}