#include "StockMovement.h"
#include <stdexcept>

StockMovement::StockMovement(double amount, MovementType type, MovementReason reason)
    : id(0), reason(reason), amount(amount), type(type), date("")
{
    if(amount <= 0.0) {
        throw std::invalid_argument("Amount must be positive");
    }
}

double StockMovement::getAmount() const {
    return amount;
}

MovementType StockMovement::getType() const {
    return type;
}

MovementReason StockMovement::getReason() const {
    return reason;
}

std::string StockMovement::getDate() const {
    return date;
}

void StockMovement::setDate(const std::string& date) {
    this->date = date;
}