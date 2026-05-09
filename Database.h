#ifndef DATABASE_H
#define DATABASE_H

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>

class Database {
private:
    SQLHENV env;
    SQLHDBC dbc;

public:
    Database();
    ~Database();

    void connect();

    // Repositories use this handle to prepare/execute SQL statements.
    SQLHDBC getConnection() const;

    // Optional generic helper for simple SQL commands.
    void executeNonQuery(const std::wstring& query);
    void executeStockMovementQuery(const std::wstring& query);
    void executeQuery(const std::wstring& query);
};

#endif
