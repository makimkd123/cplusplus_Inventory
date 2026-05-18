#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <stdexcept>

#include "models/StockMovement.h"
#include "models/Product.h"

std::wstring toString(MovementType type);
std::wstring toString(MovementReason reason);

std::string toStringNarrow(MovementType type);
std::string toStringNarrow(MovementReason reason);

MovementType toMovementType(const std::wstring& str);
MovementReason toMovementReason(const std::wstring& str);

std::string toString(Unit unit);

std::wstring toString(ProductStatus status);
ProductStatus toProductStatus(const std::wstring& str);
Unit toUnit(const std::wstring& str);

#endif