#ifndef OCVCONTROLS_H
#define OCVCONTROLS_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class ocvcontrols;
}

class MainWindow;

class ocvcontrols : public QWidget
{
    Q_OBJECT

public:
    int filtarea, filtrate, filttype;
    int kernelsize, s_color, s_space, wave_freq, wave_amp, dilate_size, dilation_el;
    bool attmodulation;
    MainWindow* mww;
    updatevals();
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

private:
    Ui::ocvcontrols *ui;
};

#endif // OCVCONTROLS_H
