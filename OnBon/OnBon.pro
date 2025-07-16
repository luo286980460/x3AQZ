QT = core
QT += serialport

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        myhttpserveronbon.cpp \
        mymainonbon.cpp \
        qaesencryption.cpp \
        screen.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    myhttpserveronbon.h \
    mymainonbon.h \
    qaesencryption.h \
    qaesencryption.h \
    screen.h


# libhv
win32: LIBS += -L$$PWD/libs/libhv/ -llibhv.dll

INCLUDEPATH += $$PWD/include/libhv
DEPENDPATH += $$PWD/include/libhv

unix:!macx: LIBS += -L$$PWD/libs/libhv/ -lhv

INCLUDEPATH += $$PWD/include/libhv
DEPENDPATH += $$PWD/include/libhv

# onBon
win32: LIBS += -L$$PWD/libs/libonbon/ -lbx_sdk_dual

INCLUDEPATH += $$PWD/include/libonbon
DEPENDPATH += $$PWD/include/libonbon

win32: LIBS += -L$$PWD/libs/libonbon/ -lbx_sdk_dual_server

INCLUDEPATH += $$PWD/include/libonbon
DEPENDPATH += $$PWD/include/libonbon



unix:!macx: LIBS += -L$$PWD/libs/libonbon/ -lbx_sdkDual

INCLUDEPATH += $$PWD/include/libonbon
DEPENDPATH += $$PWD/include/libonbon
