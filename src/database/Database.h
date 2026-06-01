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

    void connect(const std::string& configFile = "config.ini");
    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    SQLHDBC getConnection() const;

};

#endif
