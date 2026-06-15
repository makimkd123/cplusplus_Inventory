#include "gui/pages/ReportPage.h"
#include "utils/Utils.h"

#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QDateEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QAbstractItemView>
#include <QSpinBox>

ReportPage::ReportPage(
    ReportService& reportService,
    QWidget* parent
)
    : QWidget(parent),
      reportService(reportService)
{
    auto* mainLayout = new QVBoxLayout(this);

    auto* tabs = new QTabWidget(this);

    auto* lowStockTab = new QWidget(this);
    auto* inventoryValueTab = new QWidget(this);
    auto* movementTab = new QWidget(this);

    setupLowStockTab(lowStockTab);
    setupInventoryValueTab(inventoryValueTab);
    setupMovementTab(movementTab);

    tabs->addTab(lowStockTab, "Low Stock");
    tabs->addTab(inventoryValueTab, "Inventory Value");
    tabs->addTab(movementTab, "Movement History");

    mainLayout->addWidget(tabs);

    loadLowStockReport();
    loadInventoryValueReport();
    loadMovementHistoryReport();
}

void ReportPage::setupLowStockTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);

    refreshLowStockButton = new QPushButton("Refresh", tab);

    lowStockTable = new QTableWidget(tab);
    lowStockTable->verticalHeader()->setVisible(false);
    lowStockTable->setColumnCount(4);
    lowStockTable->setHorizontalHeaderLabels({
        "Product ID",
        "Name",
        "Quantity",
        "Min Quantity"
    });

    lowStockTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    lowStockTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    lowStockTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(refreshLowStockButton);
    layout->addWidget(lowStockTable);

    connect(
        refreshLowStockButton,
        &QPushButton::clicked,
        this,
        &ReportPage::onRefreshLowStockClicked
    );
}

void ReportPage::setupInventoryValueTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);

    refreshValueButton = new QPushButton("Refresh", tab);

    buyingValueLabel = new QLabel(tab);
    sellingValueLabel = new QLabel(tab);
    profitLabel = new QLabel(tab);

    auto* formLayout = new QFormLayout();
    formLayout->addRow("Total Buying Value:", buyingValueLabel);
    formLayout->addRow("Total Selling Value:", sellingValueLabel);
    formLayout->addRow("Potential Profit:", profitLabel);

    layout->addWidget(refreshValueButton);
    layout->addLayout(formLayout);
    layout->addStretch();

    connect(
        refreshValueButton,
        &QPushButton::clicked,
        this,
        &ReportPage::onRefreshValueClicked
    );
}

void ReportPage::setupMovementTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);

    auto* filterLayout = new QHBoxLayout();

    startDateEdit = new QDateEdit(QDate::currentDate().addMonths(-1), tab);
    endDateEdit = new QDateEdit(QDate::currentDate(), tab);

    startDateEdit->setCalendarPopup(true);
    endDateEdit->setCalendarPopup(true);

    productIdInput = new QSpinBox(tab);
    productIdInput->setMinimum(0);
    productIdInput->setMaximum(1000000);
    productIdInput->setSpecialValueText("All Products");

    refreshMovementButton = new QPushButton("Load Report", tab);

    filterLayout->addWidget(new QLabel("Start Date:", tab));
    filterLayout->addWidget(startDateEdit);

    filterLayout->addWidget(new QLabel("End Date:", tab));
    filterLayout->addWidget(endDateEdit);

    filterLayout->addWidget(new QLabel("Product ID:", tab));
    filterLayout->addWidget(productIdInput);

    filterLayout->addWidget(refreshMovementButton);
    filterLayout->addStretch();

    movementTable = new QTableWidget(tab);
    movementTable->verticalHeader()->setVisible(false);
    movementTable->setColumnCount(7);
    movementTable->setHorizontalHeaderLabels({
        "Movement ID",
        "Product ID",
        "Product",
        "Quantity",
        "Type",
        "Reason",
        "Date"
    });

    movementTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    movementTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    movementTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addLayout(filterLayout);
    layout->addWidget(movementTable);

    connect(
        refreshMovementButton,
        &QPushButton::clicked,
        this,
        &ReportPage::onRefreshMovementClicked
    );
}

