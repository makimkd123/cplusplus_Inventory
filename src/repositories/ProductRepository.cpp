#include <iostream>
#include <stdexcept>
#include "ProductRepository.h"
#include "utils/Utils.h"

ProductRepository::ProductRepository(Database& database)
    : database(database) {}

void ProductRepository::insertProduct(const std::string& name,
                                      const std::string& barcode,
                                      Unit unit,
                                      double buyingPrice,
                                      double sellingPrice,
                                      double tax,
                                      double height,
                                      double width,
                                      double depth,
                                      int primaryCategoryId,
                                      int supplierId,
                                      int subCategoryId,
                                      double minimumQuantity) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"INSERT INTO Products "
        L"(Name, Barcode, Unit, BuyingPrice, SellingPrice, Tax, Height, Width, Depth, "
        L"PrimaryCategoryID, SupplierId, SubCategoryId, MinQuantity) "
        L"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

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
    SQLLEN taxInd = 0;
    SQLLEN heightInd = 0;
    SQLLEN widthInd = 0;
    SQLLEN depthInd = 0;
    SQLLEN primaryCategoryInd = 0;
    SQLLEN supplierInd = 0;
    SQLLEN subCategoryInd = 0;
    SQLINTEGER nullableSubCategoryId = subCategoryId;
    SQLLEN minimumQuantityInd = 0;

    if (subCategoryId == -1) {
        subCategoryInd = SQL_NULL_DATA;
    }

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

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &tax, 0, &taxInd);

    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &height, 0, &heightInd);

    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &width, 0, &widthInd);

    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &depth, 0, &depthInd);

    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &primaryCategoryId, 0, &primaryCategoryInd);

    SQLBindParameter(stmt, 11, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &supplierId, 0, &supplierInd);

    SQLBindParameter(stmt, 12, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                    0, 0, &nullableSubCategoryId, 0, &subCategoryInd);

    SQLBindParameter(stmt, 13, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &minimumQuantity, 0, &minimumQuantityInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw std::runtime_error("Failed to add product.");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

std::vector<Product> ProductRepository::searchProduct(const std::string& keyword) {
    std::vector<Product> products;
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT ProductID, Name, BARCODE, Unit, Quantity, "
        L"PrimaryCategoryId, SupplierId, Status "
        L"FROM Products "
        L"WHERE BARCODE = ? "
        L"OR Name = ? "
        L"OR Name LIKE ? "
        L"OR CAST(PrimaryCategoryId AS NVARCHAR(50)) = ? "
        L"OR CAST(SupplierId AS NVARCHAR(50)) = ? "
        L"ORDER BY "
        L"CASE "
        L"WHEN BARCODE = ? THEN 1 "
        L"WHEN Name = ? THEN 2 "
        L"WHEN Name LIKE ? THEN 3 "
        L"WHEN CAST(PrimaryCategoryId AS NVARCHAR(50)) = ? THEN 4 "
        L"WHEN CAST(SupplierId AS NVARCHAR(50)) = ? THEN 5 "
        L"ELSE 6 END;";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::wstring wKeyword(keyword.begin(), keyword.end());
    std::wstring likeKeyword = L"%" + wKeyword + L"%";

    SQLLEN ind1 = SQL_NTS;
    SQLLEN ind2 = SQL_NTS;
    SQLLEN ind3 = SQL_NTS;
    SQLLEN ind4 = SQL_NTS;
    SQLLEN ind5 = SQL_NTS;
    SQLLEN ind6 = SQL_NTS;
    SQLLEN ind7 = SQL_NTS;
    SQLLEN ind8 = SQL_NTS;
    SQLLEN ind9 = SQL_NTS;
    SQLLEN ind10 = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind1);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind2);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind3);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind4);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind5);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind6);
    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind7);
    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind8);
    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind9);
    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyword.c_str(), 0, &ind10);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return products;
    }

    SQLINTEGER id;
    SQLWCHAR nameBuffer[100];
    SQLWCHAR barcodeBuffer[50];
    SQLWCHAR unitBuffer[20];
    double quantity;
    SQLINTEGER primaryCategoryId;
    SQLINTEGER supplierId;
    SQLWCHAR statusBuffer[20];

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nameBuffer, sizeof(nameBuffer), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, barcodeBuffer, sizeof(barcodeBuffer), NULL);
        SQLGetData(stmt, 4, SQL_C_WCHAR, unitBuffer, sizeof(unitBuffer), NULL);
        SQLGetData(stmt, 5, SQL_C_DOUBLE, &quantity, 0, NULL);
        SQLGetData(stmt, 6, SQL_C_SLONG, &primaryCategoryId, 0, NULL);
        SQLGetData(stmt, 7, SQL_C_SLONG, &supplierId, 0, NULL);
        SQLGetData(stmt, 8, SQL_C_WCHAR, statusBuffer, sizeof(statusBuffer), NULL);

        Product product;

        product.setId(id);
        product.setName(
            std::string(nameBuffer, nameBuffer + wcslen(nameBuffer))
        );

        product.setBarcode(
            std::string(barcodeBuffer, barcodeBuffer + wcslen(barcodeBuffer))
        );
        product.setUnit(toUnit(unitBuffer));
        product.setQuantity(quantity);
        product.setCategory(primaryCategoryId);
        product.setSupplierId(supplierId);
        product.setStatus(toProductStatus(statusBuffer));

        products.push_back(product);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return products;
}

