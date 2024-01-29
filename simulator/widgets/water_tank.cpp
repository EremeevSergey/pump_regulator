#include <algorithm>
#include <QPainter>
#include <QTimerEvent>
#include <QMouseEvent>
#include <cmath>
#include "sensor_dialog.h"

namespace GUI {

constexpr int WaterWaveTimerPeriod  = 100; //!< Таймер, по которому синус волны движется
constexpr int WaterDeltaTimerPeriod = 500; //!< Таймер по которому изменяется уровень воды

constexpr float WaterWaveAmplitude = 2;//!< Амплитуда (в %) волны
constexpr int   WaterWaveCount     = 2;//!< Кол-во волн (горбов) на ширину бака

CWaterTank::CWaterTank(QWidget *parent)
    : QWidget{parent}
    , Sensors_{}
    , AverangeLevel_{0}
    , RightWallLevel_{static_cast<float>(AverangeLevel_)}
    , SinTime_{0}
    , WaterWaveTimerId_{0}
    , WaterDeltaTimerId_{0}
    , WaterDelta_{WaterDeltaValue}
    , reverseSensors_{false}
{
    WaterWaveTimerId_=startTimer(WaterWaveTimerPeriod);
    WaterDeltaTimerId_=startTimer(WaterDeltaTimerPeriod);
}

CWaterTank::~CWaterTank()
{
    killTimer(WaterDeltaTimerId_);
    killTimer(WaterWaveTimerId_);
}

void CWaterTank::addSensor (CSensor sensor)
{
    if (std::find_if(Sensors_.begin(),Sensors_.end(),[&sensor](auto& s){
                     return s.Level==sensor.Level;})==Sensors_.end())
        Sensors_.push_back(sensor);
}

bool CWaterTank::getSensorState(int index)
{
    auto count = static_cast<int>(Sensors_.size());
    if (index>=0 && index<count){
        if (reverseSensors_) return Sensors_.at(count - index-1).getState();
        return Sensors_.at(index).getState();
    }
    return false;
}

/*

 |              |
 |              |
 |             <-
 |              |
 |              |
 |              |
 |             <-
 |              |
 +--------------+
*/
void CWaterTank::paintEvent(QPaintEvent*)
{
    constexpr int TankWallThickness = 3;

    QPainter painter(this);

    painter.fillRect(rect(),QBrush(Qt::white));
    drawWater(painter);

    painter.save();{
        QPen pen(Qt::black);
        pen.setWidth(TankWallThickness);
        painter.setPen(pen);
        painter.drawLine(QPoint(0+TankWallThickness/2,0+TankWallThickness/2),
                         QPoint(0+TankWallThickness/2,rect().height()-TankWallThickness/2));
        painter.drawLine(QPoint(0+TankWallThickness/2,rect().height()-TankWallThickness/2),
                         QPoint(rect().width()-TankWallThickness/2,rect().height()-TankWallThickness/2));
        painter.drawLine(QPoint(rect().width()-TankWallThickness/2,rect().height()-TankWallThickness/2),
                         QPoint(rect().width()-TankWallThickness/2,0+TankWallThickness/2));
    }painter.restore();

    int index=0;
    if (reverseSensors_){
        std::for_each(Sensors_.rbegin(),Sensors_.rend(),[&index,&painter,this](auto& sens){
            drawSensor(painter,sens,index++,RightWallLevel_+0.5);
        });
    }
    else{
        for (auto& sens:Sensors_) drawSensor(painter,sens,index++,RightWallLevel_+0.5);
    }
}

void CWaterTank::drawWater (QPainter& painter)
{
    QPen pen("#4ff198");
    painter.save();{
        painter.setPen(pen);
        const float step = WaterWaveCount*360.0/rect().width();
        for (int i=0,n=rect().width();i<n;++i){
            RightWallLevel_ = AverangeLevel_ +
                    WaterWaveAmplitude*CSensor::MaxLevel/100.0 * std::sin((step*i+SinTime_)*2*M_PI/360.0);
            float level = RightWallLevel_*rect().height()/CSensor::MaxLevel;
            QPoint p{i,std::max(0,static_cast<int>(rect().height()-level))};
            painter.drawLine(p,QPoint(i,rect().height()-1));
        }
    }painter.restore();
}

namespace{
constexpr int SensorLength = 15;//%
}

void CWaterTank::drawSensor(QPainter& painter,CSensor& sensor,int index, int water_level)
{
    constexpr int SensorThickness = 5;

    int height = sensor.Level*rect().height()/CSensor::MaxLevel;
    int length = SensorLength*rect().width()/100;
    bool state = false;
    painter.save();{
        QPen pen(Qt::green);
        pen.setWidth(SensorThickness);
        if (sensor.Enable && water_level>=sensor.Level) {pen.setColor(Qt::red); state= true;}
        else if (sensor.Type==CSensor::NormalClose)     pen.setColor(Qt::blue);
        painter.setPen(pen);
        painter.drawLine(QPoint(rect().width()-length,rect().height()-height),
                         QPoint(rect().width()       ,rect().height()-height));
        painter.setPen(Qt::black);
        painter.drawText(QPoint(rect().width()-length,rect().height()-height-10),QString::number(index));
    }painter.restore();
    if (state!=sensor.State){
        emit sensorChange(sensor.Level,state);
        sensor.State = state;
    }
}

void CWaterTank::timerEvent(QTimerEvent *event)
{
    if (event!=nullptr){
        bool f_update{false};
        if (event->timerId()==WaterWaveTimerId_){
            constexpr int SinStep  = 10;//градусов
            SinTime_=(SinTime_+SinStep)%360;
            f_update=true;
        }
        else if (event->timerId()==WaterDeltaTimerId_){
            AverangeLevel_ = std::clamp(AverangeLevel_+WaterDelta_,0,CSensor::MaxLevel);
            f_update=true;
        }
        if (f_update) update();
    }
}

void CWaterTank::mouseDoubleClickEvent(QMouseEvent *event)
{
    constexpr int RectIncreaseSize = 10;
    if (event->button()==Qt::LeftButton){
        for (auto& sens:Sensors_){
            int y = rect().height() - sens.Level*rect().height()/CSensor::MaxLevel;
            int x = rect().width() - SensorLength*rect().width()/100;
            QRect rect{x-RectIncreaseSize,y-RectIncreaseSize,
                        x+SensorLength+RectIncreaseSize*2,RectIncreaseSize*2};
            if (rect.contains(event->pos())){
                CSensorDialog dlg;
                if (dlg.execute(sens)==QDialog::Accepted) break;
            }
        }
    }
}

}//namespace GUI
