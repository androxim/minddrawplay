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
    QPoint currmousepos;
    std::mt19937 gen; std::uniform_int_distribution<int> dist;
    bool formshown = true, camerainp = false, attent_modul = true;
    bool drawbrushcontour = true; bool drops_by_click_mode = false;
    bool circle_brush = true; bool histFeaturesReady = false;
    bool multi_img_dflow = false; bool multi_img_by_att = false;
    int picwidth = 2000, picheight = 1125; bool color_overlay_flow = false;
    int currfilterarea = 250, currfilterrate = 12, currfilttype = 5, totalfilts = 6;

    int sigma_color = 25, sigma_space = 50, kernel_s = 5;                              // cartoonize params
    int wave_freqs = 42, wave_amp = 9;                                                 // waves params
    int dilation_size = 1, dilation_elem = 2;                                          // dilate params
    int nfeatures = 100, nlevels = 6, edgetreshold = 20; float scalef = 1.1;           // ORB params
    QColor fcolor = QColor(255,255,255); bool randfcolor = false;                      // ORB params

    std::vector<int> cells_indexes, free_cells; bool doublepicsmode = false;           // puzzle params
    bool puzzleflow_on = false, puzzle_edges = true, white_edges = true;               // puzzle params
    double corr_cell_part = 0.8; int puzzleflowrate = 100, changepuzzleborder = 90;    // puzzle params
    int cell_size = 200; int cols = 10; int rows = 5; unsigned int cellnums = 50;      // puzzle params
    bool corrcells_by_att = true, puzzlerate_by_att = true, cellsize_by_att = true;    // puzzle params

    int mixtype = 3, transp = 80, multi_set_size = 2, randpicn;                        // mixer params
    bool changerandpic_byclick = false; bool changebyattention = false;                // mixer params
    bool dreamflow = false; int dreamflowrate = 77; bool polygonmask = false;          // mixer params
    bool changepic_bytime = false; int changepic_interval = 3; QTimer* pichngT;        // mixer params
    QPoint seed; int x_left, x_right, y_top, y_bottom, drops_interval = 50;            // mixer params
    bool dropsmode = true; QTimer* dropsT; int dropsgrow_step = 24; Scalar wcolor;     // mixer params
    int firstdrop_size = 30; bool plotdroprect = true; bool drops_by_att = false;      // mixer params
    bool poly_by_att = true; int pointsinpoly = 3; bool drops_from_mousepos = false;   // mixer params

    bool attmodul_area = false; bool attent_modulated_dreams = false;    // attention modulated filter area and rate of dreamflow
    bool hueonly = false; bool transp_by_att = false;                    // hue only, without overlay; transparency by attention    
    bool directionswitch_by_att = false;    
    int flowdirection = 0; Mat randpic; int allngb = 30, ngbarea = 20; // area of neighbours for nearest / farest pic
    // 0: "random" - next pic is randomly from all, 1: "similar" - from N nearest, -1: "opposite" - from N farest

    bool showmenu = true; char l_str[50];  // attention label parameters
    char l_menu_item1[50], l_menu_item2[50], l_menu_item3[50], l_menu_item4[50];
    int l_posx = 1500, l_posy = 80, lfont_scale = 4, lfont_size = 5, lw = 484, lh = 80;
    int l_menu_posx = 30, l_menu_posy = 50, l_menu_fontsize = 4, l_menu_fontscale = 3, lmenuw = 625, lmenuh = 180;

    leftpanel* leftpan;
    MainWindow* mww;
    void updateformvals();
    void changerandpic();
    void randmixer_mode_on();
    void setcurrdream(int t);
    void start_stop_dreamflow(bool fl);
    void start_stop_puzzleflow(bool fl);
    void drop_center_from_mousepos();
    void setcameracheckbox(bool fl);
    void blocktabs(int i);
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

    void on_radioButton_7_clicked();

    void on_radioButton_8_clicked();

    void on_checkBox_18_clicked();

    void on_checkBox_19_clicked();

    void on_spinBox_17_valueChanged(int arg1);

    void on_pushButton_5_clicked();

    void on_spinBox_20_valueChanged(int arg1);

    void on_spinBox_19_valueChanged(int arg1);

    void on_spinBox_18_valueChanged(int arg1);

    void on_checkBox_22_clicked();

    void on_checkBox_21_clicked();

    void on_checkBox_20_clicked();

    void on_spinBox_21_valueChanged(int arg1);

    void on_pushButton_6_clicked();

    void on_checkBox_23_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

    void on_comboBox_3_currentIndexChanged(int index);

    void on_checkBox_24_clicked();

private:
    Ui::ocvcontrols *ui;
};

#endif // OCVCONTROLS_H
