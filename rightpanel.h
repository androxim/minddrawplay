#ifndef RIGHTPANEL_H
#define RIGHTPANEL_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class rightpanel;
}

class MainWindow;

class rightpanel : public QWidget
{
    Q_OBJECT

public:
    MainWindow* mww;
    int imgnumber;
    Qt::AspectRatioMode rationmode;
    int imgsize = 138;
    int currpos = 0;
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
    void fillpics();
    explicit rightpanel(QWidget *parent = 0);
    ~rightpanel();

private slots:
    void on_pushButton_clicked();

private:
    Ui::rightpanel *ui;
};

#endif // RIGHTPANEL_H