void ProductRepository::updateProduct(int productId,
                                      const std::string& name,
                                      const std::string& barcode,
                                      Unit unit,
                                      double buyingPrice,
                                      double sellingPrice,
                                      double tax,
                                      double height,
                                      double width,
                                      double depth,
                                      int primaryCategoryId,
                                      int supplierId,
                                      int subCategoryId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET Name = ?, BARCODE = ?, Unit = ?, BuyingPrice = ?, SellingPrice = ?, "
        L"Tax = ?, Height = ?, Width = ?, Depth = ?, "
        L"PrimaryCategoryID = ?, SupplierId = ?, SubCategoryId = ? "
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
    SQLLEN taxInd = 0;
    SQLLEN heightInd = 0;
    SQLLEN widthInd = 0;
    SQLLEN depthInd = 0;

    SQLLEN primaryCategoryInd = 0;
    SQLLEN supplierInd = 0;
    SQLLEN subCategoryInd = 0;
    SQLINTEGER nullableSubCategoryId = subCategoryId;

    if (subCategoryId == -1) {
        subCategoryInd = SQL_NULL_DATA;
    }
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

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &tax, 0, &taxInd);

    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &height, 0, &heightInd);

    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &width, 0, &widthInd);

    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &depth, 0, &depthInd);

    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &primaryCategoryId, 0, &primaryCategoryInd);

    SQLBindParameter(stmt, 11, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &supplierId, 0, &supplierInd);

    SQLBindParameter(stmt, 12, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &nullableSubCategoryId, 0, &subCategoryInd);

    SQLBindParameter(stmt, 13, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &productId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw std::runtime_error("Failed to update product.");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

std::optional<double>
ProductRepository::getProductQuantity(int productId) {

    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT Quantity "
        L"FROM Products "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN productIdInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0,
                     0, &productId, 0, &productIdInd);

    SQLRETURN ret = SQLExecute(stmt);

    // QUERY FAILED
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    double quantity;

    // PRODUCT NOT FOUND
    if (SQLFetch(stmt) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    SQLGetData(stmt,1, SQL_C_DOUBLE, &quantity, 0, NULL);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return quantity;
}

bool ProductRepository::updateProductQuantity(
    int productId,
    double quantityChange
) {

    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET Quantity = Quantity + ? "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN quantityInd = 0;
    SQLLEN productIdInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0,
                     0, &quantityChange, 0, &quantityInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0,0,&productId,0,&productIdInd);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

std::optional<Product> ProductRepository::getProductById(int productId) {
    SQLHSTMT stmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT ProductID, Name, Barcode, Unit, Quantity, "
        L"BuyingPrice, SellingPrice, Tax, Height, Width, Depth, "
        L"PrimaryCategoryId, SupplierId, SubCategoryId, MinQuantity "
        L"FROM Products "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLBindParameter(stmt,1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                     0, 0, &productId, 0, NULL);

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    if (SQLFetch(stmt) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    SQLINTEGER id;
    SQLWCHAR name[100] = {};
    SQLWCHAR barcode[50] = {};
    SQLWCHAR unit[20] = {};
    double quantity = 0.0;

    double buyingPrice = 0.0;
    double sellingPrice = 0.0;
    double tax = 0.0;
    double height = 0.0;
    double width = 0.0;
    double depth = 0.0;

    SQLINTEGER categoryId = -1;
    SQLINTEGER supplierId = -1;
    SQLINTEGER subCategoryId = -1;
    double minimumQuantity = 0.0;

    SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, NULL);
    SQLGetData(stmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
    SQLGetData(stmt, 3, SQL_C_WCHAR, barcode, sizeof(barcode), NULL);
    SQLGetData(stmt, 4, SQL_C_WCHAR, unit, sizeof(unit), NULL);
    SQLGetData(stmt, 5, SQL_C_DOUBLE, &quantity, 0, NULL);

    SQLGetData(stmt, 6, SQL_C_DOUBLE, &buyingPrice, 0, NULL);
    SQLGetData(stmt, 7, SQL_C_DOUBLE, &sellingPrice, 0, NULL);
    SQLGetData(stmt, 8, SQL_C_DOUBLE, &tax, 0, NULL);
    SQLGetData(stmt, 9, SQL_C_DOUBLE, &height, 0, NULL);
    SQLGetData(stmt, 10, SQL_C_DOUBLE, &width, 0, NULL);
    SQLGetData(stmt, 11, SQL_C_DOUBLE, &depth, 0, NULL);
    SQLGetData(stmt, 12, SQL_C_LONG, &categoryId, 0, NULL);
    SQLGetData(stmt, 13, SQL_C_LONG, &supplierId, 0, NULL);
    SQLGetData(stmt, 14, SQL_C_LONG, &subCategoryId, 0, NULL);
    SQLGetData(stmt, 15, SQL_C_DOUBLE, &minimumQuantity, 0, NULL);

    Product product;

    product.setId(id);
    product.setName(std::string(name, name + wcslen(name)));
    product.setBarcode(std::string(barcode, barcode + wcslen(barcode)));
    product.setUnit(toUnit(unit));
    product.setQuantity(quantity);

    product.setBuyingPrice(buyingPrice);
    product.setSellingPrice(sellingPrice);
    product.setTax(tax);
    product.setHeight(height);
    product.setWidth(width);
    product.setDepth(depth);

    product.setCategory(categoryId);
    product.setSupplierId(supplierId);
    product.setSubCategory(subCategoryId);
    product.setMinimumQuantity(minimumQuantity);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return product;
}


bool ProductRepository::updateMinimumQuantity(int productId, double minimumQuantity) {
    if (minimumQuantity < 0) {
        return false;
    }

    SQLHSTMT stmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET MinQuantity = ? "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE,
                     0, 0, &minimumQuantity, 0, NULL);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                     0, 0, &productId, 0, NULL);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}
