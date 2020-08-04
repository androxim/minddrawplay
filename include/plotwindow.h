/* header file for MindPlay window class -
   responsible for raw EEG signal processing,
   brain waves flow drawing, image filters application and sound translations */

#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <vector>
#include <valarray>
#include "complex"
#include <Eigen/Dense>
#include <qcustomplot.h>
#include "mainwindow.h"
#include "stdio.h"
#include "paintscene.h"
#include "rawsignal.h"
#include "soundplayer.h"
#include "filters.h"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"

#define NMAX 15360 // max length of single EEG line, 15360: 30 sec for 512 sampling rate

namespace Ui {
class plot;
}

typedef complex<double> ComplexM;
typedef valarray<ComplexM> CArray;

struct coords { QVector<double> xc,amp0; };

class appconnect;
class MainWindow;
class Settings;
class paintScene;
class paintform;
class ocvcontrols;
class filters;

class plotwindow : public QWidget
{
    Q_OBJECT       

public:      
    // description of most variables in plotwindow.cpp //

    MainWindow* mw;         // pointer on MainWindow object
    Settings* sw;           // pointer on settings object window
    paintScene* pss;        // pointer on paintScene object
    rawsignal* rws;         // pointer on raw signal plot window
    paintform* paintf;      // pointer on MindDraw window object
    ocvcontrols* ocvf;      // pointer on MindOCV window object
    appconnect* appcn;      // pointer on AppConnect object (BCI2000)
    filters* filtercl;      // pointer on object with standard signal filters

    soundplayer splayer;    // sound player object with sound samples and timers associated with own thread
    QStringList strLst2; QStringListModel *strLstM2;  // list of playing tones
    QString tones, lasttones;

    QGraphicsBlurEffect *blurp;
    QGraphicsColorizeEffect *colorizep;
    QStringList imglist; QString folderpath; QDir fd;
    QPixmap* pmvr; QPainter* ptr;
    QGraphicsPixmapItem itemforfilt;
    QGraphicsScene sceneforfilt;
    QImage resforfilt;    

    QPixmap pm, pmx, backimg;
    QPalette sp1, sp2;
    QImage QM, qbim1, qbim2;
    qreal volume;
    QVector<double> eegdata[20];
    QVector<int> delta_vals, theta_vals, alpha_vals, beta_vals, gamma_vals;

    double** rawdata; int* indexes; int* tvals;
    int srfr, numst, imlength, stlength, drawshift, graphcount, scaletimeout, tonedelay;
    int stepsPerPress, tonenumbers, maxtones, chorddelay, mxttimeout, curmodval, nums;
    int counter, stims, recparts, chnums, sampleblock, sourcech, picchangeborder;
    int simsrfr, maxtonerepeats, memorylength, attent, minvolumebord, meditt;
    int maxshown_eeglines, xraw, delta, theta, alpha, beta, gamma, hgamma;
    double meandelta, meantheta, meanalpha, meanbeta, meangamma, meanhgamma;
    int sdelta, stheta, salpha, sbeta, sgamma, shgamma, points_for_mean;
    int tv1, tv2, tv3, tv4, tv5, tv6, tv7, tv8, tv9, tv10;
    int pushshift, psleep, camera_interval;
    int lcutoff, hcutoff, butterord;
    int nextdrawshift, tonescheck;
    int buffercount;

    bool adaptivenumparts, backimageloaded, canbackchange, opencvstart;
    bool filteringback, blurback, hidebutt, attention_interval, fixback, colorizeback;
    bool attention_modulation, start, brainflow_on, estimation, updatewavesplot;
    bool usefiltering, musicmode_on, flowblinking, scalechange;
    bool spacemode, tank1mode, tank2mode, recordstarted, antirepeat, randmxt;       
    bool mindwstart, fftfreqs, adaptive_volume, keys_emulated, simeeg;
    bool tunemode, paintfstart, rawsignalabove, camerainp;

    QString tank1[10] = {"b","B","g","G","d","D","E","C","f#","a"};
    QString tank2[10] = {"c#","C#","b","B","f#","F#","g#","G#","d#","D#"};
    QString space[8] = {"F4","F3","D3","E4","D4","C4","A3","A4"};

    QSet<int> pressedKeys;
    CArray cdata; ComplexM t[2048]; // arrays for FFT calculation

    QElapsedTimer timer;
    QTimer* scaletim;
    QTimer* mxttim;   
    QTimer* neuro_neMehanika_camera;
    QTimer* neuro_neMehanika_colors;    
    QTimer* camerainput; cv::VideoCapture camera; cv::Mat trp;
    QTimer* tn1;  QTimer* tn2;  QTimer* tn3;  QTimer* tn4;  QTimer* tn5;
    QTimer* tn6;  QTimer* tn7;  QTimer* tn8;  QTimer* tn9;  QTimer* tn10;
    QThread* tr1; QTimer* tryplay;
    coords arrc;

    explicit plotwindow(QWidget *parent = 0);    
    ~plotwindow();    
    bool eventFilter(QObject *target, QEvent *event);           
    void plot(QCustomPlot *customPlot);
    void doplot();   

    void updatedata(int start);
    void getandprocess_eeg_data(double d1);
    void get_multichan_rawdata(int chn, double val);
    void determine_brainwaves_expression();
    void analyse_interval();
    void plot_interval();
    void playtank1(QString tonesset);
    void playtank2(QString tonesset);
    void playspace(QString tonesset);
    void randomtone();
    void letsplay();
    QPixmap grabmindplay();
    void init_timersinthread();

    void setpicfolder(QString fp);
    void savescaletofile(QString fname);
    void loadscalefromfile(QString fname);
    void setrandomscale();
    void setmusicmode(bool fl);
    void update_intervals_spinboxes();
    void print_tones(QString str);
    void musicmode_on_off();
    void camerainp_on_off();
    void update_brainexp_levels(int d, int md, int t, int mt, int a, int ma, int b, int mb, int g, int mg);

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
    void tonenumbers_increase();
    void cleanbuttons();
    void settonesvolume();
    void quitthreads();

    void update_attention(int t);
    void update_meditation(int t);

    void radiobut1();
    void radiobut2();
    void radiobut3();
    void enable_num_intervals(bool fl);
    void enablehue();
    void turn_music_checkbox(bool fl);

    QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent);
    void applyfilteronback();
    void setbackimage(QPixmap pm, bool saveback);
    void setbackimg_fromleftpanel(QString fpath);
    void updateimlength(int t);
    void playtones();
    void process_eeg_data();

private slots:

    void scaletimerUpdate();

    void mxttimerUpdate();

    void camerainput_Update();

    void neuro_neMeh_camera_update();

    void neuro_neMeh_colors_update();

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

    void on_checkBox_3_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_4_clicked();

    void slSettings();

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

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

    void on_checkBox_5_clicked();

    void on_checkBox_6_clicked();

    void on_checkBox_7_clicked();

    void on_horizontalSlider_valueChanged(int value); 

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

    void on_checkBox_14_clicked();

    void on_pushButton_2_clicked();

    void on_checkBox_15_clicked();

    void on_checkBox_8_clicked();

private:
    Ui::plot *ui;

};

#endif // PLOT_H
