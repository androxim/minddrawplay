#include "hilbert.h"
#include "mainwindow.h"

// Hilbert-transform - adapted from implementation by Joe Mietus, http://www.physionet.org/physiotools/apdet/apdet-1.0/ht.c

hilbert::hilbert(int srfr, int pstim, int lft, double osfr, double thfr, int delay, int ampl, int numst)
{  
    this->srfr = srfr;
    this->osfr = osfr;
    this->thfr = thfr;
    this->delay = delay;
    this->numst = numst;
    x=new float[NMAX];
    noise = 0;   
    sh=0;    
    phsdif = 100000;
    defosfr = osfr;
    extrphase = 0;
    posstim = pstim;    
    imstprop = 1;
    imlength = srfr/2;
    stlength = imlength * imstprop;
    stampl = ampl;
    lfilt = lft;
    mintheta = 3.0;
    maxtheta = 7.0;
    initdelay=delay;
    mind = -30;
    maxd = 30;
    twooscil = false;
    inphase = true;
    totalprocpred=0.0;
    totaldiffZC=0.0;
    totaldiffHR=0.0;
    npt = LMAX - 1;
}

hilbert::hilbert(const hilbert &obj)
{
   // init(obj.filep, obj.pos);
    for (int i=0; i<LMAX; i++)
    {
        ampl1[i]=obj.ampl1[i];
        ampl2[i]=obj.ampl2[i];
        phase1[i]=obj.phase1[i];
        phase2[i]=obj.phase2[i];
    }
}

void hilbert::init()
{
    pos = 0;   
    npt = 0;
    for (int i = 1; i <= LMAX; i++) {
        x[i]=0.0;
        xh[i] = 0.0;
        ampl1[i] = 0.0;
        ampl2[i] = 0.0;
        phase1[i] = 0.0;
        phase2[i] = 0.0;    
    }       
}

void hilbert::firstinit(QString filepath, int end)
{
    pos = 0; 
    npt = end-1;
    for (int i = 1; i <= LMAX; i++) {
        xh[i] = 0.0;
        ampl1[i] = 0.0;
        ampl2[i] = 0.0;
        phase1[i] = 0.0;
        phase2[i] = 0.0;     
    }
    filep = filepath;
  //  readfromfile();   /*EEG Data set*/
}

void hilbert::readfromfile()
{
    QFile inputFile(filep);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in(&inputFile);
    QStringList sl;
    QString line;    
    if (!inputFile.exists()) {
        printf("Data file not found!");
        exit(EXIT_FAILURE);
    }
    else
    {
        line = in.readLine();
      //  sl = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      //  for (int i = 0; i < LMAX; i++)
      //      time[i]=sl[i].toDouble();
        line = in.readLine();
        sl = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if ((sl.length()-1)<npt)
            npt=sl.length()-1;
        channel = sl[0].toStdString();
        for (int k=0; k < npt; k++)
        //    if (sl[k+1].toDouble()>0)
               x[k]=sl[k+1].toDouble();
        //    else
        //       x[k]=0;
    }  
    inputFile.close();       
    /*Test Data set */
    //    for (int i = 0; i < npt; i++)
    //        x[i] = sin(i)+cos(2*i); //30*sin(11.2*i)+60*cos(20.4*i)+40*sin(2.3*i)+20*cos(70.1*i);
}

// convolution
void hilbert::convol(float *source, float *target, float *filt, int npt, int lfilt)
{
    int i, l;
    double yt;
    for (l = 1; l <= npt - lfilt + 1; l++) {
        yt = 0.0;
        for (i = 1; i <= lfilt; i++)
            yt = yt + source[l + i - 1] * filt[lfilt + 1 - i];
        target[l] = yt;
    }
    /* shifting lfilt/1+1/2 points */
    for (i = 1; i <= npt - lfilt; i++) {
        target[i] = 0.5*(target[i] + target[i + 1]);
    }
    for (i = npt - lfilt; i >= 1; i--)
        target[i + lfilt / 2] = target[i];
    /* writing zeros */
    for (i = 1; i <= lfilt / 2; i++) {
        target[i] = 0.0;
        target[npt + 1 - i] = 0.0;
    }
}

