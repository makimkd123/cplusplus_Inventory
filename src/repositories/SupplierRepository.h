#ifndef SUPPLIER_REPOSITORY_H
#define SUPPLIER_REPOSITORY_H

#include "database/Database.h"
#include "models/Supplier.h"
#include <vector>

class SupplierRepository {
private:
    Database& database;

public:
    explicit SupplierRepository(Database& database);

    bool insertSupplier(const Supplier& supplier);

    std::vector<Supplier> searchSuppliers(const std::string& keyword);

    bool supplierExists(int supplierId);

    bool deactivateSupplier(int supplierId);

    std::vector<Supplier> getAllSuppliers();

    bool getSupplierActiveStatus(int supplierId, bool& isActive);

    bool updateSupplier(const Supplier& supplier);
};

#endif