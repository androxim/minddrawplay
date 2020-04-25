/* header file for MainWindow class -
   resposible for connecting to EEG device, starting EEG generator,
   starting MindPlay / MindDraw / MindOCV windows, processing most MindOCV actions */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "string.h"
#include <plotwindow.h>
#include "appconnect.h"
#include "rawsignal.h"
#include "leftpanel.h"
#include "rightpanel.h"
#include "ocvcontrols.h"

class plotwindow;
class appconnect;
class Settings;
class paintform;
class leftpanel;
class rightpanel;
class ocvcontrols;

using namespace std;

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:  
    // description of variables in mainwindow.cpp //

    int packetsRead, connectionId, currentsimdata, currentel, srfr;
    int opencvinterval, curhue, prevhue, curoverl, prevoverl;
    int deltafr, thetafr, alphafr, betafr, gammafr, hgammafr;
    int deltaphs, thetaphs, alphaphs, betaphs, gammaphs;
    int zdeltaamp, zthetaamp, zalphaamp, zbetaamp, zgammaamp, zhgammaamp;
    int deltaamp, thetaamp, alphaamp, betaamp, gammaamp, hgammaamp;
    int mw_raw, mw_atten, mw_medit, mw_delta, mw_theta;
    int mw_alpha1, mw_alpha2, mw_alpha, mw_beta1, mw_beta2, mw_beta, mw_gamma1, mw_gamma2;

    // indexes of nearest and farest pics in terms of histogram
    int nearest_pics[15]; int farest_pics[15];

    double allpower, p_delta, p_theta, p_alpha, p_beta, p_gamma1, p_gamma2;

    bool paintw_started, pwstart, simeeg, bciconnect, opencvstart, ocvcontrshow, storymode;
    bool canchangehue, canchangeoverlay, histfinished;

    QTimer* mindwt;
    QTimer* picfilt; 
    QTimer* simulateEEG;
    QTimer* dreamflow_timer;
    QTimer* streamflows;
    QTime time_take;

    QPixmap bkgnd, transferpm;
    QPalette palette;
    QString opencvpic, picst;
    QVector<QPixmap> imgarray;
    vector<int> picsarr;
    QVector<QString> imgpaths;
    QFutureWatcher<QImage> *imageScaling;

    explicit MainWindow(QWidget *parent = 0);      
    void printdata(QString str);
    void mindwaveconnect();        
    void startopencv();
    void getrawdata(float ft);
    void getfreqval(float dw, float tw, float aw, float bw, float gw);
    void setsourceimg(QString fpath);
    void setsourceimgd(QImage qp);
    void sethue(int i);
    void setopencvt(int i);
    void setattent(int i);
    void setoverlay(int i);
    void setborder(int i);
    void checkoverlay();
    void swap_main_overlay();
    void makeHistFeatures();
    void makeIconsAndHists();
    void shuffleiconss(bool left);    
    void just_update_mainpic(int num);
    int geticonnum(int t, bool left);
    int getmainpic();
    int getoverpic();
    void updatemainpic(int num);
    void updateoverpic(int num);
    void setfolderpath(QString fp);   
    void cancellast();
    void cancelall();
    void keys_processing();
    void setdstfromplay(QImage qm);
    void save_and_add_overlaypic();
    void run_opencvform();
    void drawwindow(int x, int y, int w, int h);
    void setdream0();
    void setprevdfrect(int x, int y, int w, int h);
    QImage grabopcvpic();
    QString getfolderpath();
    QString getimagepath(int t);
    int getimagenum(QString st);
    void gethistfeatures();
    float chi2_distance(vector<float> f1, vector<float> f2);
    void getchi2dists(int t);
    void usingcam(bool fl);
    void fillpuzzle_withneighbours();
    void fillmaininpuzzle(int t);
    int getchi2distsize();

    ~MainWindow();

signals:
    void histFinished();

public slots:
    void enablenewfolder();
    void addScaledImage(int num);
    void scalingFinished();

private slots:    

    void picfiltUpdate();
    void mindwtUpdate();
    void simulateEEGUpdate();
    void dreamflow_Update();
    void streamflows_Update();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    plotwindow *plotw;
    paintform *paintw;    
    appconnect *connectWin;  

};

#endif // MAINWINDOW_H
