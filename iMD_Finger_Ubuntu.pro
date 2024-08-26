QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_LFLAGS += -Wl,-rpath=/usr/lib:./

SOURCES += \
    IMD/FAP20/fap20controller.cpp \
    IMD/FAP20/fap20reader.cpp \
    IMD/FAP20/fap20thread.cpp \
    IMD/FAP20/fingerprint.cpp \
    IMD/common/databasemanager.cpp \
    IMD/common/devicediscover.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    IMD/FAP20/fap20controller.h \
    IMD/FAP20/fap20reader.h \
    IMD/FAP20/fap20thread.h \
    IMD/FAP20/fingerprint.h \
    IMD/common/databasemanager.h \
    IMD/common/devicediscover.h \
    IMD/common/fingersList.h \
    IMD/common/modes.h \
    customhand.h \
    databaselineedit.h \
    mainwindow.h

FORMS += \
    mainwindow.ui


LIBS += -L$$PWD/libs/ -lfpdevice  -lfpcore -lfpnbis -lfpcore20 -lfpcore30  -lfpstate -lrt -ludev
LIBS += -ldl


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
