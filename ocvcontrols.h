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
    int currfilterarea = 250, currfilterrate = 12, currfilttype = 5, totalfilts = 5;
    int sigma_color = 25, sigma_space = 50, kernel_s = 5;                       // cartoonize params
    int wave_freqs = 42, wave_amp = 9;                                          // waves params
    int dilation_size = 1, dilation_elem = 2;                                   // dilate params
    int nfeatures = 100, nlevels = 6, edgetreshold = 20; float scalef = 1.1;    // ORB params
    QColor fcolor = QColor(255,255,255); bool randfcolor=false;                 // ORB params
    int mixtype = 1, transp = 80, randpicn;                                     // mixer params
    bool changerandpic_byclick = false; bool dreamflowmode = false;             // mixer params
    bool autodreamflow = false; int dreamflowrate = 77;                         // mixer params
    bool attmodul_area = false; bool attent_modulated_dreams = false;           // attention modulated filter area
    Mat randpic;

    leftpanel* leftpan;
    MainWindow* mww;
    void updateformvals();
    void changerandpic();
    void stopdreamflow();
    explicit ocvcontrols(QWidget *parent = 0);
    ~ocvcontrols();

private slots:
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

private:
    Ui::ocvcontrols *ui;
};

#endif // OCVCONTROLS_H
