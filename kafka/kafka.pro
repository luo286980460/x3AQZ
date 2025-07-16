QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        myhttpserverkafka.cpp \
        mymainkafka.cpp \
        producer.cpp \
        producerworker.cpp \
        qaesencryption.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    myhttpserverkafka.h \
    mymainkafka.h \
    producer.h \
    producerworker.h \
    qaesencryption.h


win32: LIBS += -L$$PWD/libs/libhv/ -llibhv.dll

INCLUDEPATH += $$PWD/include/libhv
DEPENDPATH += $$PWD/include/libhv

unix:!macx: LIBS += -L$$PWD/libs/libhv/ -lhv

INCLUDEPATH += $$PWD/include/libhv
DEPENDPATH += $$PWD/include/libhv


win32: LIBS += -L$$PWD/libs/librdkafka/ -llibrdkafka++.dll

INCLUDEPATH += $$PWD/include/librdkafka
DEPENDPATH += $$PWD/include/librdkafka

unix:!macx: LIBS += -L$$PWD/libs/librdkafka/ -lrdkafka++

INCLUDEPATH += $$PWD/include/librdkafka
DEPENDPATH += $$PWD/include/librdkafka
