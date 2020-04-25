QT += core gui \
      widgets  \
      multimedia \
      concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = MindDrawPlay

TEMPLATE = app

CONFIG += c++11 \
          resources_big

SOURCES += src\main.cpp\
           src\mainwindow.cpp \
           src\qcustomplot.cpp \
           src\plotwindow.cpp \
    src\sockstream.cpp \
    src\appconnect.cpp \    
    src\settings.cpp \
    src\paintscene.cpp \
    src\paintform.cpp \
    src\rawsignal.cpp \
    src\soundplayer.cpp \
    src\leftpanel.cpp \
    src\rightpanel.cpp \
    src\ocvcontrols.cpp \
    src\filters.cpp

HEADERS  += include\mainwindow.h \
            include\qcustomplot.h \
            include\plotwindow.h \
    include\sockstream.h \
    include\appconnect.h \
    include\settings.h \
    include\paintscene.h \
    include\paintform.h \
    include\thinkgear.h \
    include\rawsignal.h \
    include\soundplayer.h \
    include\leftpanel.h \
    include\rightpanel.h \
    include\ocvcontrols.h \
    include\filters.h

FORMS    += ui\mainwindow.ui \
            ui\plotwindow.ui \
    ui\settings.ui \
    ui\paintform.ui \
    ui\rawsignal.ui \
    ui\leftpanel.ui \
    ui\rightpanel.ui \
    ui\ocvcontrols.ui

LIBS += -lwsock32 -lws2_32 -mthreads -L$$PWD/./ -lthinkgear

INCLUDEPATH += C:\OpenCV\OpenCV_bin\install\include

LIBS += C:\OpenCV\OpenCV_bin\bin\libopencv_core412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_highgui412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_imgcodecs412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_imgproc412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_calib3d412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_video412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_videoio412.dll \
        C:\OpenCV\OpenCV_bin\bin\libopencv_features2d412.dll

INCLUDEPATH += $$PWD/include/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    resf.qrc


