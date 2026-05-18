#pragma once

#include <vector>
#include "database/Database.h"
#include "models/Category.h"
#include <optional>

class CategoryRepository {
private:
    Database& database;

public:
    CategoryRepository(Database& db);

    bool addCategory(const Category& category);

    std::optional<Category> getCategoryById(int categoryId);

    std::vector<Category> getAllCategories();

    bool updateCategory(const Category& category);

    bool deactivateCategory(int categoryId);

    bool categoryExists(int categoryId);

    bool getCategoryActiveStatus(int categoryId, bool& isActive);
};