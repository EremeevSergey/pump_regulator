QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../firmware/src/system/

SOURCES += \
    ../firmware/src/system/button.cpp \
    ../firmware/src/system/digit_port.cpp \
    ../firmware/src/system/relay.cpp \
    ../firmware/src/system/rgb_led.cpp \
    ../firmware/src/system/sensor.cpp \
    ../firmware/src/system/settings.cpp \
    ../firmware/src/system/system.cpp \
    ../firmware/src/system/timer.cpp \
    main.cpp \
    mainwindow.cpp \
    widgets/pumping_station.cpp \
    widgets/pump_widget.cpp \
    widgets/regulator_widget.cpp \
    widgets/rgb_led_widget.cpp \
    widgets/sensor_dialog.cpp \
    widgets/veeprom.cpp \
    widgets/water_tank.cpp \
    utils.cpp \
    simulator.cpp

HEADERS += \
    ../firmware/src/system/button.h \
    ../firmware/src/system/config.h \
    ../firmware/src/system/digit_port.h \
    ../firmware/src/system/hw_specific.h \
    ../firmware/src/system/relay.h \
    ../firmware/src/system/rgb_led.h \
    ../firmware/src/system/sensor.h \
    ../firmware/src/system/settings.h \
    ../firmware/src/system/system.h \
    ../firmware/src/system/timer.h \
    mainwindow.h \
    utils.h \
    widgets/pumping_station.h \
    widgets/pump_widget.h \
    widgets/regulator_widget.h \
    widgets/rgb_led_widget.h \
    widgets/sensor_dialog.h \
    simulator.h \
    widgets/veeprom.h \
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
