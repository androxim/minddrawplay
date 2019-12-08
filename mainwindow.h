#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <plotwindow.h>
#include <hilbert.h>
#include "appconnect.h"
#include "rawsignal.h"

class plotwindow;
class appconnect;
class Settings;
class paintform;
class updateplot;

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:  
    hilbert* ht;
    bool psstart;
    bool pwstart;
    bool simeeg;
    bool bciconnect;
    bool opencvstart;
    QString daqport;
    QTimer* mindwt;
    QTimer* picfilt;
    QTimer* simulateEEG;
    updateplot* updl;
    int packetsRead;
    int connectionId;
    int currentsimdata;
    int currentel;
    int srfr;
    int curhue, prevhue;
    QPixmap bkgnd;
    QPalette palette;
    QString folderpath;
    QString opencvpic;
    QStringList imglist;
    bool canchangehue;
    int deltafr, thetafr, alphafr, betafr, gammafr, hgammafr;
    int deltaphs, thetaphs, alphaphs, betaphs, gammaphs;
    int zdeltaamp, zthetaamp, zalphaamp, zbetaamp, zgammaamp, zhgammaamp;
    int deltaamp, thetaamp, alphaamp, betaamp, gammaamp, hgammaamp;
    double allpower, p_delta, p_theta, p_alpha, p_beta, p_gamma1, p_gamma2;
    int mw_raw, mw_atten, mw_medit, mw_delta, mw_theta, mw_alpha1, mw_alpha2, mw_alpha, mw_beta1, mw_beta2, mw_beta, mw_gamma1, mw_gamma2;
    explicit MainWindow(QWidget *parent = 0);      
    void adddata(string s, QString fpath);
    void printdata(QString str);
    void mindwaveconnect();
    void museconnect();
    void museconnected();
    void startopencv();
    void getrawdata(float ft);
    void getfreqval(float dw, float tw, float aw, float bw, float gw);
    void setsourceimg(QString fpath);
    void sethue(int i);

    ~MainWindow();

private slots:    

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void OpenDataFile();

    void on_pushButton_5_clicked();

    void picfiltUpdate();
    void mindwtUpdate();
    void simulateEEGUpdate();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    plotwindow *plotw;
    paintform *paintw;
    appconnect *connectWin;
    rawsignal *rs;

};

#endif // MAINWINDOW_H
