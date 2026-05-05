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
        std::cout << "\n===== Inventory Menu =====\n";
        std::cout << "1. Show products\n";
        std::cout << "2. Show product details\n";
        std::cout << "3. Show stock movements\n";
        std::cout << "4. Add stock movement\n";
        std::cout << "5. Add product\n";
        std::cout << "6. Edit product\n";
        std::cout << "7. Deactive/delete product\n";
        std::cout << "8. Search\n";
        std::cout << "";
        std::cout << "";
        std::cout << "15. Exit\n";
        std::cout << "Choose: ";
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
            case 8: {
                std::string keyword;

                std::cout << "Enter name or barcode: ";
                std::cin.ignore();
                std::getline(std::cin, keyword);

                inventory.searchProducts(keyword);
                break;
            }
            case 15:{
                std::cout << "Exiting...\n";
                break;
            }
            default:{
                std::cout << "Invalid choice\n";
                break;}
        }

    } while(choice != 15);

    return 0;
}