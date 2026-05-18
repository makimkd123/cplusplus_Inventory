#pragma once
#include <string>

class Category {
private:
    int categoryId;
    std::string name;
    std::string description;
    bool isActive;

public:
    Category();

    Category(int categoryId,
             const std::string& name,
             const std::string& description,
             bool isActive);

    int getCategoryId() const;
    std::string getName() const;
    std::string getDescription() const;
    bool getIsActive() const;

    void setCategoryId(int categoryId);
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setIsActive(bool isActive);


};