void hilbert::firhilbert()
{
    for (int i = 1; i <= lfilt; i++)
        hilb[i] = 1 / ((i - lfilt / 2) - 0.5) / PI;
    for (int i = 1; i <= npt; i++)
        xh[i]=0; 
    convol(x, xh, hilb, npt, lfilt);    
    for (int i = lfilt / 2 + 1; i <= npt - lfilt / 2; i++) {
        // qDebug()<<xh[i]; // filtered data
        xt = x[i];
        xht = xh[i];
        ampl1[i] = sqrt(xt*xt + xht*xht);
        phase1[i] = (atan2(xht, xt)) * ToDeg;
        if (phase1[i] < phase1[i - 1])
            omega1[i] = phase1[i] - phase1[i - 1] + 2*PI;
        else
            omega1[i] = phase1[i] - phase1[i - 1];
        omega1[i] = omega1[i] / 2*PI; // instantaneous frequency
    }
      //  cout << "Hilbert based on FIR filter length of " << lfilt << ":" << endl;
      //  for (int i = lfilt / 2 + 2; i <= npt - lfilt / 2 - 1; i++)
      //      printf("#  Ampl  Phase:  %d  %.6f  %.6f\n", i, ampl1[i], phase1[i]);
}

// FFT (Cooley–Tukey, divide-and-conquer)
void hilbert::fft(CArray& x)
{
    const size_t N = x.size();
    if (N <= 1) return;
    // divide
    CArray even = x[std::slice(0, N / 2, 2)];
    CArray  odd = x[std::slice(1, N / 2, 2)];
    // conquer
    fft(even);
    fft(odd);
    // combine
    for (size_t k = 0; k < N / 2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

// inverse fft
void hilbert::ifft(CArray& x)
{
    // conjugate the complex numbers
    x = x.apply(std::conj);
    // forward fft
    fft(x);
    // conjugate the complex numbers again
    x = x.apply(std::conj);
    // scale the numbers
    x /= x.size();
}

// matlabhilbert
void hilbert::matlabhilbert()
{
    for (int i = 0; i < npt; i++) {
        t[i].real(x[i]);
        t[i].imag(0);
    }
    data = CArray(t, npt);
    fft(data);
    double* hx = new double[npt];
    hx[0] = 1;
    for (int i = 1; i < npt; i++)
        if (i < npt / 2)
            hx[i] = 2;
        else
            hx[i] = 0;
    hx[(npt / 2)] = 1;
   // cout << endl;

    for (int i = 0; i < npt; i++)
    {
        data[i].real(hx[i] * data[i].real());
        data[i].imag(hx[i] * data[i].imag());
    }
    ifft(data);
    for (int i = lfilt / 2 + 1; i <= npt - lfilt / 2; i++)
    {
        ampl2[i] = abs(data[i].real());        
        phase2[i] = atan2(data[i].imag(), data[i].real()) * ToDeg;        
    } 
    //   cout << "Matlab-like Hilbert through FFT: " << endl;
    //  for (int i = lfilt / 2 + 2; i <= npt - lfilt / 2 - 1; i++)
    //       printf("#  Ampl  Phase:  %d  %.6f  %.6f\n", i, ampl2[i], phase2[i]);
}

void hilbert::averagefreq()
{
    avg1 = 0.0;
    //avg2 = 0.0;
    for (int i=1; i<=npt; i++)
    {
        if (phase1[i] < phase1[i - 1])
            omega1[i] = phase1[i] - phase1[i - 1] + 2*PI;
        else
            omega1[i] = phase1[i] - phase1[i - 1];
        omega1[i] = omega1[i] / 2*PI;
        avg1+=omega1[i];

 /*       if (phase2[i] < phase2[i - 1])
            omega2[i] = phase2[i] - phase2[i - 1] + 2*PI;
        else
            omega2[i] = phase2[i] - phase2[i - 1];
        omega2[i] = omega2[i] / 2*PI;
        avg2+=omega2[i];  */
    }    
  //  qDebug()<<avg1<<" "<<npt<<" "<<lfilt;
    avg1/=(npt - lfilt);
    avg1*=2;
   // avg2/=(npt - lfilt);
}
