#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "services/InventoryService.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    InventoryService& inventoryService;

public:
    explicit MainWindow(
        InventoryService& inventoryService,
        QWidget* parent = nullptr
    );
};

#endif