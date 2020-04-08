/* header file for rawsignal class -
   resposible for plotting raw EEG signal */

#ifndef RAWSIGNAL_H
#define RAWSIGNAL_H

#include <QWidget>
#include "qcustomplot.h"

#define MAXPT 1050

namespace Ui {
class rawsignal;
}

class rawsignal : public QWidget
{
    Q_OBJECT

public:
    int numsmp;
    QVector<double> signal,xc;
    QCPItemLine *curpos;
    explicit rawsignal(QWidget *parent = 0);
    ~rawsignal();
    updatesignal(int t);
    void starting();
    QTimer* updateplot;
    bool start;
    void changefsize(bool fl);
    void updatepl();

private:
    Ui::rawsignal *ui;    

private slots:

    void updatePlotting();
};

#endif // RAWSIGNAL_H
