/* header file for ocvcontrols class -
   resposible for MindOCV parameters and actions */

#ifndef OCVCONTROLS_H
#define OCVCONTROLS_H

#include <QWidget>
#include "mainwindow.h"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

namespace Ui {
class ocvcontrols;
}

class leftpanel;
class MainWindow;

class ocvcontrols : public QWidget
{
    Q_OBJECT

public:
    bool formshown = true;
    bool camerainp = false;
    bool drawbrushcontour = true;
    bool histFeaturesReady = false;
    QPoint currmousepos;
    int picwidth = 2000, picheight = 1125;
    int currfilterarea = 250, currfilterrate = 12, currfilttype = 5, totalfilts = 5;
    int sigma_color = 25, sigma_space = 50, kernel_s = 5;                              // cartoonize params
    int wave_freqs = 42, wave_amp = 9;                                                 // waves params
    int dilation_size = 1, dilation_elem = 2;                                          // dilate params
    int nfeatures = 100, nlevels = 6, edgetreshold = 20; float scalef = 1.1;           // ORB params
    QColor fcolor = QColor(255,255,255); bool randfcolor=false;                        // ORB params
    int mixtype = 1, transp = 80, randpicn;                                            // mixer params
    bool changerandpic_byclick = false; bool changebyattention = false;                // mixer params
    bool dreamflow = false; int dreamflowrate = 77; bool polygonmask = true;           // mixer params
    bool changepic_bytime = false; int changepic_interval = 3; QTimer* pichngT;        // mixer params
    QPoint seed; int x_left, x_right, y_top, y_bottom, drops_interval = 50;            // mixer params
    bool dropsmode = false; QTimer* dropsT; int dropsgrow_step = 24; Scalar wcolor;    // mixer params
    int firstdrop_size = 30; bool plotdroprect = true; bool drops_byatt = false;       // mixer params
    bool poly_by_att = true; int pointsinpoly = 3; bool seed_frommousepos = false;     // mixer params
    bool attmodul_area = false; bool attent_modulated_dreams = false;    // attention modulated filter area and rate of dreamflow
    bool hueonly = false; bool transp_by_att = false;                    // hue only, without overlay; transparency by attention    
    bool directionswitch_by_att = false;    
    bool showlabel = false; char l_str[50];  // attention label parameters
    int l_posx = 1500, l_posy = 80, lfont_scale = 4, lfont_size = 5, lw = 484, lh = 80;
    int flowdirection = 0; Mat randpic; int allngb = 30, ngbarea = 20; // area of neighbours for nearest / farest pic
    // 0: "random" - next pic is randomly from all, 1: "similar" - from N nearest, -1: "opposite" - from N farest

    leftpanel* leftpan;
    MainWindow* mww;
    void updateformvals();
    void changerandpic();
    void setcurrdream(int t);
    void stopdreamflow();    
    void setcameracheckbox(bool fl);
    explicit ocvcontrols(QWidget *parent = 0);
    ~ocvcontrols();

signals:
    flow_direction_available();

public slots:
    void set_flow_direction_available();

private slots:

    void randpicchange_Update();

    void windowsize_Update();

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_tabWidget_tabBarClicked(int index);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_checkBox_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_spinBox_9_valueChanged(int arg1);

    void on_spinBox_10_valueChanged(int arg1);

    void on_spinBox_11_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_spinBox_12_valueChanged(int arg1);

    void on_radioButton_4_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_6_clicked();

    void on_pushButton_3_clicked();

    void on_checkBox_2_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_3_clicked();

    void on_checkBox_4_clicked();

    void on_checkBox_5_clicked();

    void on_spinBox_13_valueChanged(int arg1);

    void on_checkBox_6_clicked();

    void on_checkBox_7_clicked();

    void on_checkBox_8_clicked();

    void on_checkBox_9_clicked();

    void on_spinBox_14_valueChanged(int arg1);

    void on_checkBox_10_clicked();

    void on_checkBox_12_clicked();

    void on_spinBox_16_valueChanged(int arg1);

    void on_checkBox_11_clicked();

    void on_spinBox_15_valueChanged(int arg1);

    void on_checkBox_15_clicked();

    void on_checkBox_14_clicked();

    void on_checkBox_16_clicked();

    void on_checkBox_13_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_checkBox_17_clicked();

private:
    Ui::ocvcontrols *ui;
};

#endif // OCVCONTROLS_H
