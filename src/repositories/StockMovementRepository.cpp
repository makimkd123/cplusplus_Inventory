#include <iostream>
#include "StockMovementRepository.h"
#include "utils/Utils.h"
#include <vector>

StockMovementRepository::StockMovementRepository(Database& database)
    : database(database) {}

bool StockMovementRepository::insertStockMovement(int productId,
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

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

std::vector<StockMovement>
StockMovementRepository::getMovementsByProductId(int productId) {

    std::vector<StockMovement> movements;

    SQLHSTMT stmt = NULL;

    SQLAllocHandle(
        SQL_HANDLE_STMT,
        database.getConnection(),
        &stmt
    );

    SQLWCHAR query[] =
        L"SELECT MovementType, Reason, Amount "
        L"FROM StockMovements "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_LONG,
        SQL_INTEGER,
        0,
        0,
        &productId,
        0,
        NULL
    );

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return movements;
    }

    SQLWCHAR movementTypeBuffer[50];
    SQLWCHAR reasonBuffer[50];
    double amount;

    while (SQLFetch(stmt) == SQL_SUCCESS) {

        SQLGetData(
            stmt,
            1,
            SQL_C_WCHAR,
            movementTypeBuffer,
            sizeof(movementTypeBuffer),
            NULL
        );

        SQLGetData(
            stmt,
            2,
            SQL_C_WCHAR,
            reasonBuffer,
            sizeof(reasonBuffer),
            NULL
        );

        SQLGetData(
            stmt,
            3,
            SQL_C_DOUBLE,
            &amount,
            0,
            NULL
        );

        StockMovement movement(
            amount,
            toMovementType(movementTypeBuffer),
            toMovementReason(reasonBuffer)
        );

        movements.push_back(movement);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return movements;
}