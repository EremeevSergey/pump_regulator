#ifndef SENSOR_DIALOG_H
#define SENSOR_DIALOG_H

#include <QDialog>
#include "water_tank.h"


namespace Ui {
class SensorDialog;
}

namespace GUI {

class CSensorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSensorDialog(QWidget *parent = nullptr);
    ~CSensorDialog();

    QDialog::DialogCode execute(CSensor& sensor);
private slots:
    void on_cbEnable_stateChanged(int arg1);

private:
    Ui::SensorDialog *ui;
    void updateUi();
};

}//namespace GUI
#endif // SENSOR_DIALOG_H
