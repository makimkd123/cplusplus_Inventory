#include "SupplierRepository.h"

#include <iostream>
#include <windows.h>
#include <sqlext.h>

SupplierRepository::SupplierRepository(Database& database)
    : database(database)
{
}
bool SupplierRepository::insertSupplier(const Supplier& supplier) {
    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"INSERT INTO Suppliers "
        L"(Name, ContactName, Phone, Email, Address, IsActive) "
        L"VALUES (?, ?, ?, ?, ?, ?);";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::string name = supplier.getName();
    std::string contactName = supplier.getContactName();
    std::string phone = supplier.getPhone();
    std::string email = supplier.getEmail();
    std::string address = supplier.getAddress();

    std::wstring wName(name.begin(), name.end());
    std::wstring wContactName(contactName.begin(), contactName.end());
    std::wstring wPhone(phone.begin(), phone.end());
    std::wstring wEmail(email.begin(), email.end());
    std::wstring wAddress(address.begin(), address.end());

    SQLCHAR isActive = supplier.isActive() ? 1 : 0;

    SQLLEN nameInd = SQL_NTS;
    SQLLEN contactInd = SQL_NTS;
    SQLLEN phoneInd = SQL_NTS;
    SQLLEN emailInd = SQL_NTS;
    SQLLEN addressInd = SQL_NTS;
    SQLLEN activeInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wName.c_str(), 0, &nameInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wContactName.c_str(), 0, &contactInd);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     50, 0, (SQLPOINTER)wPhone.c_str(), 0, &phoneInd);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wEmail.c_str(), 0, &emailInd);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     255, 0, (SQLPOINTER)wAddress.c_str(), 0, &addressInd);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT,
                     0, 0, &isActive, 0, &activeInd);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

std::vector<Supplier> SupplierRepository::getAllSuppliers() {
    std::vector<Supplier> suppliers;

    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT,database.getConnection(),&stmt);

    SQLWCHAR query[] =
        L"SELECT SupplierID, Name, ContactName, Phone, Email, Address, IsActive "
        L"FROM Suppliers "
        L"ORDER BY Name;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return suppliers;
    }

    SQLINTEGER supplierId;

    SQLWCHAR nameBuffer[100];
    SQLWCHAR contactBuffer[100];
    SQLWCHAR phoneBuffer[50];
    SQLWCHAR emailBuffer[100];
    SQLWCHAR addressBuffer[255];

    SQLCHAR isActive;

    while (SQLFetch(stmt) == SQL_SUCCESS) {

        SQLGetData(stmt, 1, SQL_C_SLONG,&supplierId, 0, nullptr);

        SQLGetData(stmt, 2, SQL_C_WCHAR,nameBuffer, sizeof(nameBuffer), nullptr);

        SQLGetData(stmt, 3, SQL_C_WCHAR,contactBuffer, sizeof(contactBuffer), nullptr);

        SQLGetData(stmt, 4, SQL_C_WCHAR,phoneBuffer, sizeof(phoneBuffer), nullptr);

        SQLGetData(stmt, 5, SQL_C_WCHAR,emailBuffer, sizeof(emailBuffer), nullptr);

        SQLGetData(stmt, 6, SQL_C_WCHAR,addressBuffer, sizeof(addressBuffer), nullptr);

        SQLGetData(stmt, 7, SQL_C_BIT,&isActive, 0, nullptr);

        Supplier supplier(
            supplierId,

            std::string(nameBuffer,
                        nameBuffer + wcslen(nameBuffer)),

            std::string(contactBuffer,
                        contactBuffer + wcslen(contactBuffer)),

            std::string(phoneBuffer,
                        phoneBuffer + wcslen(phoneBuffer)),

            std::string(emailBuffer,
                        emailBuffer + wcslen(emailBuffer)),

            std::string(addressBuffer,
                        addressBuffer + wcslen(addressBuffer)),

            isActive != 0
        );

        suppliers.push_back(supplier);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return suppliers;
}


bool SupplierRepository::getSupplierActiveStatus(
    int supplierId,
    bool& isActive
) {
    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT,database.getConnection(),&stmt);

    SQLWCHAR query[] =
        L"SELECT IsActive "
        L"FROM Suppliers "
        L"WHERE SupplierID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &supplierId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS &&
        ret != SQL_SUCCESS_WITH_INFO) {

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLCHAR activeValue;

    if (SQLFetch(stmt) == SQL_SUCCESS) {

        SQLGetData(stmt,1,SQL_C_BIT,&activeValue,0,nullptr);

        isActive = activeValue;

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        return true;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return false;
}
bool SupplierRepository::deactivateSupplier(int supplierId){
    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[]=
    L"UPDATE Suppliers "
    L"SET IsActive = 0 "
    L"WHERE SupplierId = ?;";
    
    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd=0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &supplierId, 0, &idInd);
    
    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);

}

