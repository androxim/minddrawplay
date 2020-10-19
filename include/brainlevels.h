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
    explicit brainlevels(QWidget *parent = 0);
    void updatelevels(int att, int medit);
    void closeEvent(QCloseEvent *event);
    void show_attentionborder();
    ~brainlevels();

private slots:
    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::brainlevels *ui;
};

#endif // BRAINLEVELS_H
