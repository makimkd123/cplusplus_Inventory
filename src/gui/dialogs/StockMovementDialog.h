#ifndef STOCKMOVEMENTDIALOG_H
#define STOCKMOVEMENTDIALOG_H

#include <QDialog>

#include "models/StockMovement.h"

class QComboBox;
class QDoubleSpinBox;

class StockMovementDialog : public QDialog {
    Q_OBJECT

public:
    explicit StockMovementDialog(
        MovementType movementType,
        QWidget* parent = nullptr
    );

    MovementReason selectedReason() const;
    double quantity() const;

private:
    MovementType movementType;

    QComboBox* reasonComboBox;
    QDoubleSpinBox* quantityInput;
};

#endif