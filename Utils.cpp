#include "Utils.h"

std::wstring toString(MovementType type) {
    switch (type) {
        case MovementType::STOCK_IN: return L"IN";
        case MovementType::STOCK_OUT: return L"OUT";
        case MovementType::ADJUSTMENT: return L"ADJUSTMENT";
        default: return L"UNKNOWN";
    }
}

std::wstring toString(MovementReason reason) {
    switch (reason) {
        case MovementReason::BOUGHT: return L"BOUGHT";
        case MovementReason::SOLD: return L"SOLD";
        case MovementReason::DAMAGED: return L"DAMAGED";
        case MovementReason::OTHER: return L"OTHER";
        default: return L"UNKNOWN";
    }
}

MovementType toMovementType(const std::wstring& str) {
    if (str == L"IN") return MovementType::STOCK_IN;
    if (str == L"OUT") return MovementType::STOCK_OUT;
    return MovementType::ADJUSTMENT;
}

MovementReason toMovementReason(const std::wstring& str) {
    if (str == L"BOUGHT") return MovementReason::BOUGHT;
    if (str == L"SOLD") return MovementReason::SOLD;
    if (str == L"DAMAGED") return MovementReason::DAMAGED;
    return MovementReason::OTHER;
}

std::string toString(Unit unit) {
    switch (unit) {
        case Unit::KG: return "KG";
        case Unit::LITER: return "L";
        case Unit::PIECES: return "PIECES";
        default: return "UNKNOWN";
    }
}

std::wstring productStatusToWString(ProductStatus status) {
    switch (status) {
        case ProductStatus::ACTIVE:
            return L"ACTIVE";
        case ProductStatus::BLOCKED:
            return L"BLOCKED";
        case ProductStatus::INACTIVE:
            return L"INACTIVE";
        default:
            return L"ACTIVE";
    }
}