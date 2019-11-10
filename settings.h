#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <plotwindow.h>

class plotwindow;

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:  
    plotwindow* pwd;    
    double esavg, esprop;
    int eslength;
    explicit Settings(QWidget *parent = 0);   
    ~Settings();
    void init();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_spinBox_20_valueChanged(int arg1);

    void on_pushButton_clicked();

private:
    Ui::Settings *ui;

};

#endif // SETTINGS_H
