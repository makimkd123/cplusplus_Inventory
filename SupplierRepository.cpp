#include "SupplierRepository.h"

#include <iostream>
#include <windows.h>
#include <sqlext.h>

SupplierRepository::SupplierRepository(Database& database)
    : database(database)
{
}
void SupplierRepository::insertSupplier(const Supplier& supplier) {
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

    SQLBindParameter(stmt, 1,
                     SQL_PARAM_INPUT,
                     SQL_C_WCHAR,
                     SQL_WVARCHAR,
                     100, 0,
                     (SQLPOINTER)wName.c_str(),
                     0,
                     &nameInd);

    SQLBindParameter(stmt, 2,
                     SQL_PARAM_INPUT,
                     SQL_C_WCHAR,
                     SQL_WVARCHAR,
                     100, 0,
                     (SQLPOINTER)wContactName.c_str(),
                     0,
                     &contactInd);

    SQLBindParameter(stmt, 3,
                     SQL_PARAM_INPUT,
                     SQL_C_WCHAR,
                     SQL_WVARCHAR,
                     50, 0,
                     (SQLPOINTER)wPhone.c_str(),
                     0,
                     &phoneInd);

    SQLBindParameter(stmt, 4,
                     SQL_PARAM_INPUT,
                     SQL_C_WCHAR,
                     SQL_WVARCHAR,
                     100, 0,
                     (SQLPOINTER)wEmail.c_str(),
                     0,
                     &emailInd);

    SQLBindParameter(stmt, 5,
                     SQL_PARAM_INPUT,
                     SQL_C_WCHAR,
                     SQL_WVARCHAR,
                     255, 0,
                     (SQLPOINTER)wAddress.c_str(),
                     0,
                     &addressInd);

    SQLBindParameter(stmt, 6,
                     SQL_PARAM_INPUT,
                     SQL_C_BIT,
                     SQL_BIT,
                     0, 0,
                     &isActive,
                     0,
                     &activeInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        std::cout << "Supplier added successfully.\n";
    } else {
        std::cout << "Failed to insert supplier.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void SupplierRepository::printAllSuppliers() {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT SupplierID, Name, ContactName, Phone, Email, IsActive "
        L"FROM Suppliers "
        L"ORDER BY Name;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to retrieve suppliers.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLINTEGER supplierId;
    SQLWCHAR nameBuffer[100];
    SQLWCHAR contactBuffer[100];
    SQLWCHAR phoneBuffer[50];
    SQLWCHAR emailBuffer[100];
    SQLCHAR isActive;

    bool found = false;

    std::wcout << L"\nID | Name | Contact | Phone | Email | Active\n";
    std::wcout << L"------------------------------------------------------------\n";

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        found = true;

        SQLGetData(stmt, 1, SQL_C_SLONG, &supplierId, 0, nullptr);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nameBuffer, sizeof(nameBuffer), nullptr);
        SQLGetData(stmt, 3, SQL_C_WCHAR, contactBuffer, sizeof(contactBuffer), nullptr);
        SQLGetData(stmt, 4, SQL_C_WCHAR, phoneBuffer, sizeof(phoneBuffer), nullptr);
        SQLGetData(stmt, 5, SQL_C_WCHAR, emailBuffer, sizeof(emailBuffer), nullptr);
        SQLGetData(stmt, 6, SQL_C_BIT, &isActive, 0, nullptr);

        std::wcout << supplierId << L" | "
                   << nameBuffer << L" | "
                   << contactBuffer << L" | "
                   << phoneBuffer << L" | "
                   << emailBuffer << L" | "
                   << (isActive ? L"YES" : L"NO")
                   << std::endl;
    }

    if (!found) {
        std::cout << "No suppliers found.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}




bool SupplierRepository::getSupplierActiveStatus(
    int supplierId,
    bool& isActive
) {
    SQLHSTMT stmt;

    SQLAllocHandle(
        SQL_HANDLE_STMT,
        database.getConnection(),
        &stmt
    );

    SQLWCHAR query[] =
        L"SELECT IsActive "
        L"FROM Suppliers "
        L"WHERE SupplierID = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_SLONG,
        SQL_INTEGER,
        0,
        0,
        &supplierId,
        0,
        &idInd
    );

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS &&
        ret != SQL_SUCCESS_WITH_INFO) {

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLCHAR activeValue;

    if (SQLFetch(stmt) == SQL_SUCCESS) {

        SQLGetData(
            stmt,
            1,
            SQL_C_BIT,
            &activeValue,
            0,
            nullptr
        );

        isActive = activeValue;

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        return true;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return false;
}
void SupplierRepository::deactivateSupplier(int supplierId){
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

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to deactivate supplier.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

};


void SupplierRepository::searchSupplier(const std::string& keyword){
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

    std::wstring wKeyWord(keyword.begin(), keyword.end());
    std::wstring likeKeyword = L"%" + wKeyWord + L"%";
    SQLLEN ind = SQL_NTS;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 11, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 12, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 13, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 14, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLBindParameter(stmt, 15, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)wKeyWord.c_str(), 0, &ind);

    SQLBindParameter(stmt, 16, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0,
                     (SQLPOINTER)likeKeyword.c_str(), 0, &ind);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO){
        std::cout <<"Search query failed.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLINTEGER id;
    SQLWCHAR nameBuffer[100];
    SQLWCHAR ContactNameBuffer[100];
    SQLWCHAR PhoneBuffer[50];
    SQLWCHAR EmailBuffer[100];
    SQLWCHAR AddressBuffer[255];
    SQLCHAR IsActiveBuffer;
    bool found = false;


    std::wcout << L"\nID | Name | ContactName | Phone | Email | Address | Status\n";
    std::wcout<< L"-------------------------------------------------------------\n";

    while (SQLFetch(stmt) == SQL_SUCCESS){
        found = true;

        SQLGetData(stmt, 1, SQL_C_SLONG, &id, 0, NULL);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nameBuffer, sizeof(nameBuffer), NULL);
        SQLGetData(stmt, 3, SQL_C_WCHAR, ContactNameBuffer, sizeof(ContactNameBuffer), NULL);
        SQLGetData(stmt, 4, SQL_C_WCHAR, PhoneBuffer, sizeof(PhoneBuffer), NULL);
        SQLGetData(stmt, 5, SQL_C_WCHAR, EmailBuffer, sizeof(EmailBuffer), NULL);
        SQLGetData(stmt, 6, SQL_C_WCHAR, AddressBuffer, sizeof(AddressBuffer), NULL);
        SQLGetData(stmt, 7, SQL_C_BIT, &IsActiveBuffer, 0, NULL);

        std::wcout << id << L" | "
                   << nameBuffer << L" | "
                   << ContactNameBuffer << L" | "
                   << PhoneBuffer << L" | "
                   << EmailBuffer << L" | "                                      
                   << AddressBuffer << L" | "
                   << (IsActiveBuffer ? L"ACTIVE" : L"INACTIVE") << std::endl;
    }

    if (!found) {
            std::cout << "No supplier found.\n";
    };
    
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

};

void SupplierRepository::updateSupplier(const Supplier& supplier) {
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

    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        std::cout << "Supplier updated successfully.\n";
    } else {
        std::cout << "Failed to update supplier.\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}