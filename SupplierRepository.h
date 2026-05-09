#ifndef SUPPLIER_REPOSITORY_H
#define SUPPLIER_REPOSITORY_H

#include "Database.h"
#include "Supplier.h"

class SupplierRepository {
private:
    Database& database;

public:
    explicit SupplierRepository(Database& database);

    void insertSupplier(const Supplier& supplier);

    void searchSupplier(const std::string& keyword);

    bool supplierExists(int supplierId);

    void deactivateSupplier(int supplierId);

    void printAllSuppliers();

    bool getSupplierActiveStatus(int supplierId, bool& isActive);

    void updateSupplier(const Supplier& supplier);
};

#endif