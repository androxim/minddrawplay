#ifndef LEFTPANEL_H
#define LEFTPANEL_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class leftpanel;
}

class MainWindow;

class leftpanel : public QWidget
{
    Q_OBJECT

public:
    MainWindow* mww;
    int imgnumber;
    Qt::AspectRatioMode rationmode;
    int imgsize = 138;
    int currpos = 0;  
    bool eventFilter(QObject *target, QEvent *event);
    void fillpics();
    explicit leftpanel(QWidget *parent = 0);
    ~leftpanel();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::leftpanel *ui;
};

#endif // LEFTPANEL_H