std::vector<Supplier> SupplierRepository::searchSuppliers(const std::string& keyword) {
    std::vector<Supplier> suppliers;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT SupplierID, Name, ContactName, Phone, Email, Address, IsActive "
        L"FROM Suppliers "
        L"WHERE Name = ? "
        L"OR Name LIKE ? "
        L"OR ContactName = ? "
        L"OR ContactName LIKE ? "
        L"OR Phone = ? "
        L"OR Phone LIKE ? "
        L"OR Email = ? "
        L"OR Email LIKE ? "
        L"ORDER BY "
        L"CASE "
        L"WHEN Name = ? THEN 1 "
        L"WHEN Name LIKE ? THEN 2 "
        L"WHEN ContactName = ? THEN 3 "
        L"WHEN ContactName LIKE ? THEN 4 "
        L"WHEN Phone = ? THEN 5 "
        L"WHEN Phone LIKE ? THEN 6 "
        L"WHEN Email = ? THEN 7 "
        L"WHEN Email LIKE ? THEN 8 "
        L"ELSE 9 END;";

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
    SQLLEN ind11 = SQL_NTS;
    SQLLEN ind12 = SQL_NTS;
    SQLLEN ind13 = SQL_NTS;
    SQLLEN ind14 = SQL_NTS;
    SQLLEN ind15 = SQL_NTS;
    SQLLEN ind16 = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind1);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind2);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind3);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind4);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind5);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind6);

    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind7);

    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind8);

    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind9);

    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind10);

    SQLBindParameter(stmt, 11, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind11);

    SQLBindParameter(stmt, 12, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind12);

    SQLBindParameter(stmt, 13, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind13);

    SQLBindParameter(stmt, 14, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind14);

    SQLBindParameter(stmt, 15, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wKeyword.c_str(), 0, &ind15);

    SQLBindParameter(stmt, 16, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)likeKeyword.c_str(), 0, &ind16);

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return suppliers;
    }

    SQLINTEGER id;
    SQLWCHAR nameBuffer[100];
    SQLWCHAR contactNameBuffer[100];
    SQLWCHAR phoneBuffer[50];
    SQLWCHAR emailBuffer[100];
    SQLWCHAR addressBuffer[255];
    SQLCHAR isActiveBuffer;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nameBuffer, sizeof(nameBuffer), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, contactNameBuffer, sizeof(contactNameBuffer), NULL);
        SQLGetData(stmt, 4, SQL_C_WCHAR, phoneBuffer, sizeof(phoneBuffer), NULL);
        SQLGetData(stmt, 5, SQL_C_WCHAR, emailBuffer, sizeof(emailBuffer), NULL);
        SQLGetData(stmt, 6, SQL_C_WCHAR, addressBuffer, sizeof(addressBuffer), NULL);
        SQLGetData(stmt, 7, SQL_C_BIT, &isActiveBuffer, 0, NULL);

        Supplier supplier(
            id,
            std::string(nameBuffer, nameBuffer + wcslen(nameBuffer)),
            std::string(contactNameBuffer, contactNameBuffer + wcslen(contactNameBuffer)),
            std::string(phoneBuffer, phoneBuffer + wcslen(phoneBuffer)),
            std::string(emailBuffer, emailBuffer + wcslen(emailBuffer)),
            std::string(addressBuffer, addressBuffer + wcslen(addressBuffer)),
            isActiveBuffer != 0
        );

        suppliers.push_back(supplier);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return suppliers;
}

bool SupplierRepository::updateSupplier(const Supplier& supplier) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Suppliers "
        L"SET Name = ?, ContactName = ?, Phone = ?, Email = ?, Address = ?, IsActive = ? "
        L"WHERE SupplierId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    int supplierId = supplier.getId();

    std::string name = supplier.getName();
    std::string contactName = supplier.getContactName();
    std::string phone = supplier.getPhone();
    std::string email = supplier.getEmail();
    std::string address = supplier.getAddress();

    std::wstring wName(name.begin(), name.end());
    std::wstring wContactName(contactName.begin(), contactName.end());
    std::wstring wPhone(phone.begin(), phone.end());
    std::wstring wEmail(email.begin(), email.end());
    std::wstring wAddress(address.begin(), address.end());

    SQLCHAR isActive = supplier.isActive() ? 1 : 0;

    SQLLEN nameInd = SQL_NTS;
    SQLLEN contactInd = SQL_NTS;
    SQLLEN phoneInd = SQL_NTS;
    SQLLEN emailInd = SQL_NTS;
    SQLLEN addressInd = SQL_NTS;
    SQLLEN activeInd = 0;
    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wName.c_str(), 0, &nameInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wContactName.c_str(), 0, &contactInd);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     50, 0, (SQLPOINTER)wPhone.c_str(), 0, &phoneInd);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wEmail.c_str(), 0, &emailInd);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     255, 0, (SQLPOINTER)wAddress.c_str(), 0, &addressInd);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT,
                     0, 0, &isActive, 0, &activeInd);

    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                     0, 0, &supplierId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

bool SupplierRepository::supplierExists(int supplierId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT 1 FROM Suppliers WHERE SupplierId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &supplierId, 0, &idInd);

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