#include "mainwindow.h"
#include <QApplication>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include <valarray>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QDebug>
#include <QElapsedTimer>
#include <QMainWindow>
#include <QScrollArea>
#include <qcustomplot.h>
#include <plotwindow.h>
#include <hilbert.h>
#include "appconnect.h"

using namespace std;
using namespace std::chrono;

void intervalhilbert(hilbert* HT, MainWindow* w, QString filename)
{
    HT->firstinit(filename,NMAX);
    string s = "EEG channel: " + HT->channel;
    w->adddata(s,filename);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    int sr, pos, lft, delay, ampl, numst;
    double afrq, thfrq;
    sr=500; pos=0; lft=32; afrq=9.0; thfrq = 4.0; delay=2; ampl=1; numst=10;
    hilbert HT(sr,pos,lft,afrq,thfrq,delay,ampl,numst);
    // sampling rate (Hz), pos of stimulation, length of FIR filter, approx. oscill. freq, delay of phase, number of stimulations

    w.ht=&HT;    
    intervalhilbert(&HT,&w,"");

    w.setFixedSize(638,355);
    w.setWindowTitle("MindDrawPlay beta 2.24");
    w.show();

    return a.exec();
}
