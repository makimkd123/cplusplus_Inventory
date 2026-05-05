#ifndef DATABASE_H
#define DATABASE_H
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include "StockMovement.h"
#include "Product.h"

class Database{
    private:
        SQLHENV env;
        SQLHDBC dbc;
    public:
        Database();
        ~Database();
        void connect();
        void executeQuery(const std::wstring& query);
        void executeNonQuery(const std::wstring& query);
        void executeStockMovementQuery(const std::wstring& query);
        double getProductQuantity(int productId);
        void insertStockMovement(
            int productId,
            MovementType type,
            MovementReason reason,
            double amount
        );
        void updateProductQuantity(int productId, double quantityChange);
        void insertProduct(const std::string& name,
                   const std::string& barcode,
                   Unit unit,
                   double buyingPrice,
                   double sellingPrice);
        void searchProduct(const std::string& keyword);
        void updateProduct(int productId, 
                const std::string& name, 
                const std::string& barcode,
                Unit unit,
                double buyingPrice,
                double sellingPrice);
};

#endif