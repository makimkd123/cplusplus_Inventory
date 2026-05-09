#pragma once
#include <string>

class Category {
private:
    int categoryId;
    std::wstring name;
    std::wstring description;
    bool isActive;

public:
    Category();

    Category(int categoryId,
             const std::wstring& name,
             const std::wstring& description,
             bool isActive);

    int getCategoryId() const;
    std::wstring getName() const;
    std::wstring getDescription() const;
    bool getIsActive() const;

    void setCategoryId(int categoryId);
    void setName(const std::wstring& name);
    void setDescription(const std::wstring& description);
    void setIsActive(bool isActive);


};