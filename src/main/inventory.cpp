#include <iostream>
#include <exception>
#include <limits>
#include <string>

#include "models/Product.h"
#include "utils/Utils.h"
#include "database/Database.h"
#include "services/InventoryService.h"
#include "repositories/ReportRepository.h"
#include "services/ReportService.h"


double readDouble(const std::string& prompt) {
    double value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            return value;
        }

        std::cout << "Invalid number. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int readInt(const std::string& prompt) {
    int value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            return value;
        }

        std::cout << "Invalid integer. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    Database db;
    db.connect();

    InventoryService inventory(db);

    ReportRepository reportRepository(db);
    ReportService reportService(reportRepository);

    int choice;

    do {
        std::cout << "\n===== Inventory Management System =====\n";
        std::cout << "1. Show Products\n";
        std::cout << "2. Show Product Details\n";
        std::cout << "3. Show Stock Movements By Product\n";
        std::cout << "4. Record Stock Movement\n";
        std::cout << "5. Add Product\n";
        std::cout << "6. Edit Product\n";
        std::cout << "7. Change Product Status\n";
        std::cout << "8. Search Products\n";
        std::cout << "9. Set Minimum Stock Level\n";

        std::cout << "10. Low Stock Report\n";
        std::cout << "11. Inventory Value Report\n";
        std::cout << "12. Movement History Report\n";

        std::cout << "13. Show Suppliers\n";
        std::cout << "14. Add Supplier\n";
        std::cout << "15. Search Suppliers\n";
        std::cout << "16. Deactivate Supplier\n";
        std::cout << "17. Update Supplier\n";

        std::cout << "18. Show Categories\n";
        std::cout << "19. Add Category\n";
        std::cout << "20. Update Category\n";
        std::cout << "21. Deactivate Category\n";

        std::cout << "22. Exit\n";

        choice = readInt("Choose an option: ");

        switch(choice) {
            case 1: {
                inventory.searchProducts("");
                break;
            }

            case 2: {
                int id;
                std::cout << "Enter product ID: ";
                std::cin >> id;
                inventory.showProductDetails(id);
                break;
            }

            case 3: {
                int id;
                std::cout << "Enter product ID: ";
                std::cin >> id;
                inventory.showStockMovements(id);
                break;
            }

            case 4: {
                int id;
                int typeChoice;
                int reasonChoice;
                double amount;

                std::cout << "Enter product ID: ";
                std::cin >> id;

                std::cout << "Movement type:\n";
                std::cout << "1. IN\n";
                std::cout << "2. OUT\n";
                std::cout << "3. ADJUSTMENT\n";
                std::cout << "Choose: ";
                std::cin >> typeChoice;

                std::cout << "Reason:\n";
                std::cout << "1. BOUGHT\n";
                std::cout << "2. SOLD\n";
                std::cout << "3. DAMAGED\n";
                std::cout << "4. OTHER\n";
                std::cout << "Choose: ";
                std::cin >> reasonChoice;

                std::cout << "Enter amount: ";
                std::cin >> amount;

                MovementType type;
                MovementReason reason;

                if (typeChoice == 1) {
                    type = MovementType::STOCK_IN;
                }
                else if (typeChoice == 2) {
                    type = MovementType::STOCK_OUT;
                }
                else if (typeChoice == 3) {
                    type = MovementType::ADJUSTMENT;
                }
                else {
                    std::cout << "Invalid movement type.\n";
                    break;
                }

                if (reasonChoice == 1) {
                    reason = MovementReason::BOUGHT;
                }
                else if (reasonChoice == 2) {
                    reason = MovementReason::SOLD;
                }
                else if (reasonChoice == 3) {
                    reason = MovementReason::DAMAGED;
                }
                else if (reasonChoice == 4) {
                    reason = MovementReason::OTHER;
                }
                else {
                    std::cout << "Invalid movement reason.\n";
                    break;
                }

                inventory.addStockMovement(id, type, reason, amount);
                break;
            }

            case 5: {
                std::string name;
                std::string barcode;
                int unitChoice;
                Unit unit;

                double buyingPrice;
                double sellingPrice;
                double tax;
                double height;
                double width;
                double depth;

                int primaryCategoryId;
                int supplierId;
                int subCategoryId;

                std::cin.ignore();

                std::cout << "Enter product name: ";
                std::getline(std::cin, name);

                std::cout << "Enter barcode: ";
                std::getline(std::cin, barcode);

                std::cout << "Unit:\n";
                std::cout << "1. KG\n";
                std::cout << "2. LITER\n";
                std::cout << "3. PIECES\n";
                std::cout << "Choose: ";
                std::cin >> unitChoice;

                if (unitChoice == 1) {
                    unit = Unit::KG;
                }
                else if (unitChoice == 2) {
                    unit = Unit::LITER;
                }
                else if (unitChoice == 3) {
                    unit = Unit::PIECES;
                }
                else {
                    std::cout << "Invalid unit.\n";
                    break;
                }

                buyingPrice = readDouble("Enter buying price: ");

                sellingPrice = readDouble("Enter selling price: ");

                tax = readDouble("Enter tax: ");

                height = readDouble("Enter height: ");

                width = readDouble("Enter width: ");

                depth = readDouble("Enter depth: ");

                primaryCategoryId = readInt("Enter primary category ID: ");

                supplierId = readInt("Enter supplier ID: ");

                subCategoryId = readInt("Enter subcategory ID (-1 for none): ");

                inventory.addProduct(
                    name,
                    barcode,
                    unit,
                    buyingPrice,
                    sellingPrice,
                    tax,
                    height,
                    width,
                    depth,
                    primaryCategoryId,
                    supplierId,
                    subCategoryId
                );

                break;
            }

            case 6: {
                int id;
                std::string name;
                std::string barcode;
                int unitChoice;
                Unit unit;

                double buyingPrice;
                double sellingPrice;
                double tax;
                double height;
                double width;
                double depth;

                int primaryCategoryId;
                int supplierId;
                int subCategoryId;

                std::cout << "Enter product ID to edit: ";
                std::cin >> id;

                std::cin.ignore();

                std::cout << "Enter new product name: ";
                std::getline(std::cin, name);

                std::cout << "Enter new barcode: ";
                std::getline(std::cin, barcode);

                std::cout << "Unit:\n";
                std::cout << "1. KG\n";
                std::cout << "2. LITER\n";
                std::cout << "3. PIECES\n";
                std::cout << "Choose: ";
                std::cin >> unitChoice;

                if (unitChoice == 1) {
                    unit = Unit::KG;
                }
                else if (unitChoice == 2) {
                    unit = Unit::LITER;
                }
                else if (unitChoice == 3) {
                    unit = Unit::PIECES;
                }
                else {
                    std::cout << "Invalid unit.\n";
                    break;
                }

                buyingPrice = readDouble("Enter buying price: ");

                sellingPrice = readDouble("Enter selling price: ");

                tax = readDouble("Enter tax: ");

                height = readDouble("Enter height: ");

                width = readDouble("Enter width: ");

                depth = readDouble("Enter depth: ");

                primaryCategoryId = readInt("Enter primary category ID: ");

                supplierId = readInt("Enter supplier ID: ");

                subCategoryId = readInt("Enter subcategory ID (-1 for none): ");

                inventory.editProduct(
                    id,
                    name,
                    barcode,
                    unit,
                    buyingPrice,
                    sellingPrice,
                    tax,
                    height,
                    width,
                    depth,
                    primaryCategoryId,
                    supplierId,
                    subCategoryId
                );

                break;
            }

            case 7: {
                int productId;
                int statusChoice;

                std::cout << "Enter product ID: ";
                std::cin >> productId;

                std::cout << "Select status:\n";
                std::cout << "1. INACTIVE\n";
                std::cout << "2. BLOCKED\n";
                std::cout << "3. ACTIVE\n";
                std::cout << "Choice: ";
                std::cin >> statusChoice;

                ProductStatus status;

                if (statusChoice == 1) {
                    status = ProductStatus::INACTIVE;
                }
                else if (statusChoice == 2) {
                    status = ProductStatus::BLOCKED;
                }
                else if (statusChoice == 3) {
                    status = ProductStatus::ACTIVE;
                }
                else {
                    std::cout << "Invalid status.\n";
                    break;
                }

                inventory.changeProductStatus(productId, status);
                break;
            }

            case 8: {
                std::string keyword;

                std::cout << "Enter name or barcode: ";
                std::cin.ignore();
                std::getline(std::cin, keyword);

                inventory.searchProducts(keyword);
                break;
            }

            case 9: {
                int productId;
                double minimumQuantity;

                std::cout << "Enter product ID: ";
                std::cin >> productId;

                std::cout << "Enter minimum stock quantity: ";
                std::cin >> minimumQuantity;

                inventory.updateMinimumQuantity(productId, minimumQuantity);
                break;
            }

            case 10: {
                reportService.showLowStockReport();
                break;
            }

            case 11: {
                reportService.showInventoryValueReport();
                break;
            }

            case 12: {
                std::string startDate;
                std::string endDate;

                std::cout << "Enter start date YYYY-MM-DD: ";
                std::cin >> startDate;

                std::cout << "Enter end date YYYY-MM-DD: ";
                std::cin >> endDate;

                reportService.showMovementHistoryReport(startDate, endDate);
                break;
            }

            case 13: {
                inventory.showSuppliers();
                break;
            }

            case 14: {
                std::string name;
                std::string contactName;
                std::string phone;
                std::string email;
                std::string address;

                std::cin.ignore();

                std::cout << "Enter supplier name: ";
                std::getline(std::cin, name);

                std::cout << "Enter contact name: ";
                std::getline(std::cin, contactName);

                std::cout << "Enter phone: ";
                std::getline(std::cin, phone);

                std::cout << "Enter email: ";
                std::getline(std::cin, email);

                std::cout << "Enter address: ";
                std::getline(std::cin, address);

                inventory.addSupplier(name, contactName, phone, email, address);
                break;
            }

            case 15: {
                std::string keyword;

                std::cin.ignore();

                std::cout << "Enter supplier search keyword: ";
                std::getline(std::cin, keyword);

                inventory.searchSuppliers(keyword);
                break;
            }

            case 16: {
                int supplierId;

                std::cout << "Enter supplier ID: ";
                std::cin >> supplierId;

                inventory.deactivateSupplier(supplierId);
                break;
            }

            case 17: {
                int supplierId;
                std::string name;
                std::string contactName;
                std::string phone;
                std::string email;
                std::string address;
                int activeChoice;

                std::cout << "Enter supplier ID: ";
                std::cin >> supplierId;

                std::cin.ignore();

                std::cout << "Enter supplier name: ";
                std::getline(std::cin, name);

                std::cout << "Enter contact name: ";
                std::getline(std::cin, contactName);

                std::cout << "Enter phone: ";
                std::getline(std::cin, phone);

                std::cout << "Enter email: ";
                std::getline(std::cin, email);

                std::cout << "Enter address: ";
                std::getline(std::cin, address);

                do {
                    std::cout << "Is supplier active? (1 = Yes, 0 = No): ";
                    std::cin >> activeChoice;
                } while (activeChoice != 0 && activeChoice != 1);

                inventory.updateSupplier(
                    supplierId,
                    name,
                    contactName,
                    phone,
                    email,
                    address,
                    activeChoice == 1
                );

                break;
            }

            case 18: {
                inventory.showCategories();
                break;
            }
            case 19: {
                std::string name;
                std::string description;

                std::cin.ignore();

                std::cout << "Enter category name: ";
                std::getline(std::cin, name);

                std::cout << "Enter category description: ";
                std::getline(std::cin, description);

                inventory.addCategory(name, description);
                break;
            }
            case 20: {
                int categoryId;
                std::string name;
                std::string description;
                int activeChoice;

                std::cout << "Enter category ID: ";
                std::cin >> categoryId;

                std::cin.ignore();

                std::cout << "Enter new category name: ";
                std::getline(std::cin, name);

                std::cout << "Enter new description: ";
                std::getline(std::cin, description);

                do {
                    std::cout << "Is category active? (1 = Yes, 0 = No): ";
                    std::cin >> activeChoice;
                } while (activeChoice != 0 && activeChoice != 1);

                inventory.updateCategory(categoryId, name, description, activeChoice == 1);
                break;
            }

            case 21: {
                int categoryId;

                std::cout << "Enter category ID: ";
                std::cin >> categoryId;

                inventory.deactivateCategory(categoryId);
                break;
            }

            case 22: {
                std::cout << "Exiting program...\n";
                break;
            }

            default: {
                std::cout << "Invalid choice\n";
                break;
            }
        }

    } while(choice != 22);

    return 0;
}