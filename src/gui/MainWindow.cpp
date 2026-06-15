#include "gui/MainWindow.h"
#include "gui/pages/ProductPage.h"
#include "gui/pages/SupplierPage.h"
#include "gui/pages/CategoryPage.h"
#include "gui/pages/ReportPage.h"

#include <QTabWidget>

MainWindow::MainWindow(
    InventoryService& inventoryService,
    ReportService& reportService,
    QWidget* parent
)
    : QMainWindow(parent),
      inventoryService(inventoryService),
      reportService(reportService)
{
    QTabWidget* tabs = new QTabWidget(this);

    tabs->addTab(new ProductPage(inventoryService, this), "Products");
    tabs->addTab(new SupplierPage(inventoryService, this), "Suppliers");
    tabs->addTab(new CategoryPage(inventoryService, this), "Categories");
    tabs->addTab(new ReportPage(reportService, this), "Reports");
    setCentralWidget(tabs);
    setWindowTitle("Inventory Management System");
    resize(1000, 700);
}