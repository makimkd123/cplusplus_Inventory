#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "StockMovement.h"
#include "Product.h"

std::wstring toString(MovementType type);
std::wstring toString(MovementReason reason);
MovementType toMovementType(const std::wstring& str);
MovementReason toMovementReason(const std::wstring& str);
std::string toString(Unit unit);


#endif