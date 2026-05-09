#include <iostream>
#include "Database.h"
#include "Utils.h"

Database::Database() : env(NULL), dbc(NULL) {}

Database::~Database() {
    if (dbc) SQLDisconnect(dbc);
    if (dbc) SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    if (env) SQLFreeHandle(SQL_HANDLE_ENV, env);
}

void Database::connect() {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    SQLWCHAR connStr[] =
        L"Driver={ODBC Driver 17 for SQL Server};"
        L"Server=DESKTOP-R7EVUT8;"
        L"Database=Inventory;"
        L"Trusted_Connection=yes;";

    SQLWCHAR outStr[1024];
    SQLSMALLINT outStrLen;

    SQLRETURN ret = SQLDriverConnectW(
        dbc,
        nullptr,
        connStr,
        SQL_NTS,
        outStr,
        sizeof(outStr) / sizeof(SQLWCHAR),
        &outStrLen,
        SQL_DRIVER_COMPLETE
    );

    if (SQL_SUCCEEDED(ret)) {
        std::cout << "Connected to SQL Server!\n";
    } else {
        std::cout << "Connection failed.\n";
    }
}

SQLHDBC Database::getConnection() const {
    return dbc;
}

void Database::executeNonQuery(const std::wstring& query) {
    SQLHSTMT stmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLRETURN ret = SQLExecDirectW(
        stmt,
        (SQLWCHAR*)query.c_str(),
        SQL_NTS
    );

    if (SQL_SUCCEEDED(ret)) {
        std::cout << "Query executed successfully.\n";
    } else {
        std::cout << "Query failed.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void Database::executeQuery(const std::wstring& query){
    SQLHSTMT    stmt = NULL;

    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLRETURN ret = SQLExecDirectW(
        stmt,
        (SQLWCHAR*)query.c_str(),
        SQL_NTS
    );

    if (!SQL_SUCCEEDED(ret)){
        std::cout << "Query failed.\n";
        SQLFreeHandle(SQL_HANDLE_STMT,stmt);
        return;
    }

    SQLINTEGER productId;
    SQLWCHAR name[100];
    SQLWCHAR barcode[50];
    SQLWCHAR unit[20];
    double quantity;

    while(SQLFetch(stmt) == SQL_SUCCESS){
        SQLGetData(stmt, 1, SQL_C_LONG, &productId, 0 , NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, barcode, sizeof(barcode), NULL);
        SQLGetData(stmt, 4, SQL_C_WCHAR, unit, sizeof(unit), NULL);
        SQLGetData(stmt, 5, SQL_C_DOUBLE, &quantity, 0, NULL);
        std::wcout << L"ID: " << productId
                   << L" | Name: " << name
                   << L" | Barcode: " << barcode
                   << L" | Unit: " << unit
                   << L" | Quantity: " << quantity
                   << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT,stmt);
}

void Database::executeStockMovementQuery(const std::wstring& query) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

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

        std::wstring typeStr = movementTypeBuffer;
        std::wstring reasonStr = reasonBuffer;

        MovementType type = toMovementType(typeStr);
        MovementReason reason = toMovementReason(reasonStr);

        std::wcout << toString(type) << L" | "
                   << toString(reason) << L" | "
                   << amount << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
