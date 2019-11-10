#ifndef HILBERT_H
#define HILBERT_H

#include <QMainWindow>
#include <QWidget>
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
#include <QScrollArea>
#include <qcustomplot.h>

#define NMAX 15360  // 15360 5 min for 512 s.r.
#define PMAX 32000  // 61440 // 2 min for 512 s.r.
#define LMAX  10000
#define LMFILT 1024	/* Finite Impulse Response (FIR) filter length - must be even! */ // ??? choice and influence of LFILT

using namespace std;

const double PI = 3.141592653589793238460;
const double ToDeg = 57.2958;

typedef complex<double> Complex;
typedef valarray<Complex> CArray;

class hilbert
{
public:    

    int npt, lfilt, pos, posstim, srfr ,delay, initdelay, noise, stlength, imlength, stampl, sh, numst;
    double osfr, thfr, avg1, avg2, defosfr, phsdif, mintheta, maxtheta, mind, maxd, diff, extrphase;
    double totalprocpred, totalprocregr, totalproczc, imstprop, totaldiffHR, totaldiffZC, totaldiffHRonAR, optphasediff;
    bool twooscil, inphase;

    float *time, *x;
    float xh[LMAX + 1], ampl1[LMAX + 1], phase1[LMAX + 1], ampl2[LMAX + 1], phase2[LMAX + 1], omega1[LMAX + 1], omega2[LMAX + 1], intphs[LMAX+1];
    float hilb[LMFILT + 1], xt, xht;

    string channel;
    QString filep;
    Complex t[LMAX];
    CArray data;

    hilbert(int srfr, int pstim, int lft, double osfr, double thfr, int delay, int ampl, int numst);
    hilbert(const hilbert &obj);
    void init();   
    void firstinit(QString filepath, int end);
    void readfromfile();

    void convol(float *source, float *target, float *filt, int npt, int lfilt);
    void firhilbert();

    void fft(CArray& x);
    void ifft(CArray& x);
    void matlabhilbert();

	void averagefreq();      

};

#endif // HILBERT_H
