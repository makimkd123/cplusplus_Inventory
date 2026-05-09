#include "Supplier.h"
#include <stdexcept>

Supplier::Supplier()
    : id(0),
      name(""),
      contactName(""),
      phone(""),
      email(""),
      address(""),
      active(true)
{
}
Supplier::Supplier(
    const std::string& name,
    const std::string& contactName,
    const std::string& phone,
    const std::string& email,
    const std::string& address
)
    : id(0),
      name(name),
      contactName(contactName),
      phone(phone),
      email(email),
      address(address),
      active(true)
{
}

Supplier::Supplier(
    int id,
    const std::string& name,
    const std::string& contactName,
    const std::string& phone,
    const std::string& email,
    const std::string& address,
    bool active
)
    : id(id),
      name(name),
      contactName(contactName),
      phone(phone),
      email(email),
      address(address),
      active(active)
{
}

int Supplier::getId() const {
    return id;
}

std::string Supplier::getName() const {
    return name;
}

std::string Supplier::getContactName() const {
    return contactName;
}

std::string Supplier::getPhone() const {
    return phone;
}

std::string Supplier::getEmail() const {
    return email;
}

std::string Supplier::getAddress() const {
    return address;
}

bool Supplier::isActive() const {
    return active;
}

void Supplier::setId(int id) {
    this->id = id;
}

void Supplier::setName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Supplier name cannot be empty");
    }

    this->name = name;
}

void Supplier::setContactName(const std::string& contactName) {
    this->contactName = contactName;
}

void Supplier::setPhone(const std::string& phone) {
    this->phone = phone;
}

void Supplier::setEmail(const std::string& email) {
    this->email = email;
}

void Supplier::setAddress(const std::string& address) {
    this->address = address;
}

void Supplier::setActive(bool active) {
    this->active = active;
}