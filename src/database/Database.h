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

    SQLHDBC getConnection() const;

};

#endif
