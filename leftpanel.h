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
    QString picfolder;
    QStringList imglist;
    Qt::AspectRatioMode rationmode;
    int imgsize = 138;
    bool eventFilter(QObject *target, QEvent *event);
    void fillpics();
    void updateplaypic();
    explicit leftpanel(QWidget *parent = 0);
    ~leftpanel();

private slots:
    void on_pushButton_clicked();

private:
    Ui::leftpanel *ui;
};

#endif // LEFTPANEL_H