std::vector<Product> ProductRepository::getLowStockProducts() {
    std::vector<Product> products;

    SQLHSTMT stmt = NULL;

    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT ProductID, Name, Barcode, Quantity, MinQuantity, Unit, Status "
        L"FROM Products "
        L"WHERE Quantity <= MinQuantity "
        L"AND Status != 'BLOCKED';";

    SQLRETURN ret = SQLExecDirectW(stmt, query, SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return products;
    }

    SQLINTEGER productId;
    SQLWCHAR name[100];
    SQLWCHAR barcode[50];
    double quantity;
    double minQuantity;
    SQLWCHAR unit[20];
    SQLWCHAR status[30];

    while (SQLFetch(stmt) == SQL_SUCCESS) {

        SQLGetData(stmt, 1, SQL_C_LONG, &productId, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, barcode, sizeof(barcode), NULL);
        SQLGetData(stmt, 4, SQL_C_DOUBLE, &quantity, 0, NULL);
        SQLGetData(stmt, 5, SQL_C_DOUBLE, &minQuantity, 0, NULL);
        SQLGetData(stmt, 6, SQL_C_WCHAR, unit, sizeof(unit), NULL);
        SQLGetData(stmt, 7, SQL_C_WCHAR, status, sizeof(status), NULL);

        Product product;

        product.setId(productId);
        product.setName(std::string(name, name + wcslen(name)));
        product.setBarcode(std::string(barcode, barcode + wcslen(barcode)));
        product.setQuantity(quantity);
        product.setMinimumQuantity(minQuantity);
        product.setUnit(toUnit(unit));
        product.setStatus(toProductStatus(status));

        products.push_back(product);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return products;
}
bool ProductRepository::updateProductStatus(
    int productId,
    ProductStatus status
) {

    std::wstring statusStr = toString(status);

    SQLHSTMT stmt = NULL;

    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Products "
        L"SET Status = ? "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN statusLength = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     statusStr.length(), 0, (SQLPOINTER)statusStr.c_str(), 0, &statusLength);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                     0, 0, &productId, 0, NULL);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

std::optional<ProductStatus>
ProductRepository::getProductStatus(int productId) {

    SQLHSTMT stmt = NULL;

    SQLAllocHandle(SQL_HANDLE_STMT,database.getConnection(),&stmt);

    SQLWCHAR query[] =
        L"SELECT Status "
        L"FROM Products "
        L"WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,SQL_C_LONG, SQL_INTEGER,
                     0, 0, &productId, 0, NULL);

    SQLRETURN ret = SQLExecute(stmt);

    // QUERY FAILED
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    SQLWCHAR statusBuffer[30];

    // PRODUCT NOT FOUND
    if (SQLFetch(stmt) != SQL_SUCCESS) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    SQLGetData(stmt,1,SQL_C_WCHAR,statusBuffer,sizeof(statusBuffer),NULL);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    try {
        return toProductStatus(statusBuffer);
    }
    catch (const std::exception&) {

        // INVALID STATUS STRING IN DATABASE
        return std::nullopt;
    }
}

bool ProductRepository::barcodeExists(const std::string& barcode) {
    std::wstring wBarcode(barcode.begin(), barcode.end());
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT 1 FROM Products WHERE BARCODE = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN barcodeInd = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     50, 0, (SQLPOINTER)wBarcode.c_str(), 0, &barcodeInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    ret = SQLFetch(stmt);

    bool exists =
        (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return exists;
}

bool ProductRepository::productExists(int productId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT 1 FROM Products WHERE ProductID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &productId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    ret = SQLFetch(stmt);

    bool exists =
        (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return exists;
}