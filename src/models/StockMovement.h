#ifndef STOCKMOVEMENT_H
#define STOCKMOVEMENT_H

#include <string>

enum class MovementType{
    STOCK_IN,
    STOCK_OUT,
    ADJUSTMENT
};

enum class MovementReason{
    BOUGHT,
    SOLD,
    DAMAGED,
    OTHER
};

class StockMovement {
private:
    int id;
    MovementReason reason;
    double amount;
    MovementType type;
    std::string date;

public:
    StockMovement(double amount, MovementType type, MovementReason reason);
    double getAmount() const;
    MovementType getType() const;
    MovementReason getReason() const;
    std::string getDate() const;
    void setDate(const std::string& date);
};



#endif