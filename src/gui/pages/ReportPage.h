#ifndef REPORTPAGE_H
#define REPORTPAGE_H

#include <QWidget>

#include "services/ReportService.h"

class QTableWidget;
class QPushButton;
class QDateEdit;
class QLabel;
class QSpinBox;


class ReportPage : public QWidget {
    Q_OBJECT

private:
    ReportService& reportService;

    QTableWidget* lowStockTable;
    QTableWidget* movementTable;

    QLabel* buyingValueLabel;
    QLabel* sellingValueLabel;
    QLabel* profitLabel;

    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;

    QPushButton* refreshLowStockButton;
    QPushButton* refreshValueButton;
    QPushButton* refreshMovementButton;

    QSpinBox* productIdInput;

    void setupLowStockTab(QWidget* tab);
    void setupInventoryValueTab(QWidget* tab);
    void setupMovementTab(QWidget* tab);

    void loadLowStockReport();
    void loadInventoryValueReport();
    void loadMovementHistoryReport();

private slots:
    void onRefreshLowStockClicked();
    void onRefreshValueClicked();
    void onRefreshMovementClicked();

public:
    explicit ReportPage(
        ReportService& reportService,
        QWidget* parent = nullptr
    );
};

#endif