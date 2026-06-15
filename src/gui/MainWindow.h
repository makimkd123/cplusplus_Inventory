#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "services/InventoryService.h"
#include "services/ReportService.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    InventoryService& inventoryService;
    ReportService& reportService;

public:
    explicit MainWindow(
        InventoryService& inventoryService,
        ReportService& reportService,
        QWidget* parent = nullptr
    );
};

#endif