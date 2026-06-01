#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include "Database.h"
#include "utils/Utils.h"

static std::string trim(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

static std::map<std::string, std::string> loadConfig(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Could not open config file: " << filename << "\n";
        return config;
    }

    std::string line;
    std::string section;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            section = trim(line.substr(1, line.length() - 2));
            continue;
        }

        size_t equalsPos = line.find('=');

        if (equalsPos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));

        config[section + "." + key] = value;
    }

    return config;
}

static std::wstring toWideString(const std::string& text) {
    return std::wstring(text.begin(), text.end());
}

void Database::connect(const std::string& configFile) {
    auto config = loadConfig(configFile);

    std::string driver = config["database.driver"];
    std::string server = config["database.server"];
    std::string database = config["database.database"];
    std::string trustedConnection = config["database.trusted_connection"];

    if (driver.empty()) driver = "ODBC Driver 17 for SQL Server";
    if (server.empty()) server = "localhost";
    if (database.empty()) database = "Inventory";
    if (trustedConnection.empty()) trustedConnection = "yes";

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);

    std::wstring connStr =
        L"Driver={" + toWideString(driver) + L"};"
        L"Server=" + toWideString(server) + L";"
        L"Database=" + toWideString(database) + L";"
        L"Trusted_Connection=" + toWideString(trustedConnection) + L";";

    SQLWCHAR outStr[1024];
    SQLSMALLINT outStrLen;

    SQLRETURN ret = SQLDriverConnectW(
        dbc,
        nullptr,
        (SQLWCHAR*)connStr.c_str(),
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
Database::Database()
    : env(SQL_NULL_HENV), dbc(SQL_NULL_HDBC) {
}

Database::~Database() {
    if (dbc != SQL_NULL_HDBC) {
        SQLDisconnect(dbc);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    }

    if (env != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, env);
    }
}

SQLHDBC Database::getConnection() const {
    return dbc;
}

void Database::beginTransaction() {
    SQLRETURN ret = SQLSetConnectAttr(
        dbc,
        SQL_ATTR_AUTOCOMMIT,
        (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
        0
    );

    if (!SQL_SUCCEEDED(ret)) {
        throw std::runtime_error("Failed to begin transaction.");
    }
}

void Database::commitTransaction() {
    SQLRETURN ret = SQLEndTran(
        SQL_HANDLE_DBC,
        dbc,
        SQL_COMMIT
    );

    if (!SQL_SUCCEEDED(ret)) {
        throw std::runtime_error("Failed to commit transaction.");
    }

    SQLSetConnectAttr(
        dbc,
        SQL_ATTR_AUTOCOMMIT,
        (SQLPOINTER)SQL_AUTOCOMMIT_ON,
        0
    );
}

void Database::rollbackTransaction() {
    SQLRETURN ret = SQLEndTran(
        SQL_HANDLE_DBC,
        dbc,
        SQL_ROLLBACK
    );

    SQLSetConnectAttr(
        dbc,
        SQL_ATTR_AUTOCOMMIT,
        (SQLPOINTER)SQL_AUTOCOMMIT_ON,
        0
    );

    if (!SQL_SUCCEEDED(ret)) {
        throw std::runtime_error("Failed to rollback transaction.");
    }
}