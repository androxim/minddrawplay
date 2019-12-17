QT += core gui \
      widgets  \
      multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ClosedLoop
TEMPLATE = app

CONFIG += c++11 \
          resources_big

SOURCES += main.cpp\
           mainwindow.cpp \
           qcustomplot.cpp \
           plotwindow.cpp \
    hilbert.cpp \      
    sockstream.cpp \
    appconnect.cpp \    
    settings.cpp \
    paintscene.cpp \
    paintform.cpp \
    rawsignal.cpp

HEADERS  += mainwindow.h \
            qcustomplot.h \
            plotwindow.h \
    hilbert.h \        
    sockstream.h \
    appconnect.h \
    settings.h \
    paintscene.h \
    paintform.h \
    thinkgear.h \
    rawsignal.h

FORMS    += mainwindow.ui \
            plotwindow.ui \
    settings.ui \
    paintform.ui \
    rawsignal.ui

LIBS += -lwsock32 -lws2_32 -mthreads -L$$PWD/./ -lthinkgear

INCLUDEPATH += C:\OpenCV\OpenCV_bin\install\include

LIBS += C:\OpenCV\OpenCV_bin\bin\libopencv_core412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_highgui412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_imgcodecs412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_imgproc412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_calib3d412.dll



INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    resf.qrc


