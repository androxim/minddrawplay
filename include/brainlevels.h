#ifndef BRAINLEVELS_H
#define BRAINLEVELS_H

#include <QWidget>
#include "mainwindow.h"
#include "plotwindow.h"

class MainWindow;
class plotwindow;

namespace Ui {
class brainlevels;
}

class brainlevels : public QWidget
{
    Q_OBJECT

public:
    MainWindow* mww;
    plotwindow* plw;
    int attention, meditation, attention_bord;
    bool attention_I;
    explicit brainlevels(QWidget *parent = 0);
    void updatelevels(int att, int medit);
    void closeEvent(QCloseEvent *event);
    void show_attentionborder();
    void settonesbordervisible(bool fl);
    ~brainlevels();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_comboBox_currentIndexChanged(int index);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

private:
    Ui::brainlevels *ui;
};

#endif // BRAINLEVELS_H
