#include "Category.h"

Category::Category()
    : categoryId(0), name(L""), description(L""), isActive(true) {}

Category::Category(int categoryId,
                   const std::wstring& name,
                   const std::wstring& description,
                   bool isActive)
    : categoryId(categoryId),
      name(name),
      description(description),
      isActive(isActive) {}

int Category::getCategoryId() const {
    return categoryId;
}

std::wstring Category::getName() const {
    return name;
}

std::wstring Category::getDescription() const {
    return description;
}

bool Category::getIsActive() const {
    return isActive;
}

void Category::setCategoryId(int categoryId) {
    this->categoryId = categoryId;
}

void Category::setName(const std::wstring& name) {
    this->name = name;
}

void Category::setDescription(const std::wstring& description) {
    this->description = description;
}

void Category::setIsActive(bool isActive) {
    this->isActive = isActive;
}