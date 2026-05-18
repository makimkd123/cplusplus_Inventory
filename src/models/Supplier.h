#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <string>

class Supplier {
private:
    int id;
    std::string name;
    std::string contactName;
    std::string phone;
    std::string email;
    std::string address;
    bool active;

public:
    Supplier();

    // For creating new suppliers
    Supplier(
        const std::string& name,
        const std::string& contactName,
        const std::string& phone,
        const std::string& email,
        const std::string& address
    );

    // For updating/fetching existing suppliers
    Supplier(
        int id,
        const std::string& name,
        const std::string& contactName,
        const std::string& phone,
        const std::string& email,
        const std::string& address,
        bool active
    );

    int getId() const;
    std::string getName() const;
    std::string getContactName() const;
    std::string getPhone() const;
    std::string getEmail() const;
    std::string getAddress() const;
    bool isActive() const;

    void setId(int id);
    void setName(const std::string& name);
    void setContactName(const std::string& contactName);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setAddress(const std::string& address);
    void setActive(bool active);
};

#endif