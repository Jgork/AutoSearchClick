QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += $$PWD/counter.o
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

INCLUDEPATH += "C:\OpenCV\OpenCV-MinGW-Build-OpenCV-4.5.5-x64/include"
LIBS += -L"C:\OpenCV\OpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib" \
        -lopencv_core455 \
        -lopencv_imgproc455 \
        -lopencv_highgui455 \
        -lopencv_imgcodecs455

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