void ReportPage::loadLowStockReport() {
    try {
        auto items = reportService.getLowStockReport();

        lowStockTable->setRowCount(static_cast<int>(items.size()));

        for (int row = 0; row < static_cast<int>(items.size()); ++row) {
            const auto& item = items[row];

            lowStockTable->setItem(
                row,
                0,
                new QTableWidgetItem(QString::number(item.productId))
            );

            lowStockTable->setItem(
                row,
                1,
                new QTableWidgetItem(QString::fromStdString(item.name))
            );

            lowStockTable->setItem(
                row,
                2,
                new QTableWidgetItem(QString::number(item.quantity))
            );

            lowStockTable->setItem(
                row,
                3,
                new QTableWidgetItem(QString::number(item.minQuantity))
            );
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Report Error", e.what());
    }
}

void ReportPage::loadInventoryValueReport() {
    try {
        auto report = reportService.getInventoryValueReport();

        buyingValueLabel->setText(
            QString::number(report.totalBuyingValue, 'f', 2)
        );

        sellingValueLabel->setText(
            QString::number(report.totalSellingValue, 'f', 2)
        );

        profitLabel->setText(
            QString::number(report.potentialProfit, 'f', 2)
        );
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Report Error", e.what());
    }
}

void ReportPage::loadMovementHistoryReport() {
    try {
        std::string startDate =
            startDateEdit->date().toString("yyyy-MM-dd").toStdString();

        std::string endDate =
            endDateEdit->date().toString("yyyy-MM-dd").toStdString();

        if (productIdInput->value() > 0) {
            movementTable->clearSelection();
            movementTable->setCurrentCell(-1, -1);
            movementTable->setRowCount(0);
            movementTable->clearContents();
            movementTable->setColumnCount(4);
            movementTable->setHorizontalHeaderLabels({
                "Quantity",
                "Type",
                "Reason",
                "Date"
            });

            auto movements =
                reportService.getProductMovementHistory(productIdInput->value());

            movementTable->setRowCount(static_cast<int>(movements.size()));

            for (int row = 0; row < static_cast<int>(movements.size()); ++row) {
                const auto& movement = movements[row];

                movementTable->setItem(
                    row,
                    0,
                    new QTableWidgetItem(QString::number(movement.getAmount()))
                );

                movementTable->setItem(
                    row,
                    1,
                    new QTableWidgetItem(QString::fromStdString(
                        toStringNarrow(movement.getType())
                    ))
                );

                movementTable->setItem(
                    row,
                    2,
                    new QTableWidgetItem(QString::fromStdString(
                        toStringNarrow(movement.getReason())
                    ))
                );

                movementTable->setItem(
                    row,
                    3,
                    new QTableWidgetItem(QString::fromStdString(
                        movement.getDate()
                    ))
                );
            }

            movementTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            return;
        }

        movementTable->clearSelection();
        movementTable->setCurrentCell(-1, -1);
        movementTable->setRowCount(0);
        movementTable->clearContents();
        movementTable->setColumnCount(7);
        movementTable->setHorizontalHeaderLabels({
            "Movement ID",
            "Product ID",
            "Product",
            "Quantity",
            "Type",
            "Reason",
            "Date"
        });

        auto items = reportService.getMovementHistoryReport(startDate, endDate);

        movementTable->setRowCount(static_cast<int>(items.size()));

        for (int row = 0; row < static_cast<int>(items.size()); ++row) {
            const auto& item = items[row];

            movementTable->setItem(
                row,
                0,
                new QTableWidgetItem(QString::number(item.movementId))
            );

            movementTable->setItem(
                row,
                1,
                new QTableWidgetItem(QString::number(item.productId))
            );

            movementTable->setItem(
                row,
                2,
                new QTableWidgetItem(QString::fromStdString(item.productName))
            );

            movementTable->setItem(
                row,
                3,
                new QTableWidgetItem(QString::number(item.quantity))
            );

            movementTable->setItem(
                row,
                4,
                new QTableWidgetItem(QString::fromStdString(item.movementType))
            );

            movementTable->setItem(
                row,
                5,
                new QTableWidgetItem(QString::fromStdString(item.reason))
            );

            movementTable->setItem(
                row,
                6,
                new QTableWidgetItem(QString::fromStdString(item.movementDate))
            );
        }

        movementTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Report Error", e.what());
    }
}

void ReportPage::onRefreshLowStockClicked() {
    loadLowStockReport();
}

void ReportPage::onRefreshValueClicked() {
    loadInventoryValueReport();
}

void ReportPage::onRefreshMovementClicked() {
    loadMovementHistoryReport();
}