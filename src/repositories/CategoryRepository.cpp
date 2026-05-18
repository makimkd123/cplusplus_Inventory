#include "CategoryRepository.h"
#include <iostream>
#include <vector>

CategoryRepository::CategoryRepository(Database& db)
    : database(db) 
{}


bool CategoryRepository::addCategory(const Category& category) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"INSERT INTO Categories "
        L"(Name, Description, IsActive) "
        L"VALUES (?, ?, ?);";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::string nameStr = category.getName();
    std::string descStr = category.getDescription();
    std::wstring wName(nameStr.begin(), nameStr.end());
    std::wstring wDescription(descStr.begin(), descStr.end());

    SQLCHAR isActive = category.getIsActive() ? 1 : 0;

    SQLLEN nameInd = SQL_NTS;
    SQLLEN descriptionInd = SQL_NTS;
    SQLLEN activeInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)wName.c_str(), 0, &nameInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     255, 0, (SQLPOINTER)wDescription.c_str(), 0, &descriptionInd);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT,
                     0, 0, &isActive, 0, &activeInd);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

std::optional<Category>
CategoryRepository::getCategoryById(int categoryId) {
    SQLHSTMT stmt;

    SQLAllocHandle(
        SQL_HANDLE_STMT,
        database.getConnection(),
        &stmt
    );

    SQLWCHAR query[] =
        L"SELECT CategoryId, Name, Description, IsActive "
        L"FROM Categories "
        L"WHERE CategoryId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(
        stmt,
        1,
        SQL_PARAM_INPUT,
        SQL_C_LONG,
        SQL_INTEGER,
        0,
        0,
        &categoryId,
        0,
        &idInd
    );

    SQLRETURN ret = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    int id;
    SQLWCHAR name[100];
    SQLWCHAR description[255];
    SQLCHAR isActive;

    SQLLEN idLen;
    SQLLEN nameLen;
    SQLLEN descriptionLen;
    SQLLEN activeLen;

    SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_WCHAR, name, sizeof(name), &nameLen);
    SQLBindCol(stmt, 3, SQL_C_WCHAR, description, sizeof(description), &descriptionLen);
    SQLBindCol(stmt, 4, SQL_C_BIT, &isActive, 0, &activeLen);

    ret = SQLFetch(stmt);

    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    Category category(
        id,
        std::string(name, name + wcslen(name)),
        std::string(description, description + wcslen(description)),
        isActive == 1
    );

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return category;
}

std::vector<Category> CategoryRepository::getAllCategories() {
    std::vector<Category> categories;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT CategoryId, Name, Description, IsActive "
        L"FROM Categories "
        L"ORDER BY Name;";

    SQLRETURN ret = SQLExecDirectW(stmt, query, SQL_NTS);

    if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return categories;
    }

    int id;
    SQLWCHAR name[100];
    SQLWCHAR description[255];
    SQLCHAR isActive;

    SQLLEN idLen;
    SQLLEN nameLen;
    SQLLEN descriptionLen;
    SQLLEN activeLen;

    SQLBindCol(stmt, 1, SQL_C_LONG, &id, 0, &idLen);
    SQLBindCol(stmt, 2, SQL_C_WCHAR, name, sizeof(name), &nameLen);
    SQLBindCol(stmt, 3, SQL_C_WCHAR, description, sizeof(description), &descriptionLen);
    SQLBindCol(stmt, 4, SQL_C_BIT, &isActive, 0, &activeLen);

    while ((ret = SQLFetch(stmt)) == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        categories.emplace_back(
            id,
            std::string(name, name + wcslen(name)),
            std::string(description, description + wcslen(description)),
            isActive == 1
        );
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return categories;
}

bool CategoryRepository::updateCategory(const Category& category) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Categories "
        L"SET Name = ?, Description = ?, IsActive = ? "
        L"WHERE CategoryId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    std::string nameStr = category.getName();
    std::string descStr = category.getDescription();
    std::wstring name(nameStr.begin(), nameStr.end());
    std::wstring description(descStr.begin(), descStr.end());

    SQLCHAR isActive = category.getIsActive() ? 1 : 0;
    int categoryId = category.getCategoryId();

    SQLLEN nameInd = SQL_NTS;
    SQLLEN descriptionInd = SQL_NTS;
    SQLLEN activeInd = 0;
    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     100, 0, (SQLPOINTER)name.c_str(), 0, &nameInd);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
                     255, 0, (SQLPOINTER)description.c_str(), 0, &descriptionInd);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT,
                     0, 0, &isActive, 0, &activeInd);

    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                     0, 0, &categoryId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

bool CategoryRepository::deactivateCategory(int categoryId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"UPDATE Categories "
        L"SET IsActive = 0 "
        L"WHERE CategoryId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
                     0, 0, &categoryId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}

bool CategoryRepository::categoryExists(int categoryId) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT 1 FROM Categories WHERE CategoryId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(stmt,
                     1,
                     SQL_PARAM_INPUT,
                     SQL_C_SLONG,
                     SQL_INTEGER,
                     0,
                     0,
                     &categoryId,
                     0,
                     &idInd);

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

bool CategoryRepository::getCategoryActiveStatus(int categoryId, bool& isActive) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, database.getConnection(), &stmt);

    SQLWCHAR query[] =
        L"SELECT IsActive "
        L"FROM Categories "
        L"WHERE CategoryId = ?;";

    SQLPrepareW(stmt, query, SQL_NTS);

    SQLLEN idInd = 0;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                     0, 0, &categoryId, 0, &idInd);

    SQLRETURN ret = SQLExecute(stmt);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return false;
    }

    SQLCHAR activeValue;

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_BIT, &activeValue, 0, nullptr);
        isActive = activeValue;

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return true;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return false;
}