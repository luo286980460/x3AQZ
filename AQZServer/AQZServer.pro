QT = core
QT += serialport network
CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        detectionmode.cpp \
        gps.cpp \
        i2cmanager.cpp \
        i2cmanagerworker.cpp \
        main.cpp \
        myhttpserver.cpp \
        mymain.cpp \
        qaesencryption.cpp \
        radar.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    detectionmode.h \
    gps.h \
    i2cmanager.h \
    i2cmanagerworker.h \
    myhttpserver.h \
    mymain.h \
    qaesencryption.h \
    radar.h


win32: LIBS += -L$$PWD/libs/libhv/ -llibhv.dll

INCLUDEPATH += $$PWD/include/libhv
DEPENDPATH += $$PWD/include/libhv

unix:!macx: LIBS += -L$$PWD/libs/libhv/ -lhv

INCLUDEPATH += $$PWD/include/libhv
DEPENDPATH += $$PWD/include/libhv


LIBS += -li2c
