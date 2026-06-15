#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include <QFile>


#include "database/Database.h"
#include "services/InventoryService.h"
#include "services/ReportService.h"
#include "gui/MainWindow.h"
#include "repositories/ReportRepository.h"
#include "repositories/StockMovementRepository.h"



int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    QFile styleFile(":/styles/app.qss");

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QString::fromUtf8(styleFile.readAll());
        app.setStyleSheet(style);
    }

    Database db;

    db.connect("../config.ini");

    InventoryService inventoryService(db);

    ReportRepository reportRepository(db);

    StockMovementRepository stockMovementRepository(db);

    ReportService reportService(
        reportRepository,
        stockMovementRepository
    );

    MainWindow window(
        inventoryService,
        reportService
    );

    window.show();

    return app.exec();
}