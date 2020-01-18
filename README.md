# minddrawplay

The code is written in C++, Qt 5.6.0

The project is specifically made for mobile EEG device MindWave NeuroSky,

but also can work with other EEG devices (with less features) through BCI2000 module.

====

MindDrawPlay – is a project of experimental interactive audio-visual art, 
representing translation of brain waves to drawing, graphical and musical space controls. 
It has been grown on a base of a research work in the direction of Brain-Computer Interfaces. 
Combining both latest technology advances – such as mobile EEG devices and musical knowledge – 
such as pentatonic scales, it allows everyone literally to see, to hear his brain activity 
represented by set of sounds and to use brain waves as a brush for drawing, as parameters 
for image filtering or changes in a graphical flow and in a simple puzzle gathering 
and identical pictures recognition games. 

broader overview with screenshots in "about.pdf" 

Examples of running app: https://www.artstation.com/neur0forest/albums/1425498

==== hardware requirements ====

- EEG device MindWave NeuroSky

http://neurosky.com

in case of a problem with the device connection via bluetooth, try to install driver manually:

http://download.neurosky.com/public/Products/MindWave%20headset/RF%20driver/RF%20driver.zip

- full-hd resolution of the screen is required for a proper usage of the application GUI

====  software requirements ====

OpenCV libraries (see .pro file)

==== third-party code / libraries licenses: ====

qcustomplot.h / qcustomplot.cpp codes by Emanuel Eichhammer, GNU General Public License v. 3.0

appconnect.h / appconnect.cpp codes by J. Adam Wilson, BCI2000, GNU General Public License v. 3.0

sockstream.h / sockstream.cpp codes by Juergen Mellinger, GNU General Public License v. 3.0

thinkgear.h / thinkgear.lib / thinkgear.dll from official MindWave NeuroSky SDK

https://store.neurosky.com/pages/license-agreement

Eigen library, Mozilla Public License v. 2.0
