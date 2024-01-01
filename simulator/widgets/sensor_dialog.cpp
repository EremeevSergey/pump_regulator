#include "sensor_dialog.h"
#include "ui_sensor_dialog.h"

namespace GUI {

CSensorDialog::CSensorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SensorDialog)
{
    ui->setupUi(this);
}

CSensorDialog::~CSensorDialog()
{
    delete ui;
}


QDialog::DialogCode CSensorDialog::execute(CSensor& sensor)
{
    if (sensor.Enable) ui->cbEnable->setCheckState(Qt::Checked);
    else  ui->cbEnable->setCheckState(Qt::Unchecked);
    ui->spinBox->setMaximum(CSensor::MaxLevel);
    ui->spinBox->setMinimum(0);
    ui->spinBox->setValue(sensor.Level);
    ui->rbClose->setChecked(sensor.Type==CSensor::NormalClose);
    ui->rbOpen ->setChecked(sensor.Type==CSensor::NormalOpen);
    updateUi();
    if (exec()==QDialog::Accepted){
        sensor.Enable = ui->cbEnable->isChecked();
        if (ui->rbOpen->isChecked()) sensor.Type=CSensor::NormalOpen;
        else                         sensor.Type=CSensor::NormalClose;
        sensor.Level = ui->spinBox->value();
        return QDialog::Accepted;
    }
    return QDialog::Rejected;
}

void CSensorDialog::on_cbEnable_stateChanged(int )
{
    updateUi();
}

void CSensorDialog::updateUi()
{
    bool enable = ui->cbEnable->isChecked();
    ui->spinBox->setEnabled(enable);
    ui->rbClose->setEnabled(enable);
    ui->rbOpen->setEnabled(enable);
}

}//namespace GUI

