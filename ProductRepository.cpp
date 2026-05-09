#include <iostream>
#include "ProductRepository.h"
#include "Utils.h"

ProductRepository::ProductRepository(Database& database)
    : database(database) {}

void ProductRepository::insertProduct(const std::string& name,
                                      const std::string& barcode,
                                      Unit unit,
                                      double buyingPrice,
                                      double sellingPrice) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

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

void ProductRepository::searchProduct(const std::string& keyword) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT ProductID, Name, Barcode, Unit, Quantity, Category, SupplierId, Status "
        L"FROM Products "
        L"WHERE Barcode = ? "
        L"OR Name = ? "
        L"OR Name LIKE ? "
        L"OR Category = ? "
        L"OR Category LIKE ? "
        L"OR CAST(SupplierId AS NVARCHAR(50)) = ? "
        L"ORDER BY "
        L"CASE "
        L"WHEN Barcode = ? THEN 1 "
        L"WHEN Name = ? THEN 2 "
        L"WHEN Name LIKE ? THEN 3 "
        L"WHEN Category = ? THEN 4 "
        L"WHEN Category LIKE ? THEN 5 "
        L"WHEN CAST(SupplierId AS NVARCHAR(50)) = ? THEN 6 "
        L"ELSE 7 END;";

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
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 11, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 12, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Search query failed.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLINTEGER id;
    SQLWCHAR nameBuffer[100];
    SQLWCHAR barcodeBuffer[50];
    SQLWCHAR unitBuffer[20];
    double quantity;
    SQLWCHAR categoryBuffer[100];
    SQLINTEGER supplierId;
    SQLWCHAR statusBuffer[20];

    bool found = false;

    std::wcout << L"\nID | Name | Barcode | Unit | Quantity | Category | SupplierID | Status\n";
    std::wcout << L"--------------------------------------------------------------------------\n";

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        found = true;

        SQLGetData(stmt, 1, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nameBuffer, sizeof(nameBuffer), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, barcodeBuffer, sizeof(barcodeBuffer), NULL);
        SQLGetData(stmt, 4, SQL_C_WCHAR, unitBuffer, sizeof(unitBuffer), NULL);
        SQLGetData(stmt, 5, SQL_C_DOUBLE, &quantity, 0, NULL);
        SQLGetData(stmt, 6, SQL_C_WCHAR, categoryBuffer, sizeof(categoryBuffer), NULL);
        SQLGetData(stmt, 7, SQL_C_SLONG, &supplierId, 0, NULL);
        SQLGetData(stmt, 8, SQL_C_WCHAR, statusBuffer, sizeof(statusBuffer), NULL);

        std::wcout << id << L" | "
                   << nameBuffer << L" | "
                   << barcodeBuffer << L" | "
                   << unitBuffer << L" | "
                   << quantity << L" | "
                   << categoryBuffer << L" | "
                   << supplierId << L" | "
                   << statusBuffer << std::endl;
    }

    if (!found) {
        std::cout << "No products found.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void ProductRepository::updateProduct(int productId,
                                      const std::string& name,
                                      const std::string& barcode,
                                      Unit unit,
                                      double buyingPrice,
                                      double sellingPrice) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

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

double ProductRepository::getProductQuantity(int productId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] = L"SELECT Quantity FROM Products WHERE ProductID = ?;";
    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN productIdInd = 0;
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &productId, 0, &productIdInd);

    SQLRETURN ret = SQLExecute(stmt);

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

void ProductRepository::updateProductQuantity(int productId, double quantityChange) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET Quantity = Quantity + ? "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN quantityInd = 0;
    SQLLEN productIdInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &quantityChange, 0, &quantityInd);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &productId, 0, &productIdInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to update product quantity.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
void ProductRepository::printAllProducts() {
    database.executeQuery(
        L"SELECT ProductID, Name, Barcode, Unit, Quantity FROM Products;"
    );
}

void ProductRepository::printProductDetails(int productId) {
    std::wstring query =
        L"SELECT ProductID, Name, Barcode, Unit, Quantity "
        L"FROM Products WHERE ProductID = " + std::to_wstring(productId) + L";";

    database.executeQuery(query);
}
void ProductRepository::updateMinimumQuantity(int productId, double minimumQuantity) {
    if (minimumQuantity < 0) {
        std::cout << "Minimum quantity cannot be negative.\n";
        return;
    }

    std::wstring query =
        L"UPDATE Products SET MinQuantity = " + std::to_wstring(minimumQuantity) +
        L" WHERE ProductID = " + std::to_wstring(productId) + L";";

    database.executeNonQuery(query);

    std::cout << "Minimum quantity updated successfully.\n";
}
void ProductRepository::printLowStockProducts() {
    database.executeQuery(
        L"SELECT ProductID, Name, Barcode, Quantity, MinQuantity, Unit, Status "
        L"FROM Products "
        L"WHERE Quantity <= MinQuantity "
        L"AND Status != 'BLOCKED';"
    );
}
void ProductRepository::updateProductStatus(int productId, ProductStatus status) {
    std::wstring statusStr = productStatusToWString(status);

    std::wstring query =
        L"UPDATE Products SET Status = '" + statusStr +
        L"' WHERE ProductID = " + std::to_wstring(productId) + L";";

    database.executeNonQuery(query);

    std::cout << "Product status updated successfully.\n";
}
ProductStatus ProductRepository::getProductStatus(int productId) {
    std::wstring query =
        L"SELECT Status FROM Products WHERE ProductID = " +
        std::to_wstring(productId) + L";";

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return ProductStatus::BLOCKED;
    }

    wchar_t statusBuffer[50];

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, statusBuffer, sizeof(statusBuffer), nullptr);

        std::wstring status(statusBuffer);

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        if (status == L"ACTIVE") {
            return ProductStatus::ACTIVE;
        }
        if (status == L"INACTIVE") {
            return ProductStatus::INACTIVE;
        }
        if (status == L"BLOCKED") {
            return ProductStatus::BLOCKED;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return ProductStatus::BLOCKED;
}