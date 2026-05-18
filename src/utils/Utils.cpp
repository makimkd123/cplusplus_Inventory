#include "Utils.h"

std::wstring toString(MovementType type) {
    switch (type) {
        case MovementType::STOCK_IN:
            return L"IN";

        case MovementType::STOCK_OUT:
            return L"OUT";

        case MovementType::ADJUSTMENT:
            return L"ADJUSTMENT";
    }

    throw std::invalid_argument("Invalid movement type enum value");
}

std::wstring toString(MovementReason reason) {
    switch (reason) {
        case MovementReason::BOUGHT:
            return L"BOUGHT";

        case MovementReason::SOLD:
            return L"SOLD";

        case MovementReason::DAMAGED:
            return L"DAMAGED";

        case MovementReason::OTHER:
            return L"OTHER";
    }

    throw std::invalid_argument("Invalid movement reason enum value");
}

MovementType toMovementType(const std::wstring& str) {
    if (str == L"IN")
        return MovementType::STOCK_IN;

    if (str == L"OUT")
        return MovementType::STOCK_OUT;

    if (str == L"ADJUSTMENT")
        return MovementType::ADJUSTMENT;

    throw std::invalid_argument("Invalid movement type string");
}

MovementReason toMovementReason(const std::wstring& str) {
    if (str == L"BOUGHT")
        return MovementReason::BOUGHT;

    if (str == L"SOLD")
        return MovementReason::SOLD;

    if (str == L"DAMAGED")
        return MovementReason::DAMAGED;

    if (str == L"OTHER")
        return MovementReason::OTHER;

    throw std::invalid_argument("Invalid movement reason string");
}

std::string toString(Unit unit) {
    switch (unit) {
        case Unit::KG:
            return "KG";

        case Unit::LITER:
            return "L";

        case Unit::PIECES:
            return "PIECES";
    }

    throw std::invalid_argument("Invalid unit enum value");
}

std::wstring toString(ProductStatus status) {
    switch (status) {
        case ProductStatus::ACTIVE:
            return L"ACTIVE";

        case ProductStatus::BLOCKED:
            return L"BLOCKED";

        case ProductStatus::INACTIVE:
            return L"INACTIVE";
    }

    throw std::invalid_argument("Invalid product status enum value");
}

ProductStatus toProductStatus(const std::wstring& str) {
    if (str == L"ACTIVE")
        return ProductStatus::ACTIVE;

    if (str == L"BLOCKED")
        return ProductStatus::BLOCKED;

    if (str == L"INACTIVE")
        return ProductStatus::INACTIVE;

    throw std::invalid_argument("Invalid product status string");
}

Unit toUnit(const std::wstring& str) {
    if (str == L"KG")
        return Unit::KG;

    if (str == L"L")
        return Unit::LITER;

    if (str == L"PIECES")
        return Unit::PIECES;

    throw std::invalid_argument("Invalid unit string");
}

std::string toStringNarrow(MovementType type) {
    switch (type) {
        case MovementType::STOCK_IN:   return "IN";
        case MovementType::STOCK_OUT:  return "OUT";
        case MovementType::ADJUSTMENT: return "ADJUSTMENT";
    }
    throw std::invalid_argument("Invalid movement type");
}

std::string toStringNarrow(MovementReason reason) {
    switch (reason) {
        case MovementReason::BOUGHT:  return "BOUGHT";
        case MovementReason::SOLD:    return "SOLD";
        case MovementReason::DAMAGED: return "DAMAGED";
        case MovementReason::OTHER:   return "OTHER";
    }
    throw std::invalid_argument("Invalid movement reason");
}