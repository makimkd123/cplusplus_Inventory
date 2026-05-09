#pragma once

#include <vector>
#include "Database.h"
#include "Category.h"

class CategoryRepository {
private:
    Database& database;

public:
    CategoryRepository(Database& db);

    void addCategory(const Category& category);

    Category getCategoryById(int categoryId);

    std::vector<Category> getAllCategories();

    void updateCategory(const Category& category);

    void deactivateCategory(int categoryId);
};