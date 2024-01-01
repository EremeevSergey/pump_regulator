#ifndef WATER_TANK_H
#define WATER_TANK_H
#include <QWidget>
#include <vector>

namespace GUI {

class CWaterTank;
struct CSensor{
    friend CWaterTank;
    constexpr static int MaxLevel = 100;
    enum Type{
        NormalOpen,
        NormalClose,
    };

    CSensor(int level,Type type=NormalOpen,bool enable=true):Level{level},Type{type},Enable{enable},State{false}{}
    CSensor(const CSensor& ) = default;
    CSensor(      CSensor&&) = default;
    CSensor& operator=(const CSensor& ) = default;
    CSensor& operator=(      CSensor&&) = default;
    int  Level;
    Type Type;
    bool Enable;

    bool getState(){if (Type==NormalOpen) return State; else return !State;}
private:
    bool State;
};


class CWaterTank : public QWidget
{
    Q_OBJECT
public:
    explicit CWaterTank(QWidget *parent = nullptr);
    CWaterTank(const CWaterTank& )=delete;
    CWaterTank(      CWaterTank&&)=delete;
    CWaterTank& operator=(const CWaterTank& )=delete;
    CWaterTank& operator=(      CWaterTank&&)=delete;
    ~CWaterTank();
    void     addSensor (CSensor sensor);
    void     addWaterDelta(int val)noexcept{AverangeLevel_=std::clamp(AverangeLevel_+val,0,CSensor::MaxLevel);}
    void     setWaterDelta(int val)noexcept{WaterDelta_ = val;}
    auto     getWaterDelta() const noexcept{return WaterDelta_;}

    bool     getSensorState(int index);
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    void     drawSensor(QPainter& painter, CSensor& sensor, int water_level);
    void     drawWater (QPainter& painter);
private:
    std::vector<CSensor> Sensors_;
    int AverangeLevel_;
    float RightWallLevel_;
    int SinTime_;
    int WaterWaveTimerId_;
    int WaterDeltaTimerId_;
    int WaterDelta_;//!< Дельта, на которую будет увеличваться/ уменьшаться вода
signals:
    void sensorChange(int level,bool state);
};

}//namespace GUI

#endif // WATER_TANK_H
