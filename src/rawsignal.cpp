/* source file for rawsignal class -
   resposible for plotting raw EEG signal */

#include "rawsignal.h"
#include "ui_rawsignal.h"


rawsignal::rawsignal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rawsignal)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setWindowOpacity(0.75);
    numsmp=0;
    signal = QVector<double>(MAXPT);
    xc = QVector<double>(MAXPT);
    for (int j=0; j<MAXPT; j++)
    {
        signal[j]=0;
        xc[j]=0;
    }
    ui->plot->xAxis->setRange(0,1024);
    ui->plot->yAxis->setRange(-256,256);
    ui->plot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(6);
    legendFont.setBold(false);
    ui->plot->legend->setFont(legendFont);
    ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom);
    ui->plot->legend->setBrush(QBrush(QColor(255,255,255,190)));
    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(Qt::black));
    ui->plot->graph(0)->setName("Raw signal");
    ui->plot->graph(0)->setData(xc, signal);
    ui->plot->graph(0)->selectionDecorator()->setPen(QPen(Qt::black));
    ui->plot->setGeometry(0,0,1600,80);
    ui->plot->axisRect()->setAutoMargins(QCP::msNone);
    ui->plot->axisRect()->setMargins(QMargins(0,0,0,0));
    start=false;

    updateplot = new QTimer(this);
    updateplot->connect(updateplot,SIGNAL(timeout()), this, SLOT(updatePlotting()));
    updateplot->setInterval(100);

    QPen penpos;
    penpos.setWidth(2);
    penpos.setColor(Qt::yellow);
    curpos = new QCPItemLine(ui->plot);
    curpos->setPen(penpos);
    curpos->start->setCoords(0,-256);
    curpos->end->setCoords(0,256);
}



rawsignal::~rawsignal()
{
    delete ui;
}

rawsignal::updatesignal(int t)
{    
    signal[numsmp]=t*2;
    xc[numsmp]=numsmp;
    numsmp++;
    if (numsmp>1024)
        numsmp-=1025;
}

void rawsignal::starting()
{
    start=true;
    updateplot->start();
}

void rawsignal::changefsize(bool fl)
{
    if (fl)
    {
        setWindowOpacity(0.55);
        ui->plot->setGeometry(0,0,1940,80);
    }
    else
    {
        setWindowOpacity(0.75);
        ui->plot->setGeometry(0,0,1600,80);
    }
}

void rawsignal::updatepl()
{
    curpos->start->setCoords(numsmp,-256);
    curpos->end->setCoords(numsmp,256);
    ui->plot->graph(0)->setData(xc, signal);
    ui->plot->replot();
}

void rawsignal::updatePlotting()
{     
  //  if (numsmp>2048)
  //  {
  //      for (int i=0; i<1024; i++)
   //         signal[i]=signal[numsmp-i];
  //      ui->plot->xAxis->moveRange(-2048);
  //      numsmp-=1025;
  //  }
    curpos->start->setCoords(numsmp,-256);
    curpos->end->setCoords(numsmp,256);
    ui->plot->graph(0)->setData(xc, signal);
    ui->plot->replot();
}
