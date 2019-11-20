#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <qcustomplot.h>
#include <hilbert.h>
#include <vector>
#include "mainwindow.h"
#include "omp.h"
#include "stdio.h"
#include <Eigen/Dense>
#include <paintscene.h>
#include <QAudioRecorder>
#include "qsound.h"
#include "QSoundEffect"
#include "QMediaPlayer"
#include "rawsignal.h"

namespace Ui {
class plot;
}

using namespace Eigen;

typedef std::vector<int> vectori;
typedef std::vector<double> vectord;

struct coords {
    QVector<double> xc,amp0,amp1,amp2,of1,of2,of3,of4;
    int lscale,lmax,posstim;   
};

struct freqband {
    int lf,hf;
};

static const int shift=100;

class appconnect;
class MainWindow;
class Settings;
class paintScene;
class myTone;

class plotwindow : public QWidget
{
    Q_OBJECT       

public:  
    int stepsPerPress, drawshift, graphcount, recnumb, baseshift, scaletimeout, tonenumbers, maxtones, chorddelay, mxttimeout;
    int counter,stims,startpos,tscale,ampval,corrprednumb,recparts, autoregdegree, autoreglength, daqscalevalue, transdelay, flength, chnums, sampleblock, exch, sourcech;
    QString daqport;
    QString folderpath;
    rawsignal* rws;
    bool bfiltmode,adaptivenumparts;
    myTone* myT1;
    myTone* myT2;
    myTone* myT3;
    myTone* myT4;
    myTone* myT5;
    myTone* myT6;
    myTone* myT7;
    myTone* myT8;
    QDir fd;
    bool backimageloaded;
    bool filteringback;
    bool hidebutt, soundmodul, fixback;
    QStringList imglist;
    int picchangeborder;
    QPixmap pm;
    QImage QM, qbim1, qbim2;
    QSoundEffect* Glow; QSoundEffect* D3;
    QSoundEffect* fdiez; QSoundEffect* A3;
    QSoundEffect* Clow; QSoundEffect* C4;
    QSoundEffect* atone; QSoundEffect* A4;
    QSoundEffect* Elow; QSoundEffect* D4;
    QSoundEffect* dtone; QSoundEffect* E4;
    QSoundEffect* Dlow; QSoundEffect* F3;
    QSoundEffect* btone;
    QSoundEffect* Blow; QSoundEffect* F4;
    QSoundEffect* gtone;
    QSoundEffect* tone_Blow; QSoundEffect* tone_b;
    QSoundEffect* tone_Dlowsh; QSoundEffect* tone_dsh;
    QSoundEffect* tone_Glowsh; QSoundEffect* tone_gsh;
    QSoundEffect* tone_Flowsh; QSoundEffect* tone_fsh;
    QSoundEffect* tone_Clowsh; QSoundEffect* tone_csh;
    bool readyfortones;
    qreal volume;
    int simsrfr;
    int grct;
    int maxtonerepeats, memorylength, attent;
    bool paintfstart, attentmodul, recplaying;
    bool start, offlinedata, addmode, addmodeon, estimation, adaptampl, addmoderec, hideardata, usefiltering, filterardata, carfilter, zerocr, phasepr;
    bool showprediction, estimpredstim, filtereddata, correctdelay, maxentropy, leastsqr, regforstim, hronar, hronarrun, filtfir;
    double *autoreginput;
    double *autoregcoeffs;
    double plv, averopttime, fsampr, flowpass;
    int *exlchannels;
    bool recurbutter, zerobutter, mindplay, blink, playsaved, scalechange, spacemode, tank1mode, tank2mode, recordstarted, micrecord, antirepeat, randmxt;
    QAudioRecorder* audioRecorder;
    int loopnum;
    int volumebord, meditt;
    bool mindwstart, fftfreqs, attentionvolume, startnmmode, simeeg;
    QVector<double> eegdata[20];
    vectord acoeffs, bcoeffs, ac, bc, b_f, a_f, b_s, a_s;
    int butterord, lcutoff, hcutoff;
    double offsetcomp, intlen;
    double** rawdata;
    int* indexes;
    double* DenC;
    double* NumC;
    int nextdrawshift;
    int samplenums, samplesize, eegintervals, intervallength, xraw, edge, delta, theta, alpha, beta, gamma, hgamma;
    int deltanum, thetanum, alphanum, betanum, gammanum, hgammanum, tonedelay, nums;
    int sdelta, stheta, salpha, sbeta, sgamma, shgamma, tv1, tv2, tv3, tv4, tv5, tv6, tv7, tv8, tv9, tv10;
    int* tvals;
    int pushshift, psleep;
    double meandelta, meantheta, meanalpha, meanbeta, meangamma, meanhgamma;
    bool tunemode, pssstart, initfilterback, rawsignalabove;
    QSet<int> pressedKeys;
    CArray cdata;
    QPixmap* pmvr;
    QPainter* ptr;
    QGraphicsPixmapItem itemforfilt;
    QImage resforfilt;
    QGraphicsScene sceneforfilt;
    freqband sigb, noibp, noibs;
    MainWindow* mw;
    Settings* sw;
    paintScene* pss;
    QElapsedTimer timer;
    QTimer* tim;
    QTimer* scaletim;
    QTimer* mxttim;
    QTimer* tn1;
    QTimer* tn2;
    QTimer* tn3;
    QTimer* tn4;
    QTimer* tn5;
    QTimer* tn6;
    QTimer* tn7;
    QTimer* tn8;
    QTimer* tn9;
    QTimer* tn10;
    QTimer* tryplay;
    QTimer* checkstates;
    QTimer* colorchange;
    QByteArray stimarr;
    hilbert* hnt;
    coords arrc;
    appconnect* appcn;
    QSound* Bells;
    QString tones;
    QString lasttones;
    QStringList strLst1, strLst2, strLst3;
    QStringListModel *strLstM1, *strLstM2, *strLstM3;
    QPixmap backimg;
    explicit plotwindow(QWidget *parent = 0);    
    ~plotwindow();
    void plot(QCustomPlot *customPlot);
    bool eventFilter(QObject *target, QEvent *event);          
    double phasediff(double* stim1, double* stim2, int length);
    void clearstim(int posstim, int length);
    void doplot();
    void singlestim();
    void clearstim();
    void cleareeg(int start, int end);
    void clearfiltered();
    void draw(int start);
    void bcidata(double d1);
    void analysemeandata();
    void analysepart();
    void analyseinterval();
    void playdata();
    void gettones();
    void playtank1(QString tonesset);
    void playtank2(QString tonesset);
    void playspace(QString tonesset);
    void randomtone();
    void letsplay();

