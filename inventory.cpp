#include <iostream>
#include <exception>
#include "Product.h"
#include "Utils.h"
#include "Database.h"
#include "InventoryService.h"

int main() {
    Database db;
    db.connect();

    InventoryService inventory(db);

    int choice;

    do {

        std::cout << "\n===== Inventory Management System =====\n";
        std::cout << "1. Show Products\n";
        std::cout << "2. Show Product Details\n";
        std::cout << "3. Show Stock Movements\n";
        std::cout << "4. Record Stock Movement\n";
        std::cout << "5. Add Product\n";
        std::cout << "6. Edit Product\n";
        std::cout << "7. Change Product Status\n";
        std::cout << "8. Search Products\n";
        std::cout << "9. Set Minimum Stock Level\n";
        std::cout << "10. Show Low Stock Products\n";

        std::cout << "11. Show Suppliers\n";
        std::cout << "12. Add Supplier\n";
        std::cout << "13. Search Suppliers\n";
        std::cout << "14. Deactivate Supplier\n";
        std::cout << "15. Update Supplier\n";

        std::cout << "16. Show Categories\n";
        std::cout << "17. Add Category\n";
        std::cout << "18. Update Category\n";
        std::cout << "19. Deactivate Category\n";

        std::cout << "20. Exit\n";

        std::cout << "Choose an option: ";

        std::cin >> choice;

        switch(choice) {
            case 1:{
                inventory.showProducts();
                break;
            }
            case 2:{
                int id;
                std::cout << "Enter product ID: ";
                std::cin>> id;
                inventory.showProductDetails(id);
                break;
            }
            case 3:{
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

                std::cout << "Enter buying price: ";
                std::cin >> buyingPrice;

                std::cout << "Enter selling price: ";
                std::cin >> sellingPrice;

                inventory.addProduct(name, barcode, unit, buyingPrice, sellingPrice);
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

                std::cout << "Enter new buying price: ";
                std::cin >> buyingPrice;

                std::cout << "Enter new selling price: ";
                std::cin >> sellingPrice;

                inventory.editProduct(id, name, barcode, unit, buyingPrice, sellingPrice);
                break;
            }
            case 7: {
                int productId;
                int choice;

                std::cout << "Enter product ID: ";
                std::cin >> productId;

                std::cout << "Select status:\n";
                std::cout << "1. INACTIVE (cannot be bought)\n";
                std::cout << "2. BLOCKED (cannot be bought or sold)\n";
                std::cout << "3. ACTIVE\n";
                std::cout << "Choice: ";
                std::cin >> choice;

                ProductStatus status;

                if (choice == 1) {
                    status = ProductStatus::INACTIVE;
                } else if (choice == 2) {
                    status = ProductStatus::BLOCKED;
                } else {
                    status = ProductStatus::ACTIVE;
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
                inventory.showLowStockProducts();
                break;
            }
            case 11: {
                inventory.showSuppliers();
                break;
            }
            case 12: {
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

            case 13: {
                std::string keyword;

                std::cin.ignore();

                std::cout << "Enter supplier search keyword: ";
                std::getline(std::cin, keyword);

                inventory.searchSuppliers(keyword);
                break;
            }

            case 14: {
                int supplierId;

                std::cout << "Enter supplier ID: ";
                std::cin >> supplierId;

                inventory.deactivateSupplier(supplierId);
                break;
            }
        case 15: {
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
                std::wcout << L"Is supplier active? (1 = Yes, 0 = No): ";
                std::wcin >> activeChoice;

                if (activeChoice != 0 && activeChoice != 1) {
                    std::wcout << L"Invalid choice. Please enter 1 or 0.\n";
                }

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

        case 16: {
            inventory.showCategories();
            break;
        }

        case 17: {
            std::wstring name;
            std::wstring description;

            std::wcin.ignore();

            std::wcout << L"Enter category name: ";
            std::getline(std::wcin, name);

            std::wcout << L"Enter category description: ";
            std::getline(std::wcin, description);

            inventory.addCategory(name, description);

            break;
        }

        case 18: {
            int categoryId;
            std::wstring name;
            std::wstring description;
            int activeChoice;

            std::wcout << L"Enter category ID: ";
            std::wcin >> categoryId;

            std::wcin.ignore();

            std::wcout << L"Enter new category name: ";
            std::getline(std::wcin, name);

            std::wcout << L"Enter new description: ";
            std::getline(std::wcin, description);

            do {
                std::wcout << L"Is category active? (1 = Yes, 0 = No): ";
                std::wcin >> activeChoice;

                if (activeChoice != 0 && activeChoice != 1) {
                    std::wcout << L"Invalid choice. Please enter 1 or 0.\n";
                }

            } while (activeChoice != 0 && activeChoice != 1);

            inventory.updateCategory(
                categoryId,
                name,
                description,
                activeChoice == 1
                );

            break;
        }

        case 19: {
            int categoryId;

            std::cout << "Enter category ID: ";
            std::cin >> categoryId;

            inventory.deactivateCategory(categoryId);

            break;
        }

        case 20: {
            std::cout << "Exiting program...\n";
            break;
        }
            default:{
                std::cout << "Invalid choice\n";
                break;}
        }

    } while(choice != 20);

    return 0;
}