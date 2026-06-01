#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include "database/Database.h"
#include "services/InventoryService.h"
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    std::cout << "Program started\n";

    QApplication app(argc, argv);
    std::cout << "QApplication created\n";

    Database db;
    std::cout << "Database object created\n";

    db.connect("../config.ini");
    std::cout << "Database connected\n";

    InventoryService inventoryService(db);
    std::cout << "InventoryService created\n";

    MainWindow window(inventoryService);
    std::cout << "MainWindow created\n";

    window.show();
    std::cout << "Window shown\n";

    return app.exec();
}