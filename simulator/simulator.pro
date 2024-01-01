QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    regulator/relay.cpp \
    regulator/sensor.cpp \
    widgets/pumping_station.cpp \
    regulator/digit_port.cpp \
    regulator/rgb_led.cpp \
    regulator/system.cpp \
    regulator/timer.cpp \
    utils.cpp \
    widgets/pump_widget.cpp \
    widgets/regulator_widget.cpp \
    widgets/rgb_led_widget.cpp \
    widgets/sensor_dialog.cpp \
    simulator.cpp \
    widgets/water_tank.cpp

HEADERS += \
    mainwindow.h \
    regulator/relay.h \
    regulator/sensor.h \
    widgets/pumping_station.h \
    regulator/config.h \
    regulator/digit_port.h \
    regulator/hw_specific.h \
    regulator/rgb_led.h \
    regulator/system.h \
    regulator/timer.h \
    utils.h \
    widgets/pump_widget.h \
    widgets/regulator_widget.h \
    widgets/rgb_led_widget.h \
    widgets/sensor_dialog.h \
    simulator.h \
    widgets/water_tank.h

FORMS += \
    mainwindow.ui \
    widgets/pump_widget.ui \
    widgets/regulator_widget.ui \
    widgets/sensor_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
