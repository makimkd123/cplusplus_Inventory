#pragma once

#include <string>

struct ServiceResult {
    bool success;
    std::string message;

    static ServiceResult Success(const std::string& message) {
        return { true, message };
    }

    static ServiceResult Failure(const std::string& message) {
        return { false, message };
    }
};