    void recurbuttfilt();
    void savedatatofile(QString fname);
    void loaddatafromfile(QString fname);
    void savescaletofile(QString fname);
    void loadscalefromfile(QString fname);
    void setrandomscale();
    void refresh();
    int estimateoptlength(int n, int l1, int l2, int pos);
    double estimateoptprop(int n, double p1, double p2, int pos);
    void setaddmode(bool f);  
    int maxabsstim(int pos);
    void stimulationonly(int pos);
    double offlinevaluation(int pos, int i);
    void maxentropyf(double *input, int length, int degree, double **ar, double *per, double *pef, double *h, double *g);
    void autoregress(double *input, int length, int degree, double *coeffs);
    void runautoreg(int pos);
    double zcdifference(int pos);
    void printtoresultstring(QString str);
    void printtoresultbox(QString str);
    void printtoresultmean(QString str);
    void cleareegdata();

    void cleanmem();

    void hilbonar(int pos);
    double arphasediff(int posstim, int length);

    void getrawdata(int chn, double val);

    void filterdata(int posstim, int length);
    void filterar(int posstim, int length);

    void zerocrossingstim(int posstim, int length);

    void recurbutterf(int order, double sfr, double hpf, double* x, int length);
    void recurbutterfilter(int posstim, int length);
    void recurbutterfilterar(int posstim, int length);

