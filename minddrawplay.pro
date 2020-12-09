QT += core gui \
      widgets  \
      multimedia \
      concurrent \
      charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = MindDrawPlay

TEMPLATE = app

CONFIG += c++11 \
          resources_big

QMAKE_CXXFLAGS_RELEASE += -mavx

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
    src\filters.cpp \
    src\myitem.cpp \
    src\brainlevels.cpp \
	src\statistics.cpp \
    oscpack\IpEndpointName.cpp \
    oscpack\NetworkingUtils.cpp \
    oscpack\UdpSocket.cpp \
    oscpack\OscOutboundPacketStream.cpp \
    oscpack\OscTypes.cpp

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
    include\filters.h \
    include\myitem.h \
    include\brainlevels.h \
    include\statistics.h \
    oscpack\IpEndpointName.h \
    oscpack\NetworkingUtils.h \
    oscpack\OscOutboundPacketStream.h \
    oscpack\UdpSocket.h \
    oscpack\OscException.h \
    oscpack\OscTypes.h \
    oscpack\OscHostEndianness.h

FORMS    += ui\mainwindow.ui \
            ui\plotwindow.ui \
    ui\settings.ui \
    ui\paintform.ui \
    ui\rawsignal.ui \
    ui\leftpanel.ui \
    ui\rightpanel.ui \
    ui\ocvcontrols.ui \
    ui\brainlevels.ui \
    ui\statistics.ui

LIBS += -lwsock32 -lws2_32 -lwinmm -mthreads -L$$PWD/./ -lthinkgear \
        -LC:\SFML-2.5.1\build\lib -lsfml-audio

INCLUDEPATH += C:\opencv\build\include \
               $$PWD/include/. \
               $$PWD/oscpack/. \
               C:\SFML-2.5.1\include

LIBS += C:\opencv\build\bin\libopencv_core412.dll \
        C:\opencv\build\bin\libopencv_highgui412.dll \
        C:\opencv\build\bin\libopencv_imgcodecs412.dll \
        C:\opencv\build\bin\libopencv_imgproc412.dll \
        C:\opencv\build\bin\libopencv_calib3d412.dll \
        C:\opencv\build\bin\libopencv_video412.dll \
        C:\opencv\build\bin\libopencv_videoio412.dll \
        C:\opencv\build\bin\libopencv_features2d412.dll

DEPENDPATH += $$PWD/. C:\SFML-2.5.1\include

RC_ICONS = mdp_icon.ico

RESOURCES += \
    resf.qrc
