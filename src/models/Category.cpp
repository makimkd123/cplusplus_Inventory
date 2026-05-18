#include "Category.h"

Category::Category()
    : categoryId(0), name(""), description(""), isActive(true) {}

Category::Category(int categoryId,
                   const std::string& name,
                   const std::string& description,
                   bool isActive)
    : categoryId(categoryId),
      name(name),
      description(description),
      isActive(isActive) {}

int Category::getCategoryId() const {
    return categoryId;
}

std::string Category::getName() const {
    return name;
}

std::string Category::getDescription() const {
    return description;
}

bool Category::getIsActive() const {
    return isActive;
}

void Category::setCategoryId(int categoryId) {
    this->categoryId = categoryId;
}

void Category::setName(const std::string& name) {
    this->name = name;
}

void Category::setDescription(const std::string& description) {
    this->description = description;
}

void Category::setIsActive(bool isActive) {
    this->isActive = isActive;
}