    double* ComputeLP(int FilterOrder );
    double* ComputeHP(int FilterOrder );
    double* TrinomialMultiply(int FilterOrder, double *b, double *c );
    double* ComputeNumCoeffs(int FilterOrder);
    double* ComputeNumCoeffs(int FilterOrder, double Lcutoff, double Ucutoff, double *DenC);
    double* ComputeDenCoeffs(int FilterOrder, double Lcutoff, double Ucutoff);
    void filter(int ord, double *a, double *b, int np, double *x, double *y);
    void butterfiltcoefs(int lcut, int hcut, int order, int sampr);

    void filtfilt(vectord B, vectord A, const vectord &X, vectord &Y);
    void filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi);
    vectord subvector_reverse(const vectord &vec, int idx_end, int idx_start);
    void append_vector(vectord &vec, const vectord &tail);
    void add_index_const(vectori &indices, int value, size_t numel);
    void add_index_range(vectori &indices, int beg, int end);
    void zerophaseinit(int lcut, int hcut, int order, int sampr);
    void zerophasefilt(int posstim, int length);

    void getcoeffs(vectord& acc, vectord& bcc, int ord);
    MatrixXf covarian(MatrixXf mat);
    MatrixXf readdata(int intlen, int length);

    void pushleft();
    void pushright();
    void pushup();
    void pushdown();
    void pushenter();
    void delay(int temp);

    void play_Glow();
    void play_fdiez();
    void play_Clow();
    void play_a();
    void play_Elow();
    void play_d();
    void play_Dlow();
    void play_b();
    void play_Blow();
    void play_g();
    void cleanbuttons();
    void settonesvolume();

    void update_attention(int t);
    void update_meditation(int t);

    void radiobut1();
    void radiobut2();
    void radiobut3();
    void enablenumparts(bool fl);

    void setpicfolder(QString fp);
    void applyfilteronback();
    QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent);
    void setbackimage(QPixmap pm);

private slots:

    void timerUpdate();

    void scaletimerUpdate();

    void mxttimerUpdate();

    void tn1Update();
    void tn2Update();
    void tn3Update();
    void tn4Update();
    void tn5Update();
    void tn6Update();
    void tn7Update();
    void tn8Update();
    void tn9Update();
    void tn10Update();
    void playUpdate();
    void checkstatesUpdate();
    void colorchangesUpdate();

    void on_widget_destroyed();

    void on_pushButton_clicked();

    void on_checkBox_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_4_clicked();

    void slSettings();

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_8_clicked();

    void on_spinBox_7_editingFinished();

    void on_spinBox_5_editingFinished();

    void on_checkBox_5_clicked();

    void on_checkBox_6_clicked();

    void on_checkBox_7_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_2_valueChanged(int value);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_9_valueChanged(int arg1);

    void on_spinBox_10_valueChanged(int arg1);

    void on_spinBox_11_valueChanged(int arg1);

    void on_spinBox_12_valueChanged(int arg1);

    void on_spinBox_13_valueChanged(int arg1);

    void on_spinBox_14_valueChanged(int arg1);

    void on_spinBox_15_valueChanged(int arg1);

    void on_checkBox_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_checkBox_2_clicked();

    void on_spinBox_16_valueChanged(int arg1);

    void on_checkBox_8_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_spinBox_17_valueChanged(int arg1);

    void on_checkBox_9_clicked();

    void on_spinBox_18_valueChanged(int arg1);

    void on_spinBox_19_valueChanged(int arg1);

    void on_checkBox_10_clicked();

    void on_spinBox_20_valueChanged(int arg1);

    void on_spinBox_21_valueChanged(int arg1);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_radioButton_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_2_clicked();

    void on_pushButton_23_clicked();

    void on_checkBox_11_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_checkBox_12_clicked();

    void on_pushButton_24_clicked();

    void on_spinBox_22_valueChanged(int arg1);

    void on_checkBox_13_clicked();

    void on_pushButton_25_clicked();

private:
    Ui::plot *ui;

};

#endif // PLOT_H
