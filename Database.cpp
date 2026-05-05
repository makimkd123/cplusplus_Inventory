#include <iostream>
#include "Database.h"
#include "Utils.h"

Database::Database() : env(NULL), dbc(NULL){}
Database::~Database(){
    if (dbc) SQLDisconnect(dbc);
    if (dbc) SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    if (env) SQLFreeHandle(SQL_HANDLE_ENV, env);
}

void Database::connect(){
    SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE, &env);
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
void Database::executeNonQuery(const std::wstring& query){
    SQLHSTMT stmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLRETURN ret = SQLExecDirectW(
        stmt,
        (SQLWCHAR*)query.c_str(),
        SQL_NTS
    );

    if (SQL_SUCCEEDED(ret)){
        std::cout << "Query executed successfully.\n";
    } else {
        std::cout << "Query failed. \n";
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
double Database::getProductQuantity(int productId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query =
        L"SELECT Quantity FROM Products WHERE ProductID = " +
        std::to_wstring(productId) + L";";

    SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return -1;
    }

    double quantity = -1;

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_DOUBLE, &quantity, 0, NULL);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return quantity;
}

void Database::insertStockMovement(
    int productId,
    MovementType type,
    MovementReason reason,
    double amount
) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

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

    SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_SLONG,
        SQL_INTEGER,
        0,
        0,
        &productId,
        0,
        &productIdInd
    );

    SQLBindParameter(
        stmt,
        2,
        SQL_PARAM_INPUT,
        SQL_C_WCHAR,
        SQL_WVARCHAR,
        20,
        0,
        (SQLPOINTER)typeStr.c_str(),
        0,
        &typeInd
    );

    SQLBindParameter(
        stmt,
        3,
        SQL_PARAM_INPUT,
        SQL_C_WCHAR,
        SQL_WVARCHAR,
        50,
        0,
        (SQLPOINTER)reasonStr.c_str(),
        0,
        &reasonInd
    );

    SQLBindParameter(
        stmt,
        4,
        SQL_PARAM_INPUT,
        SQL_C_DOUBLE,
        SQL_DOUBLE,
        0,
        0,
        &amount,
        0,
        &amountInd
    );

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to insert stock movement.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void Database::updateProductQuantity(int productId, double quantityChange) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET Quantity = Quantity + ? "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN quantityInd = 0;
    SQLLEN productIdInd = 0;

    SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_DOUBLE,
        SQL_DOUBLE,
        0,
        0,
        &quantityChange,
        0,
        &quantityInd
    );

    SQLBindParameter(
        stmt,
        2,
        SQL_PARAM_INPUT,
        SQL_C_SLONG,
        SQL_INTEGER,
        0,
        0,
        &productId,
        0,
        &productIdInd
    );

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to update product quantity.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


void Database::insertProduct(const std::string& name,
                             const std::string& barcode,
                             Unit unit,
                             double buyingPrice,
                             double sellingPrice)
{
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLWCHAR query[] =
        L"INSERT INTO Products (Name, Barcode, Unit, BuyingPrice, SellingPrice) "
        L"VALUES (?, ?, ?, ?, ?);";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::wstring wName(name.begin(), name.end());
    std::wstring wBarcode(barcode.begin(), barcode.end());
    std::string unitStr = toString(unit);
    std::wstring wUnit(unitStr.begin(), unitStr.end());

    SQLLEN nameInd = SQL_NTS;
    SQLLEN barcodeInd = SQL_NTS;
    SQLLEN unitInd = SQL_NTS;
    SQLLEN buyingInd = 0;
    SQLLEN sellingInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wName.c_str(), 0, &nameInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0,
                     (SQLPOINTER)wBarcode.c_str(), 0, &barcodeInd);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 20, 0,
                     (SQLPOINTER)wUnit.c_str(), 0, &unitInd);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0,
                     &buyingPrice, 0, &buyingInd);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0,
                     &sellingPrice, 0, &sellingInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to add product.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void Database::searchProduct(const std::string& keyword) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLWCHAR query[] =
        L"SELECT ProductID, Name, Barcode, Unit, Quantity "
        L"FROM Products "
        L"WHERE Barcode = ? "
        L"OR Name = ? "
        L"OR Name LIKE ? "
        L"ORDER BY "
        L"CASE "
        L"WHEN Barcode = ? THEN 1 "
        L"WHEN Name = ? THEN 2 "
        L"WHEN Name LIKE ? THEN 3 "
        L"ELSE 4 END;";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::wstring wKeyword(keyword.begin(), keyword.end());
    std::wstring likeKeyword = L"%" + wKeyword + L"%";

    SQLLEN ind = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Search query failed.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLINTEGER id;
    SQLWCHAR name[100];
    SQLWCHAR barcode[50];
    SQLWCHAR unit[20];
    double quantity;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, barcode, sizeof(barcode), NULL);
        SQLGetData(stmt, 4, SQL_C_WCHAR, unit, sizeof(unit), NULL);
        SQLGetData(stmt, 5, SQL_C_DOUBLE, &quantity, 0, NULL);

        std::wcout << id << L" | "
                   << name << L" | "
                   << barcode << L" | "
                   << unit << L" | "
                   << quantity << std::endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
void Database::updateProduct(int productId,
                             const std::string& name,
                             const std::string& barcode,
                             Unit unit,
                             double buyingPrice,
                             double sellingPrice)
{
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET Name = ?, Barcode = ?, Unit = ?, BuyingPrice = ?, SellingPrice = ? "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::wstring wName(name.begin(), name.end());
    std::wstring wBarcode(barcode.begin(), barcode.end());

    std::string unitStr = toString(unit);
    std::wstring wUnit(unitStr.begin(), unitStr.end());

    SQLLEN nameInd = SQL_NTS;
    SQLLEN barcodeInd = SQL_NTS;
    SQLLEN unitInd = SQL_NTS;
    SQLLEN buyingInd = 0;
    SQLLEN sellingInd = 0;
    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wName.c_str(), 0, &nameInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     50, 0, (SQLPOINTER)wBarcode.c_str(), 0, &barcodeInd);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     20, 0, (SQLPOINTER)wUnit.c_str(), 0, &unitInd);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &buyingPrice, 0, &buyingInd);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &sellingPrice, 0, &sellingInd);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &productId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to update product.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}