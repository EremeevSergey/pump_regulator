QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    firmware/settings.cpp \
    main.cpp \
    mainwindow.cpp \
    firmware/button.cpp \
    firmware/relay.cpp \
    firmware/sensor.cpp \
    firmware/digit_port.cpp \
    firmware/rgb_led.cpp \
    firmware/system.cpp \
    firmware/timer.cpp \
    widgets/pumping_station.cpp \
    widgets/pump_widget.cpp \
    widgets/regulator_widget.cpp \
    widgets/rgb_led_widget.cpp \
    widgets/sensor_dialog.cpp \
    widgets/water_tank.cpp \
    utils.cpp \
    simulator.cpp

HEADERS += \
    firmware/button.h \
    firmware/relay.h \
    firmware/sensor.h \
    firmware/config.h \
    firmware/digit_port.h \
    firmware/hw_specific.h \
    firmware/rgb_led.h \
    firmware/settings.h \
    firmware/system.h \
    firmware/timer.h \
    mainwindow.h \
    utils.h \
    widgets/pumping_station.h \
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
