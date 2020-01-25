#include "plotwindow.h"
#include "mainwindow.h"
#include "ui_plotwindow.h"
#include <iostream>
#include <QShortcut>
#include <hilbert.h>
#include <QFileDialog>
#include <settings.h>
#include <math.h>
#include <cmath>
#include <complex>
#include <stdio.h>
#include <vector>
#include <exception>
#include <algorithm>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "QTime"
#include "QSound"
#include <QAudioRecorder>
#include "QSoundEffect"
#include <bits/stdc++.h>
#include <windows.h>
#include "QThread"

// TO DO:

// improve randomwaves mode
// uploading any sounds for tones

const complex<double> I(0.0,1.0);

using namespace Eigen;

typedef std::vector<int> vectori;
typedef std::vector<double> vectord;
typedef std::complex<double> Complex;

plotwindow::plotwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plot)
{   
   // setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);    
    //datamap = QCPDataMap();
    //datamap_ptr = &datamap;
    QMenuBar* menu = new QMenuBar(this);
    QMenu* mainMenu = new QMenu("Menu");
    QMenu* mOpt = new QMenu("Options");
    QAction* acOpt = mOpt->addAction("Settings");   
    strLstM1 = new QStringListModel();
    strLstM1->setStringList(strLst1);
    strLstM2 = new QStringListModel();    
    strLstM2->setStringList(strLst2);
    strLstM3 = new QStringListModel();
    strLstM3->setStringList(strLst3);
    connect(acOpt, SIGNAL(triggered()),this,SLOT(slSettings()));
    menu->addMenu(mainMenu);
    menu->addMenu(mOpt);
    Bells = new QSound("C:/Users/willi/Documents/rec.wav");
    initfilterback=false;
    startnmmode=false; backimageloaded=false; hidebutt=false;
    maxtonerepeats=0; memorylength=2; antirepeat=true;
    drawshift=-150; graphcount=0; recnumb=20; edge=0;
    counter=0; startpos=0; ampval=70; tscale=12; recparts=0; transdelay=20; chnums=3, sampleblock=4;
    offlinedata=false; addmode=true; addmodeon=false; adaptampl=false; hideardata=true;
    estimpredstim=true; filtereddata=false; correctdelay=false;
    usefiltering=true; filterardata=false; carfilter=true; zerocr=true;
    recurbutter=false; filtfir=false; zerobutter=true; playsaved=false;
    tank1mode=false; tank2mode=true; spacemode=false;
    audioRecorder = new QAudioRecorder; micrecord=false; randmxt=false; pssstart=false;
    loopnum=10; maxtones=1; chorddelay=0; mxttimeout=5; volume=1;
    plv=0; averopttime=0;    
    rawsignalabove=true;
    flength=16; fsampr=512; flowpass=40;
    butterord=2; lcutoff=4; hcutoff=70;

    exlchannels = new int[64]; exch=64;
    indexes = new int [64];
    for (int i=0; i<64; i++)
        indexes[i]=0;
 //   eegdata = new QVector<double>[1000]; // max intervals number, 60/
    for (int i=0; i<20; i++)
    {
        eegdata[i] = QVector<double>(15360); //15360 - 30 sec, 153600 - max interval length, 5 min
        for (int j=0; j<15360; j++)
            eegdata[i][j]=0;
    }
    simeeg=true;
    colorizeback=false;
    NumC = new double [20];
    DenC = new double [20];
    sigb.lf=10; sigb.hf=12;
    noibp.lf=8; noibp.hf=14;
    noibs.lf=9; noibs.hf=13;
    samplenums=500; samplesize=32; eegintervals=8; intervallength=256; mindplay=false; blink=true;
    nextdrawshift=200;
    arrc.xc = QVector<double>(NMAX);
    arrc.amp0 = QVector<double>(NMAX);
    arrc.amp1 = QVector<double>(NMAX);
    arrc.amp2 = QVector<double>(NMAX);
    arrc.of1 = QVector<double>(NMAX);
    arrc.of2 = QVector<double>(NMAX);
    arrc.of3 = QVector<double>(NMAX);
    arrc.of4 = QVector<double>(NMAX);
    for (int j=0; j<NMAX; j++)
    {
        arrc.xc[j]=0;
        arrc.amp1[j]=0;
    }
    ui->setupUi(this);    
    ui->widget->installEventFilter(this);

    canbackchange=true;
    attentmodul=true;
    recplaying=false;
    tim = new QTimer(this);
    tim->connect(tim, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    tim->setInterval(5);
    scaletimeout=5;
    scaletim = new QTimer(this);
    scaletim->connect(scaletim, SIGNAL(timeout()), this, SLOT(scaletimerUpdate()));
    scaletim->setInterval(scaletimeout*1000);
    mxttim = new QTimer(this);
    mxttim->connect(mxttim, SIGNAL(timeout()), this, SLOT(mxttimerUpdate()));
    mxttim->setInterval(mxttimeout*1000);

    checkstates = new QTimer(this);
    checkstates->connect(checkstates,SIGNAL(timeout()), this, SLOT(checkstatesUpdate()));
    checkstates->setInterval(100);
    checkstates->start();

    colorchange = new QTimer(this);
    colorchange->connect(colorchange,SIGNAL(timeout()), this, SLOT(colorchangesUpdate()));
    colorchange->setInterval(100);

    bfiltmode=false;
    opencvstart=false;
    simsrfr=500;
    attentionvolume=true; volumebord=25;
    readyfortones=false;
    estimation=false;
    showprediction=false;
    corrprednumb=0;
    recordstarted=false;
    setaddmode(false);
    nums=1;
    delta=0; theta=0; alpha=0; beta=0; gamma=0; sgamma=0;
   // sdelta=13; stheta=25; salpha=25; sbeta=25; sgamma=8; shgamma=4;
    sdelta=0; stheta=0; salpha=0; sbeta=0; sgamma=0; shgamma=0;
    meandelta=0; meantheta=0; meanalpha=0; meanbeta=0; meangamma=0; meanhgamma=0;
    tunemode=true;
    tvals=new int[10];
    tv1=5; tv2=6;  tv3=6; tv4=7;  tv5=4; tv6=4;  tv7=3; tv8=4;  tv9=4; tv10=3;
    tvals[0]=tv1; tvals[1]=tv2;
    tvals[2]=tv3; tvals[3]=tv4;
    tvals[4]=tv5; tvals[5]=tv6;
    tvals[6]=tv7; tvals[7]=tv8;
    tvals[8]=tv9; tvals[9]=tv10;

    mindwstart=false;
    paintfstart=false;
    soundmodul=false;
    if (tank1mode)
    {
        ui->radioButton->setChecked(tank1mode);
        on_radioButton_clicked();
    }
    if (tank2mode)
    {
        ui->radioButton_2->setChecked(tank2mode);
        on_radioButton_2_clicked();
    }
    if (spacemode)
    {
        ui->radioButton_3->setChecked(spacemode);
        on_radioButton_3_clicked();
    }  

    tim->start();

    adaptivenumparts=true;

    filteringback = false;
    picchangeborder = 70;
    ui->spinBox_22->setValue(picchangeborder);

    sceneforfilt.addItem(&itemforfilt);
    resforfilt=QImage(QSize(1500, 800), QImage::Format_ARGB32);
    ptr = new QPainter(&resforfilt);

    splayer.init();

    folderpath="D:/PICS";
    fd.setPath(folderpath+"/");
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
}

plotwindow::~plotwindow()
{
    delete ui;
}

bool plotwindow::eventFilter(QObject *target, QEvent *event)
{
    if ((target == ui->widget) && (event->type() == QEvent::MouseMove))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPalette *p = new QPalette;
        p->setColor(QPalette::WindowText, Qt::darkBlue);
        ui->lcdNumber->setPalette(*p);
        ui->lcdNumber_2->setPalette(*p);
        ui->lcdNumber->display((int)ui->widget->xAxis->pixelToCoord(mouseEvent->pos().x()));
        ui->lcdNumber_2->display((int)ui->widget->yAxis->pixelToCoord(mouseEvent->pos().y()));      
    }
    if ((target == ui->widget) && (event->type() == QEvent::MouseButtonPress))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPalette *p = new QPalette;
        p->setColor(QPalette::WindowText, Qt::red);
        ui->lcdNumber->setPalette(*p);
        ui->lcdNumber_2->setPalette(*p);
        ui->lcdNumber->display((int)ui->widget->xAxis->pixelToCoord(mouseEvent->pos().x()));
        ui->lcdNumber_2->display((int)ui->widget->yAxis->pixelToCoord(mouseEvent->pos().y()));
       // if (mouseEvent->button() == Qt::RightButton)
      //  {
      //      offlinedata=true;
     //       ui->spinBox->setValue((int)ui->widget->xAxis->pixelToCoord(mouseEvent->pos().x()));
      //      on_pushButton_clicked();
       // }
        if (mouseEvent->button() == Qt::LeftButton)
        {           
            ui->widget->setFocus();
            hnt->numst=ui->spinBox_7->value();
            if (mindwstart)
                hnt->imlength=ui->spinBox_5->value()/1.953125;
            else if (simeeg)
            {
                hnt->imlength=ui->spinBox_5->value()/2;
                hnt->srfr=500;
            }
            intlen=(double)hnt->numst/((double)hnt->srfr/hnt->imlength);
            ui->widget->graph(graphcount)->setName("EEG: ["+QString::number(graphcount*intlen,'g',2)+" "+QString::number((graphcount+1)*intlen,'g',2)+"] secs");
        }
        if (mouseEvent->button() == Qt::MiddleButton)
        {
            if (tim->isActive())
                tim->stop();
            else
                tim->start();
            hnt->numst=ui->spinBox_7->value();
            if (mindwstart)
                hnt->imlength=ui->spinBox_5->value()/1.953125;
            else if (simeeg)
            {
                hnt->imlength=ui->spinBox_5->value()/2;
                hnt->srfr=500;
            }
            intlen=(double)hnt->numst/((double)hnt->srfr/hnt->imlength);
            ui->widget->graph(graphcount)->setName("EEG: ["+QString::number(graphcount*intlen,'g',2)+" "+QString::number((graphcount+1)*intlen,'g',2)+"] secs");

        }
        if ((mouseEvent->button() == Qt::RightButton) && ((appcn->ready) || (mindwstart) || (simeeg)))
        {
            hnt->numst=ui->spinBox_7->value();
            if (mindwstart)
                hnt->imlength=ui->spinBox_5->value()/1.953125;
            else if (simeeg)
            {
                hnt->srfr=500;
                hnt->imlength=ui->spinBox_5->value()/2;
            }
            intlen=(double)hnt->numst/((double)hnt->srfr/hnt->imlength);
            ui->widget->graph(graphcount)->setName("EEG: ["+QString::number(graphcount*intlen,'g',2)+" "+QString::number((graphcount+1)*intlen,'g',2)+"] secs");
            if ((addmode) && (addmodeon))
            {
                addmodeon=false;
               // graphcount=0;
            }
            else
            if ((addmode) && (!addmodeon))
            {
                recparts=0;
                eegintervals=ui->spinBox_6->value();
                hnt->numst=ui->spinBox_7->value();
               // mw->setintlength(hnt->imlength);
                recnumb=hnt->numst;
                startpos=(int)ui->widget->xAxis->pixelToCoord(mouseEvent->pos().x());                
                addmodeon=true;
                if (ui->checkBox_4->isChecked())
                    mindplay=true;                
                printtoresultstring("");
                printtoresultstring("Start of recording:    samlping rate "+QString::number(hnt->srfr)+"    part length "+QString::number(hnt->imlength));
            }
            else           
            {
              /*  startpos=(int)ui->widget->xAxis->pixelToCoord(mouseEvent->pos().x());
                hnt->imlength=ui->spinBox_5->value();
                hnt->imstprop=ui->doubleSpinBox_3->value();
                hnt->stlength=hnt->imlength*hnt->imstprop;
                stims=0;
                cleareeg(startpos,startpos+(hnt->imlength+hnt->stlength)*hnt->numst);
                clearstim(startpos,(hnt->imlength+hnt->stlength)*hnt->numst);
              //  ui->widget->graph(0)->setData(arrc.xc, arrc.amp1);
               // ui->widget->graph(3)->setData(arrc.xc, arrc.of1);
               // ui->widget->replot();
                tim->start();*/
            }
        }        
    }


    if ((target == ui->widget) && (event->type() == QEvent::MouseButtonDblClick))
    {
       // on_pushButton_24_clicked();
    }

    if ((target == ui->widget) && (event->type() == QEvent::KeyPress))
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        pressedKeys += ((QKeyEvent*)event)->key();

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_1) )
        {
            if (tvals[0]>0)
                tvals[0]--;
            ui->spinBox_2->setValue(tvals[0]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_1) )
        {
            if (tvals[0]<12)
                tvals[0]++;
            ui->spinBox_2->setValue(tvals[0]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_2) )
        {
            if (tvals[1]>0)
                tvals[1]--;
            ui->spinBox_3->setValue(tvals[1]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_2) )
        {
            if (tvals[1]<12)
                tvals[1]++;
            ui->spinBox_3->setValue(tvals[1]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_3) )
        {
            if (tvals[2]>0)
                tvals[2]--;
            ui->spinBox_4->setValue(tvals[2]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_3) )
        {
            if (tvals[2]<12)
                tvals[2]++;
            ui->spinBox_4->setValue(tvals[2]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_4) )
        {
            if (tvals[3]>0)
                tvals[3]--;
            ui->spinBox_9->setValue(tvals[3]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_4) )
        {
            if (tvals[3]<12)
                tvals[3]++;
            ui->spinBox_9->setValue(tvals[3]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_5) )
        {
            if (tvals[4]>0)
                tvals[4]--;
            ui->spinBox_10->setValue(tvals[4]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_5) )
        {
            if (tvals[4]<12)
                tvals[4]++;
            ui->spinBox_10->setValue(tvals[4]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_6) )
        {
            if (tvals[5]>0)
                tvals[5]--;
            ui->spinBox_11->setValue(tvals[5]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_6) )
        {
            if (tvals[5]<12)
                tvals[5]++;
            ui->spinBox_11->setValue(tvals[5]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_7) )
        {
            if (tvals[6]>0)
                tvals[6]--;
            ui->spinBox_12->setValue(tvals[6]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_7) )
        {
            if (tvals[6]<12)
                tvals[6]++;
            ui->spinBox_12->setValue(tvals[6]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_8) )
        {
            if (tvals[7]>0)
                tvals[7]--;
            ui->spinBox_13->setValue(tvals[7]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_8) )
        {
            if (tvals[7]<12)
                tvals[7]++;
            ui->spinBox_13->setValue(tvals[7]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_9) )
        {
            if (tvals[8]>0)
                tvals[8]--;
            ui->spinBox_14->setValue(tvals[8]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_9) )
        {
            if (tvals[8]<7)
                tvals[8]++;
            ui->spinBox_14->setValue(tvals[8]);
        }

        if( pressedKeys.contains(Qt::Key_Down) && pressedKeys.contains(Qt::Key_0) )
        {
            if (tvals[9]>0)
                tvals[9]--;
            ui->spinBox_15->setValue(tvals[9]);
        }

        if( pressedKeys.contains(Qt::Key_Up) && pressedKeys.contains(Qt::Key_0) )
        {
            if (tvals[9]<4)
                tvals[9]++;
            ui->spinBox_15->setValue(tvals[9]);
        }

        // add yAxis->moveRange on Key_Up and Key_Down

        if (keyEvent->key() == Qt::Key_Left)
        { double low = ui->widget->xAxis->range().lower;
            double high = ui->widget->xAxis->range().upper;
            ui->widget->xAxis->moveRange(-((high - low) / stepsPerPress));
            ui->widget->replot();
        }

        if (keyEvent->key() == Qt::Key_P)
        {
            QPixmap pmx = ui->widget->grab();
            setbackimage(pmx);
        }

        if (keyEvent->key() == Qt::Key_Right)
        {double low = ui->widget->xAxis->range().lower;
            double high = ui->widget->xAxis->range().upper;
            ui->widget->xAxis->moveRange((high - low) / stepsPerPress);
            ui->widget->replot();
        }
        if (keyEvent->key()==Qt::Key_Control)
        {
            ui->widget->axisRect()->setRangeZoom(Qt::Horizontal);
        }
        if (keyEvent->key()==Qt::Key_Alt)
        {
            ui->widget->axisRect()->setRangeZoom(Qt::Vertical);
        }
        if (keyEvent->key()==Qt::Key_B)
        {
            cleanbuttons();
            play_fdiez();
        }
        if (keyEvent->key()==Qt::Key_C)
        {
            cleanbuttons();
            play_Elow();
        }
        if (keyEvent->key()==Qt::Key_F)
        {
            cleanbuttons();
            play_Blow();
        }
        if (keyEvent->key()==Qt::Key_E)
        {
            cleanbuttons();
            play_b();
        }
        if (keyEvent->key()==Qt::Key_V)
        {
            cleanbuttons();
            play_Clow();
        }
        if (keyEvent->key()==Qt::Key_D)
        {
            cleanbuttons();
            play_Dlow();
        }
        if (keyEvent->key()==Qt::Key_X)
        {
            cleanbuttons();
            play_d();
        }
        if (keyEvent->key()==Qt::Key_G)
        {
            cleanbuttons();
            play_Glow();
        }
        if (keyEvent->key()==Qt::Key_T)
        {
            cleanbuttons();
            play_g();
        }
        if (keyEvent->key()==Qt::Key_Space)
        {
            cleanbuttons();
            play_a();
        }
        if (keyEvent->key()==Qt::Key_Tab)
        {
            this->hide();
            pss->paintf->show();
            pss->paintf->setFocus();
        }
        if (keyEvent->key()==Qt::Key_CapsLock)
        {
            if (rawsignalabove)
            {
                rws->move(165,900);
                rawsignalabove=false;
            } else
            {
                rws->move(165,0);
                rawsignalabove=true;
            }
        }

    }
    if ((target == ui->widget) && (event->type()==QEvent::KeyRelease))
    {
        pressedKeys -= ((QKeyEvent*)event)->key();
    }
    return false;
}

void plotwindow::printtoresultstring(QString str)
{
    strLst1.push_front(str);
    strLstM1->setStringList(strLst1);
}

void plotwindow::printtoresultbox(QString str)
{
    strLst2.push_front(str);
    strLstM2->setStringList(strLst2);
}

void plotwindow::printtoresultmean(QString str)
{
    strLst3.push_front(str);
    strLstM3->setStringList(strLst3);
}

void plotwindow::doplot()
{           
    ui->widget->setGeometry(50,50,1500,750);// ->setFixedSize(1500,570);

    QPalette* palette1 = new QPalette();
    palette1->setColor(QPalette::ButtonText,Qt::blue);
    ui->pushButton_7->setPalette(*palette1);
    QPalette* palette2 = new QPalette();
    palette2->setColor(QPalette::ButtonText,Qt::darkGreen);
    ui->pushButton_8->setPalette(*palette2);
    QPalette* palette3 = new QPalette();
    palette3->setColor(QPalette::ButtonText,Qt::red);
    ui->pushButton_9->setPalette(*palette3);
    QPalette* palette4 = new QPalette();
    palette4->setColor(QPalette::ButtonText,Qt::darkBlue);
    ui->pushButton_10->setPalette(*palette4);
    QPalette* palette5 = new QPalette();
    palette5->setColor(QPalette::ButtonText,Qt::yellow);
    ui->pushButton_11->setPalette(*palette5);
    QColor orangeColor(255,165,0);
    QPalette* palette6 = new QPalette();
    palette6->setColor(QPalette::ButtonText,orangeColor);
    ui->pushButton_12->setPalette(*palette6);
    QPalette* palette7 = new QPalette();
    palette7->setColor(QPalette::ButtonText,orangeColor);
    ui->pushButton_13->setPalette(*palette7);
    QPalette* palette8 = new QPalette();
    palette8->setColor(QPalette::ButtonText,Qt::darkMagenta);
    ui->pushButton_14->setPalette(*palette8);
    QPalette* palette9 = new QPalette();
    palette9->setColor(QPalette::ButtonText,Qt::darkMagenta);
    ui->pushButton_15->setPalette(*palette9);
    QPalette* palette10 = new QPalette();
    palette10->setColor(QPalette::ButtonText,Qt::blue);
    ui->pushButton_16->setPalette(*palette10);

    QPalette* palette21 = new QPalette();
    palette21->setColor(QPalette::ButtonText,Qt::darkGreen);
    ui->pushButton_21->setPalette(*palette21);
    ui->pushButton_21->setText("Record..");
    ui->pushButton_21->setVisible(false);
    ui->pushButton_22->setVisible(false);


    sp1.setColor(QPalette::Window, Qt::white);
    sp1.setColor(QPalette::WindowText, Qt::red);

    sp2.setColor(QPalette::Window, Qt::white);
    sp2.setColor(QPalette::WindowText, Qt::darkGreen);

    fixback = true;

    ui->label_23->setPalette(sp1);
    ui->label_23->setGeometry(140,20,181,26);
    ui->label_24->setPalette(sp2);
    ui->label_24->setGeometry(350,20,181,26);
    ui->label_24->setVisible(false);
    ui->progressBar->setGeometry(700,26,236,20);
    ui->progressBar->setPalette(sp1);

   // ui->pushButton_9->setStyleSheet("QPushButton{background: rgb(255, 0, 0); }");

    ui->checkBox->setGeometry(100,950,131,20);
    ui->checkBox->setChecked(tunemode);
    ui->label->setGeometry(250,950,20,16);
    ui->spinBox_2->setGeometry(280,950,40,21);
    ui->label_2->setGeometry(330,950,20,16);
    ui->spinBox_3->setGeometry(360,950,40,21);

    ui->label_4->setGeometry(415,950,25,20);
    ui->spinBox_4->setGeometry(440,950,40,21);
    ui->label_3->setGeometry(490,950,23,16);
    ui->spinBox_9->setGeometry(520,950,40,21);
    ui->label_8->setGeometry(570,950,20,16);
    ui->spinBox_10->setGeometry(600,950,40,21);
    ui->label_5->setGeometry(650,950,20,16);
    ui->spinBox_11->setGeometry(680,950,40,21);
    ui->label_14->setGeometry(730,950,23,16);
    ui->spinBox_12->setGeometry(760,950,40,21);
    ui->label_13->setGeometry(810,950,23,16);
    ui->spinBox_13->setGeometry(840,950,40,21);
    ui->label_16->setGeometry(890,950,23,16);
    ui->spinBox_14->setGeometry(920,950,40,21);
    ui->label_15->setGeometry(970,950,23,16);
    ui->spinBox_15->setGeometry(1000,950,40,21);

    ui->pushButton_18->setGeometry(1050,950,71,22);
    ui->pushButton_19->setGeometry(1129,950,71,22);
    ui->pushButton_17->setGeometry(1215,950,88,22);
    ui->pushButton_20->setGeometry(1312,950,88,22);
    ui->pushButton_21->setGeometry(1360,922,66,22);
    ui->spinBox_22->setGeometry(1522,920,40,20);
    ui->label_25->setGeometry(1430,915,91,35);
    ui->checkBox_13->setGeometry(1430,950,80,20);
    ui->pushButton_22->setGeometry(1360,904,66,18);

    ui->checkBox_2->setGeometry(1055,920,155,25);
    ui->label_17->setGeometry(1220,922,87,21);
    ui->spinBox_16->setGeometry(1317,922,40,21);
    ui->spinBox_16->setValue(scaletimeout);

    ui->spinBox_2->setValue(tvals[0]);
    ui->spinBox_3->setValue(tvals[1]);
    ui->spinBox_4->setValue(tvals[2]);
    ui->spinBox_9->setValue(tvals[3]);
    ui->spinBox_10->setValue(tvals[4]);
    ui->spinBox_11->setValue(tvals[5]);
    ui->spinBox_12->setValue(tvals[6]);
    ui->spinBox_13->setValue(tvals[7]);
    ui->spinBox_14->setValue(tvals[8]);
    ui->spinBox_15->setValue(tvals[9]);

    ui->listView->setGeometry(97,869,940,45);
    ui->listView->setModel(strLstM1);
    ui->listView->show();
    ui->listView->setAutoScroll(true);
    ui->listView_2->setGeometry(1055,869,135,45);
    ui->listView_2->setModel(strLstM2);
    ui->listView_2->show();
    ui->listView_2->setAutoScroll(true);
    ui->listView_3->setGeometry(97,919,940,24);
    ui->listView_3->setModel(strLstM3);
    ui->listView_3->show();
    ui->listView_3->setAutoScroll(true);

    ui->label_7->setGeometry(675,810,140,25);
    ui->label_7->setVisible(false);
    ui->spinBox_6->setGeometry(800,810,40,25);
    ui->spinBox_6->setVisible(false);
    ui->checkBox_3->setGeometry(800,810,100,25);
    ui->pushButton_24->setGeometry(920,810,90,25);
    ui->pushButton_4->setGeometry(1040,810,70,25);

    ui->spinBox_7->setGeometry(610,810,50,25);
    ui->spinBox_7->setEnabled(false);

    ui->checkBox_8->setGeometry(1330,870,125,25);
    ui->checkBox_8->setVisible(false);

    ui->checkBox_6->setChecked(false);
    ui->checkBox_4->setGeometry(1210,840,125,25);
    ui->checkBox_6->setGeometry(1210,860,125,25);
    ui->checkBox_7->setGeometry(1210,880,125,25);
    ui->checkBox_5->setGeometry(1210,900,125,25);

    ui->radioButton->setGeometry(1330,846,95,20);
    ui->pushButton_25->setGeometry(1436,810,120,21);
    ui->checkBox_14->setGeometry(1430,833,130,20);
    ui->checkBox_11->setGeometry(1430,853,145,20);
    ui->checkBox_12->setGeometry(1430,873,130,20);
    ui->comboBox->setGeometry(1430,893,90,20);
    ui->radioButton_2->setGeometry(1330,866,95,20);
    ui->radioButton_3->setGeometry(1330,886,95,20);

    ui->pushButton->setGeometry(1200,880,70,25);
    ui->pushButton_6->setGeometry(15,869,75,25);
    ui->pushButton_23->setGeometry(15,889,75,25);
  //  ui->pushButton_24->setGeometry(15,919,80,25);
    ui->pushButton_6->setVisible(true);
    ui->pushButton->setVisible(false);

    ui->checkBox_13->setChecked(fixback);
    ui->checkBox_7->setChecked(blink);
    ui->widget->setFocus();
    ui->spinBox_6->setValue(eegintervals);
    ui->spinBox_7->setValue(hnt->numst);
    ui->checkBox_3->setChecked(usefiltering);
    ui->lcdNumber->setGeometry(100,35,70,25);
    ui->lcdNumber_2->setGeometry(190,35,70,25);
    QPalette *p = new QPalette(Qt::green);      
    ui->lcdNumber_4->setPalette(*p);    
    ui->spinBox_8->setGeometry(205,810,35,25);
    ui->spinBox_17->setGeometry(195,838,45,25);
    ui->checkBox_9->setGeometry(285,840,130,25);
    ui->label_19->setGeometry(425,840,80,25);
    ui->spinBox_18->setGeometry(510,840,45,25);
    ui->spinBox_18->setValue(memorylength);
    ui->label_20->setGeometry(570,840,85,25);
    ui->spinBox_19->setGeometry(666,840,45,25);
    ui->checkBox_10->setGeometry(730,840,140,25);
    ui->label_21->setGeometry(875,840,90,25);
    ui->spinBox_20->setGeometry(970,840,35,25);
    ui->label_22->setGeometry(1030,833,50,25);
    ui->horizontalSlider_3->setGeometry(1030,855,50,12);
    ui->horizontalSlider_3->setValue(100);
    ui->spinBox_21->setGeometry(1090,840,45,25);
    ui->spinBox_20->setValue(mxttimeout);
    ui->spinBox_19->setValue(maxtonerepeats);
    ui->checkBox_9->setChecked(antirepeat);
    ui->label_12->setGeometry(510,810,92,25);
    ui->label_6->setGeometry(285,810,140,25);
    ui->horizontalSlider->setGeometry(285,830,120,12);
    if (mindwstart)
        ui->horizontalSlider->setValue(hnt->imlength*1.953125);
    else if (simeeg)
        ui->horizontalSlider->setValue(hnt->imlength*2);
    ui->horizontalSlider->setTickInterval(50);
    ui->horizontalSlider->setSingleStep(50);
    ui->spinBox_5->setGeometry(425,810,50,25);
    ui->lcdNumber_4->setGeometry(210,70,50,25);  
    ui->label_9->setGeometry(50,810,150,25);
    ui->label_18->setGeometry(50,835,140,25);
    ui->label_10->setGeometry(115,70,140,25);
    ui->spinBox_8->setValue(maxtones);
    ui->lcdNumber_4->display(hnt->srfr/hnt->osfr);
    ui->lcdNumber_4->hide();
    ui->lcdNumber_2->hide();
    ui->lcdNumber->hide();
    ui->label_10->hide();
    stepsPerPress = 10;
    startpos=hnt->posstim;
    if (mindwstart)
        ui->spinBox_5->setValue(hnt->imlength*1.953125);
    else if (simeeg)
        ui->spinBox_5->setValue(hnt->imlength*2);
  //  ui->widget->xAxis->grid()->setVisible(false);
  //  ui->widget->yAxis->grid()->setVisible(false);
  //  ui->widget->replot();
    if (opencvstart)
        enablehue();
    plot(ui->widget);
}

void plotwindow::plot(QCustomPlot *customPlot)
{    
    if (start) return;
    sw = new Settings();
   /* QCPItemText *textLabel1 = new QCPItemText(customPlot);
    customPlot->addItem(textLabel1);
    textLabel1->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel1->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel1->position->setCoords(0.5, 0); // place position at center/top of axis rect
    textLabel1->setText("Data from 1-sensor EEG NeuroSky MindWave");
//  MiddleButton in 'Additional mode' - start/stop EEG recording or stimulation
//  RightButton / RUN Button - offline mode with preloaded data");
    textLabel1->setFont(QFont(font().family(), 10));
    textLabel1->setPen(QPen(Qt::gray)); */
    customPlot->addGraph();
    customPlot->legend->setVisible(false);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    legendFont.setBold(true);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));    
  //  customPlot->addGraph();
   // cleareeg(0,NMAX);
   // qDebug()<<"1";
    customPlot->graph(0)->setPen(QPen(Qt::green));
    intlen=(double)hnt->numst/((double)hnt->srfr/hnt->imlength);
    customPlot->graph(0)->setName("EEG: [0 "+QString::number(intlen,'g',2)+"] secs");
    customPlot->graph(0)->setData(arrc.xc, arrc.amp1);
    graphcount=0;
    setaddmode(true);
    ui->checkBox_4->setChecked(true);
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");    
    customPlot->rescaleAxes();
    customPlot->xAxis->setRange(150, hnt->imlength * 22);
    customPlot->yAxis->setRange(-1600, 0);
    customPlot->xAxis->moveRange(hnt->posstim - hnt->srfr / 2);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->axisRect()->setRangeZoom(Qt::Vertical);
    zerophaseinit(lcutoff,hcutoff,butterord,hnt->srfr);
    start=true;
}

void plotwindow::clearstim(int posstim, int length)
{
    if ((hronarrun))
    for (int i = posstim; i < posstim+length; i++)
        arrc.of3[i]=0;
    else
    for (int i = posstim; i < posstim+length; i++) {
        arrc.of1[i]=0;
        arrc.of2[i]=0;
    }
}

void plotwindow::clearstim()
{
    for (int i=0; i<NMAX; i++) {
        arrc.of1[i]=0;
        arrc.of2[i]=0;
        arrc.of3[i]=0;
        arrc.of4[i]=0;
    }

    ui->widget->replot();
}

void plotwindow::cleareeg(int start, int end)
{
    for (int i=start; i<=end; i++)
        arrc.amp1[i]=0;
    ui->widget->graph(0)->setData(arrc.xc, arrc.amp1);
    ui->widget->replot();
}

void plotwindow::clearfiltered()
{
    for (int i=0; i<NMAX; i++)
        arrc.amp2[i]=0;
    ui->widget->graph(1)->setData(arrc.xc, arrc.amp2);
    ui->widget->replot();
}

void plotwindow::updatedata(int start)
{
    int pos=0;
    if ((recparts>0) && (eegdata[graphcount][start-2]==0))
    {
        for (int j=start-2; j>0; j--)
            if (!eegdata[graphcount][j]==0)
            {
                pos=j+1;
                break;
            }
        start=pos;
    }

    for (int i=0; i<hnt->imlength; i++)
        eegdata[graphcount][start+i]=drawshift+arrc.amp0[i];
}

void plotwindow::update_attention(int t)
{

    attent=t;
    ui->label_23->setText("ATTENTION: "+QString::number(t)+"%");
    if ((attentionvolume) && (t>volumebord))
        on_horizontalSlider_3_valueChanged(t);

    if (attentmodul)
        ui->progressBar->setValue(t);

    if ((!fixback) && (attentmodul) && (backimageloaded) && (t<picchangeborder))
        if (!canbackchange)
            canbackchange=true;

    if ((!fixback) && (attentmodul) && (backimageloaded) && (t>picchangeborder)) // && (filteringback)
        if (canbackchange)
        {
            on_pushButton_6_clicked();
            canbackchange=false;
        }
}


void plotwindow::update_meditation(int t)
{
    meditt = t;
    ui->label_24->setText("MEDITATION: "+QString::number(t)+"%");

    if (!attentmodul)
        ui->progressBar->setValue(t);

    if ((!fixback) && (!attentmodul) && (backimageloaded) && (t>picchangeborder)) // && (filteringback)
        on_pushButton_6_clicked();
  /*  if (t>70)
    {
        if (!tank2mode)
        {
            on_radioButton_2_clicked();
            ui->radioButton_2->setChecked(true);
        }
    }
    else
 //   if (t<60)
    {
        if (!spacemode)
        {
            on_radioButton_3_clicked();
            ui->radioButton_3->setChecked(true);
        }
    }
  //  else
    //   on_radioButton_2_clicked(); */
}

void plotwindow::radiobut1()
{
    on_radioButton_clicked();
    ui->radioButton->setChecked(true);
}

void plotwindow::radiobut2()
{
    on_radioButton_2_clicked();
    ui->radioButton_2->setChecked(true);
}

void plotwindow::radiobut3()
{
    on_radioButton_3_clicked();
    ui->radioButton_3->setChecked(true);
}

void plotwindow::enablenumparts(bool fl)
{
    ui->spinBox_7->setEnabled(fl);
}

void plotwindow::enablehue()
{
    if (!ui->checkBox_14->isChecked())
        ui->checkBox_11->setEnabled(true);
}

void plotwindow::setpicfolder(QString fp)
{
    folderpath=fp;
    fd.setPath(fp+"/");
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
}

QImage plotwindow::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent)
{
    if(src.isNull()) return QImage();   //No need to do anything else!
    if(!effect) return src;             //No need to do anything else!
    if (initfilterback)
        delete pmvr;
    pmvr = new QPixmap(QPixmap::fromImage(src));
    if (!initfilterback)
        initfilterback=true;
    itemforfilt.setPixmap(*pmvr);
    itemforfilt.setGraphicsEffect(effect);  
    resforfilt.fill(Qt::transparent);
    sceneforfilt.render(ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ),Qt::IgnoreAspectRatio);
    return resforfilt;
}

void plotwindow::grabopencv(QString fpath)
{
    backimg.load(fpath);
    backimg.scaled(1400,700,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();
    ui->checkBox_6->setChecked(false);
    if (!backimageloaded)
        backimageloaded=true;
}

void plotwindow::applyfilteronback()
{
    //if (!backimg.isNull())
    {      
        blurp = new QGraphicsBlurEffect;
        colorizep = new QGraphicsColorizeEffect;

        blurp->setBlurRadius((100-attent)/12);

        if (colorizeback)
        {
           // colorize->setColor(QColor(pw->alpha*5,256-pw->beta*5,256-pw->gamma*6,pw->meditt*2));
            colorizep->setColor(QColor(beta*4,theta*4,alpha*4,meditt*2));
            colorizep->setStrength((double)attent/70);
        }

        if ((opencvstart) && (!colorizeback))
        {
            if (!backimageloaded)
                backimageloaded=true;
            backimg = QPixmap::fromImage(mw->grabopcvpic());
        }

        QM = backimg.toImage();
        qbim1 = applyEffectToImage(QM, blurp, 0);

        if (colorizeback)
            qbim2 = applyEffectToImage(qbim1, colorizep, 0);

        delete pmvr;
        if (!colorizeback)
            pmvr = new QPixmap(QPixmap::fromImage(qbim1));
        else
            pmvr = new QPixmap(QPixmap::fromImage(qbim2));

        ui->widget->setBackground(*pmvr,true,Qt::IgnoreAspectRatio);

    }
}

void plotwindow::setbackimage(QPixmap pm)
{
    backimageloaded=true;
    backimg=pm;
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();
    ui->checkBox_6->setChecked(false);
}

void plotwindow::bcidata(double d1)
{
    arrc.amp0[counter]=d1;
        counter++; 
  //  xraw=(int)d1;
}

void plotwindow::settonesvolume()
{
   splayer.setvolume(volume);
}

// B G  C  E  D  g f# a  d  b
// B D# G# F# C# b d# g# f# c#

void plotwindow::play_Glow()
{
    ui->pushButton_7->setDown(true);
    if (spacemode)
        emit splayer.playD3();
    else if (tank1mode)
        emit splayer.playGlow();
    else
        emit splayer.playtone_Dlowsh();
}

void plotwindow::play_fdiez()
{
    ui->pushButton_8->setDown(true);
    if (spacemode)
        emit splayer.playA3();
    else if (tank1mode)
        emit splayer.playfdiez();
    else
        emit splayer.playtone_dsh();
}

void plotwindow::play_Clow()
{
    ui->pushButton_9->setDown(true);
    if (spacemode)
        emit splayer.playC4();
    else if (tank1mode)
        emit splayer.playClow();
    else
        emit splayer.playtone_Glowsh();

}

void plotwindow::play_a()
{
    ui->pushButton_10->setDown(true);
    if (spacemode)
        emit splayer.playA4();
    else if (tank1mode)
        emit splayer.playatone();
    else
        emit splayer.playtone_gsh();
}

void plotwindow::play_Elow()
{
    ui->pushButton_11->setDown(true);
    if (spacemode)
        emit splayer.playD4();
    else if (tank1mode)
        emit splayer.playElow();
    else
        emit splayer.playtone_Flowsh();
}

void plotwindow::play_d()
{
    ui->pushButton_12->setDown(true);
    if (spacemode)
        emit splayer.playE4();
    else if (tank1mode)
        emit splayer.playdtone();
    else
        emit splayer.playtone_fsh();
}

void plotwindow::play_Dlow()
{
    ui->pushButton_13->setDown(true);
    if (spacemode)
        emit splayer.playF3();
    else if (tank1mode)
        emit splayer.playDlow();
    else
        emit splayer.playtone_Clowsh();
}

void plotwindow::play_b()
{    
    if (tank1mode)            
    {
        ui->pushButton_14->setDown(true);
        emit splayer.playbtone();
    }
    else if (tank2mode)
    {
        ui->pushButton_14->setDown(true);
        emit splayer.playtone_csh();
    }
}

void plotwindow::play_Blow()
{
    ui->pushButton_15->setDown(true);
    if (spacemode)
        emit splayer.playF4();
    else if (tank1mode)
        emit splayer.playBlow();
    else    
        emit splayer.playtone_Blow();
}

void plotwindow::play_g()
{
    if (tank1mode)
    {
        ui->pushButton_16->setDown(true); 
        emit splayer.playgtone();
    } else
    if (tank2mode)
    {
        ui->pushButton_16->setDown(true); 
        emit splayer.playtone_b();
    }
}

void plotwindow::cleanbuttons()
{
    ui->pushButton_7->setDown(false);
    ui->pushButton_8->setDown(false);
    ui->pushButton_9->setDown(false);
    ui->pushButton_10->setDown(false);
    ui->pushButton_11->setDown(false);
    ui->pushButton_12->setDown(false);
    ui->pushButton_13->setDown(false);
    ui->pushButton_14->setDown(false);
    ui->pushButton_15->setDown(false);
    ui->pushButton_16->setDown(false);
}

void plotwindow::colorchangesUpdate()
{

}

void plotwindow::checkstatesUpdate()
{
    if ((GetKeyState('Q') & 0x8000) && (startnmmode==false))
        startnmmode=true;
    if ((GetKeyState('E') & 0x8000) && (startnmmode))
        startnmmode=false;

    if ((startnmmode) && (attent>50))
        pushenter();

    psleep = 10;

    if ((mindwstart) && (startnmmode))
    {

        if (alpha<meanalpha)
            pushleft();
        else if (alpha>meanalpha)
            pushright();

        if (beta>meanbeta)
            pushup();
        else if (beta<meanbeta)
            pushdown();
    }
}

void plotwindow::tn1Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((delta>meandelta+tvals[0]) && (!spacemode) && (!tones.contains("b")) && (!tones.contains("c#")))
            tonenumbers++;
        if (tank1mode)
            tones+="b ";
        else if (tank2mode)
            tones+="c# ";
    }
}

void plotwindow::tn2Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((delta<meandelta-tvals[1]) && (!tones.contains("B")) && (!tones.contains("F4")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="B ";
            else if (tank2mode)
                tones+="B ";
            else
                tones+="F4 ";
        }
    }
}

void plotwindow::tn3Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((theta>meantheta+tvals[2]) && (!spacemode) && (!tones.contains("g")) && (!tones.contains("b")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="g ";
            else if (tank2mode)
                tones+="b ";
        }
    }
}

void plotwindow::tn4Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((theta<meantheta-tvals[3]) && (!tones.contains("G")) && (!tones.contains("D#")) && (!tones.contains("D3")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="G ";
            else if (tank2mode)
                tones+="D# ";
            else
                tones+="D3 ";
        }
    }
}

void plotwindow::tn5Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((alpha>meanalpha+tvals[4]) && (!tones.contains("d")) && (!tones.contains("f#")) && (!tones.contains("E4")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="d ";
            else if (tank2mode)
                tones+="f# ";
            else
                tones+="E4 ";
        }
    }
}

void plotwindow::tn6Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((alpha<meanalpha-tvals[5]) && (!tones.contains("D")) && (!tones.contains("C#")) && (!tones.contains("F3")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="D ";
            else if (tank2mode)
                tones+="C# ";
            else
                tones+="F3 ";
        }
    }
}

void plotwindow::tn7Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((beta>meanbeta+tvals[6]) && (!tones.contains("f#")) && (!tones.contains("d#")) && (!tones.contains("A3")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="f# ";
            else if (tank2mode)
                tones+="d# ";
            else
                tones+="A3 ";
        }
    }
}

void plotwindow::tn8Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((gamma<meangamma-tvals[7]) && (!tones.contains("E")) && (!tones.contains("F#")) && (!tones.contains("D4")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="E ";
            else if (tank2mode)
                tones+="F# ";
            else
                tones+="D4 ";
        }
    }
}

void plotwindow::tn9Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((gamma>meangamma+tvals[8]) && (!tones.contains("C")) && (!tones.contains("G#")) && (!tones.contains("C4")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="C ";
            else if (tank2mode)
                tones+="G# ";
            else
                tones+="C4 ";
        }
    }
}

void plotwindow::tn10Update()
{
    if ((readyfortones) && (tonenumbers<maxtones))
    {
        if ((hgamma>meangamma+tvals[9]) && (!tones.contains("a")) && (!tones.contains("g#")) && (!tones.contains("A4")))
        {
            tonenumbers++;
            if (tank1mode)
                tones+="a ";
            else if (tank2mode)
                tones+="g# ";
            else
                tones+="A4 ";
        }
    }
}

void plotwindow::playUpdate()
{
    if (addmodeon)
    {
        int z=0;
        if ((antirepeat) && (strLst2.length()>memorylength))
        {
            for (int i=0; i<memorylength; i++)
                if (strLst2.at(i).toLocal8Bit().constData()==tones)
                    z++;
            if (z>maxtonerepeats)
                randomtone();
        }
        printtoresultbox(tones);
       // letsplay();
        //  readyfortones=false;
    }
}

void plotwindow::gettones()
{
 //   B G  C  E  D  g f# a  d  b
 //   B D# G# F# C# b d# g# f# c#

    tones="";
    if ((delta>meandelta+tvals[0]) && (!spacemode))
    {
        tonenumbers++;
        if (tank1mode)
            tones+="b ";
        else if (tank2mode)
            tones+="c# ";
    }
    if (delta<meandelta-tvals[1])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="B ";
            else if (tank2mode)
                tones+="B ";
            else
                tones+="F4 ";
        }
    }
    if ((theta>meantheta+tvals[2]) && (!spacemode))
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="g ";
            else if (tank2mode)
                tones+="b ";
        }
    }
    if (theta<meantheta-tvals[3])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="G ";
            else if (tank2mode)
                tones+="D# ";
            else
                tones+="D3 ";
        }
    }

    if (alpha>meanalpha+tvals[4])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="d ";
            else if (tank2mode)
                tones+="f# ";
            else
                tones+="E4 ";
        }
    }
    if (alpha<meanalpha-tvals[5])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="D ";
            else if (tank2mode)
                tones+="C# ";
            else
                tones+="F3 ";
        }
    }

    if (beta>meanbeta+tvals[6])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="f# ";
            else if (tank2mode)
                tones+="d# ";
            else
                tones+="A3 ";
        }
    }

    if (gamma<meangamma-tvals[7])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="E ";
            else if (tank2mode)
                tones+="F# ";
            else
                tones+="D4 ";
        }
    }
    if (gamma>meangamma+tvals[8])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="C ";
            else if (tank2mode)
                tones+="G# ";
            else
                tones+="C4 ";
        }
    }

    if (hgamma>meangamma+tvals[9])
    {
        tonenumbers++;
        if (tonenumbers<=maxtones)
        {
            if (tank1mode)
                tones+="a ";
            else if (tank2mode)
                tones+="g# ";
            else
                tones+="A4 ";
        }
    }
}

void plotwindow::randomtone()
{    
    tonenumbers=0;
    delta = meandelta - 7 + qrand() % 18;
    theta = meantheta - 7 + qrand() % 18;
    alpha = meanalpha - 7 + qrand() % 18;
    beta = meanbeta - 7 + qrand() % 18;
    gamma = meangamma - 4 + qrand() % 11;
    hgamma = meanhgamma - 4 + qrand() % 11;
    gettones();
}

void plotwindow::analysemeandata()
{
    deltanum=0; thetanum=0; alphanum=0; betanum=0; gammanum=0; hgammanum=0;

    int length = 2048; int bordfreq=70;
    for (int i=0; i<hnt->imlength; i++)
        t[i].real(arrc.amp0[i]);

    for (int i=hnt->imlength; i<length; i++)  // zero-padding
        t[i].real(0);

    for (int i=0; i<length; i++)
        t[i].imag(0);

    cdata = CArray(t,length);
    hnt->fft(cdata);
    double deltafr, thetafr, alphafr, betafr, gammafr, hgammafr, totalpow, temppow;
    deltafr=0; thetafr=0; alphafr=0; betafr=0; gammafr=0; hgammafr=0; totalpow=0;
    for (int i=1; i<bordfreq*4; i++)
    {
        temppow=2*sqrt(cdata[i].real()*cdata[i].real()+cdata[i].imag()*cdata[i].imag());
        totalpow+=temppow;
        if ((i>0) && (i<4*4))
            deltafr+=temppow;
        if ((i>=4*4) && (i<8*4))
            thetafr+=temppow;
        if ((i>=8*4) && (i<15*4))
            alphafr+=temppow;
        if ((i>=15*4) && (i<33*4))
            betafr+=temppow;
        if ((i>=33*4) && (i<50*4))
            gammafr+=temppow;
        if ((i>=50*4))
            hgammafr+=temppow;
    }
    deltafr/=totalpow;
    thetafr/=totalpow;
    alphafr/=totalpow;
    betafr/=totalpow;
    gammafr/=totalpow;
    hgammafr/=totalpow;

    delta=deltafr*100;
    theta=thetafr*100;
    alpha=alphafr*100;
    beta=betafr*100;
    gamma=gammafr*100;
    hgamma=hgammafr*100;

// cout<<fixed<<setprecision(2)<<delta<<" "<<theta<<" "<<alpha<<" "<<beta<<" "<<gamma<<" "<<hgamma<<endl;

  //  if ((startnmmode) && (attent>50))
  //     pushenter();
        // pushright();

    if ((addmodeon) || (mindplay))
    {
        nums++;
        sdelta+=delta; meandelta=(double)sdelta/nums;
        stheta+=theta; meantheta=(double)stheta/nums;
        salpha+=alpha; meanalpha=(double)salpha/nums;
        sbeta+=beta; meanbeta=(double)sbeta/nums;
        sgamma+=gamma; meangamma=(double)sgamma/nums;
        shgamma+=hgamma; meanhgamma=(double)shgamma/nums;
    }

    if (pssstart)
    {
        pss->paintf->updatefreqarrs(delta,theta,alpha,beta,gamma,hgamma);
       // if (opencvstart)
           // mw->setoverlay(pss->paintf->getestattval());
         //   mw->setattent(pss->paintf->getestattval());
    }

    if ((pssstart) && (bfiltmode) && (!pss->paintf->gamemode) && (!pss->paintf->flowmode))
    {
        if (!pss->drawflow)
            pss->applyfilter();
        else
            pss->applyfilteronbackimg();
    }

    if ((pssstart) && (bfiltmode) && (pss->paintf->gamemode))
    {
        pss->paintf->filtering_allpuzzles(5);
        pss->paintf->filteringmain_ingame(5);
    }

    if ((filteringback) && (!backimg.isNull()))
        applyfilteronback();

    if ((pssstart) && (pss->paintf->updateback))
    {
        pmx = ui->widget->grab();
        pss->paintf->setbackimage(pmx);
        pss->paintf->mainpic=pmx;
        pss->paintf->pmain=pmx;
    }

    //qDebug()<<"mdelta "<<meandelta<<" mtheta "<<meantheta<<" malpha "<<meanalpha<<" mbeta "<<meanbeta<<" mgamma "<<meangamma<<" mhgamma "<<meanhgamma<<endl;
    printtoresultmean("mdelta% "+QString::number(meandelta,'f',2)+"    mtheta% "+QString::number(meantheta,'f',2)+"    malpha% "+QString::number(meanalpha,'f',2)+"    mbeta% "+QString::number(meanbeta,'f',2)+"    mgamma% "+QString::number(meangamma,'f',2)+"    mhgamma% "+QString::number(meanhgamma,'f',2));
    if ((addmodeon) || (playsaved))
        printtoresultstring("delta%: "+QString::number(delta)+"       theta%: "+QString::number(theta)+"       alpha%: "+QString::number(alpha)+"       beta%: "+QString::number(beta)+"        gamma%: "+QString::number(gamma)+"       highgamma%: "+QString::number(hgamma));    
    //qDebug()<<"delta%: "+QString::number(delta)+"    theta%: "+QString::number(theta)+"    alpha%: "+QString::number(alpha)+"    beta%: "+QString::number(beta)+"     gamma%: "+QString::number(gamma)+"    highgamma%: "+QString::number(hgamma);

    // b B g G d D f# E C a
    // G D3, f# A3, C C4, a A4, E D4, d E4, D F3, b, B F4, g

    if ((mindplay) || (playsaved)) {

        cleanbuttons(); tonenumbers=0;
        tones="";

        gettones();                   // sequential version

        if ((antirepeat) && (strLst2.length()>memorylength))
        {
            lasttones=""; int z=0;
            for (int i=0; i<memorylength; i++)
            {
                lasttones+=strLst2.at(i).toLocal8Bit().constData();
                if (strLst2.at(i).toLocal8Bit().constData()==tones)
                    z++;
            }
            if (z>maxtonerepeats)
                randomtone();
        }

        printtoresultbox(tones);

        letsplay();

    }

    readyfortones=false;

    if (blink)
    {
        if (graphcount<8)
        for (int i=0; i<graphcount+1; i++)
            ui->widget->graph(i)->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
        else
        for (int i=graphcount-8; i<graphcount+1; i++)
            ui->widget->graph(i)->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    }

   // counter=0;
}

void plotwindow::letsplay()
{
    if (tank1mode)
        playtank1(tones);
    else if (tank2mode)
        playtank2(tones);
    else
        playspace(tones);
}

void plotwindow::scaletimerUpdate()
{
    ui->pushButton_20->clicked();
}

void plotwindow::mxttimerUpdate()
{
    maxtones= 1 + qrand()%3;
    ui->spinBox_8->setValue(maxtones);
}

void plotwindow::delay(int temp)
{
    QTime dieTime = QTime::currentTime().addMSecs(temp);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void plotwindow::cleareegdata()
{
    for (int i=0; i<20; i++)
    {
        for (int j=0; j<15360; j++) // 15360
            eegdata[i][j]=0;
    }
}

void plotwindow::cleanmem()
{
    delete[] eegdata;
}

void plotwindow::analysepart()
{
    if ((addmodeon) && (addmoderec))
    {        
        updatedata(recparts*hnt->imlength);        
        ui->widget->graph(graphcount)->setData(arrc.xc, eegdata[graphcount]);
        ui->widget->replot();
        recparts++;           
        if (recparts >= hnt->numst)
        {
            recparts=0;
            if (graphcount>eegintervals+2)
            {
                ui->widget->removeGraph(graphcount-eegintervals);
                for (int i=0; i<6000; i++)
                    eegdata[graphcount][i]=0;
                graphcount--;
            }
            if (graphcount>eegintervals-2)
                 ui->widget->yAxis->moveRange(-nextdrawshift);            
            drawshift-=nextdrawshift;
            graphcount++;
            ui->widget->addGraph();
            ui->widget->graph(graphcount)->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));

            if (adaptivenumparts)
            {                
                if (attent<20)
                    hnt->numst = qrand() % 12 + 4;
                else if ((attent>20) && (attent<40))
                    hnt->numst = qrand() % 10 + 4;
                else if ((attent>40) && (attent<60))
                    hnt->numst = qrand() % 8 + 4;
                else if ((attent>60) && (attent<80))
                    hnt->numst = qrand() % 6 + 4;
                else if (attent>80)
                    hnt->numst = qrand() % 4 + 4;
            }

         //   hnt->numst = qrand() % 16;
        }

    }
}

void plotwindow::timerUpdate()
{
     // check if new fragment ready
     if (counter>=hnt->imlength)
     {        
         if (usefiltering)
             zerophasefilt(0,hnt->imlength);
           //  recurbuttfilt();
         if (!playsaved)
         {
            analysemeandata();            
            analysepart();            
            if (soundmodul)
            {
                if (attentmodul)
                    curmodval = attent;
                else
                    curmodval = meditt;
                if (curmodval<20)
                    hnt->imlength=100;
                else if ((curmodval>20) && (curmodval<40))
                    hnt->imlength=150;
                else if ((curmodval>40) && (curmodval<60))
                    hnt->imlength=250;
                else if ((curmodval>60) && (curmodval<80))
                    hnt->imlength=400;
                else if (curmodval>80)
                    hnt->imlength=600;
                ui->spinBox_5->setValue(hnt->imlength*2);
                ui->horizontalSlider->setValue(hnt->imlength*2);
            }
         }
         counter=0;
     }     
}

void plotwindow::setrandomscale()
{
    tvals[0] = 5 + qrand() % 5;
    tvals[1] = 3 + qrand() % 5;
    tvals[2] = 5 + qrand() % 5;
    tvals[3] = 5 + qrand() % 5;
    tvals[4] = 3 + qrand() % 5;
    tvals[5] = 5 + qrand() % 5;
    tvals[6] = 3 + qrand() % 5;
    tvals[7] = 3 + qrand() % 5;
    tvals[8] = 2 + qrand() % 4;
    tvals[9] = 2 + qrand() % 4;
    ui->spinBox_2->setValue(tvals[0]);
    ui->spinBox_3->setValue(tvals[1]);
    ui->spinBox_4->setValue(tvals[2]);
    ui->spinBox_9->setValue(tvals[3]);
    ui->spinBox_10->setValue(tvals[4]);
    ui->spinBox_11->setValue(tvals[5]);
    ui->spinBox_12->setValue(tvals[6]);
    ui->spinBox_13->setValue(tvals[7]);
    ui->spinBox_14->setValue(tvals[8]);
    ui->spinBox_15->setValue(tvals[9]);
}

void plotwindow::savescaletofile(QString fname)
{
    QFile outputFile(fname);
    outputFile.open(QIODevice::WriteOnly);
    QTextStream fout(&outputFile);
    for (int i=0; i<10; i++)
        fout << tvals[i] << " ";
    outputFile.close();
}

void plotwindow::loadscalefromfile(QString fname)
{
    QFile inputFile(fname);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream fin(&inputFile);
    QStringList sl;
    QString line = fin.readLine();
    if (line.length()>0)
    {
        sl = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        for (int i=0; i<10; i++)
            tvals[i] = sl[i].toInt();
    }
    inputFile.close();
    ui->spinBox_2->setValue(tvals[0]);
    ui->spinBox_3->setValue(tvals[1]);
    ui->spinBox_4->setValue(tvals[2]);
    ui->spinBox_9->setValue(tvals[3]);
    ui->spinBox_10->setValue(tvals[4]);
    ui->spinBox_11->setValue(tvals[5]);
    ui->spinBox_12->setValue(tvals[6]);
    ui->spinBox_13->setValue(tvals[7]);
    ui->spinBox_14->setValue(tvals[8]);
    ui->spinBox_15->setValue(tvals[9]);
}

void plotwindow::refresh()
{
    if (mindwstart)
        ui->spinBox_5->setValue(hnt->imlength*1.953125);
    else if (simeeg)
        ui->spinBox_5->setValue(hnt->imlength*2);
    ui->spinBox_7->setValue(hnt->numst);
}

void plotwindow::slSettings()
{          
   sw->pwd=this;
   sw->setFixedSize(435,453);
   sw->init();
   sw->show();
}

int plotwindow::estimateoptlength(int n, int l1, int l2, int pos)
{
    return 0;
}

double plotwindow::estimateoptprop(int n, double p1, double p2, int pos)
{

    return 0;
}

void plotwindow::setaddmode(bool f)
{
    addmoderec=f;
}

int plotwindow::maxabsstim(int pos)
{
    int maxel=0;
    for (int j=pos; j<pos+hnt->stlength; j++)
        if (abs(arrc.of1[j])>maxel)
            maxel=abs(arrc.of1[j])+1;
    return maxel;
}

void plotwindow::getrawdata(int chn, double val)
{
    if ((chn==chnums-1) && (indexes[chn]==hnt->imlength))
    {
      /*  cout<<endl;
        for (int j=0; j<chnums; j++)
        {
            for (int k=0; k<10; k++)
                cout<<rawdata[j][k]<<" ";
            cout<<endl;
        }
        cout<<endl;
        QApplication::quit(); */
        for (int i=0; i<chnums; i++)
            indexes[i]=0;
    }
    rawdata[chn][indexes[chn]]=val;
    indexes[chn]++;
}

// ==== butterfiltcoeffs implementation ==== //

double* plotwindow::ComputeLP( int FilterOrder )
{
    double *NumCoeffs;
    int m;
    int i;

    NumCoeffs = (double *)calloc( FilterOrder+1, sizeof(double) );
    if( NumCoeffs == NULL ) return( NULL );

    NumCoeffs[0] = 1;
    NumCoeffs[1] = FilterOrder;
    m = FilterOrder/2;
    for( i=2; i <= m; ++i)
    {
        NumCoeffs[i] =(double) (FilterOrder-i+1)*NumCoeffs[i-1]/i;
        NumCoeffs[FilterOrder-i]= NumCoeffs[i];
    }
    NumCoeffs[FilterOrder-1] = FilterOrder;
    NumCoeffs[FilterOrder] = 1;

    return NumCoeffs;
}

double* plotwindow::ComputeHP( int FilterOrder )
{
    double *NumCoeffs;
    int i;

    NumCoeffs = ComputeLP(FilterOrder);
    if(NumCoeffs == NULL ) return( NULL );

    for( i = 0; i <= FilterOrder; ++i)
        if( i % 2 ) NumCoeffs[i] = -NumCoeffs[i];

    return NumCoeffs;
}

double* plotwindow::TrinomialMultiply( int FilterOrder, double *b, double *c )
{
    int i, j;
    double *RetVal;

    RetVal = (double *)calloc( 4 * FilterOrder, sizeof(double) );
    if( RetVal == NULL ) return( NULL );

    RetVal[2] = c[0];
    RetVal[3] = c[1];
    RetVal[0] = b[0];
    RetVal[1] = b[1];

    for( i = 1; i < FilterOrder; ++i )
    {
        RetVal[2*(2*i+1)]   += c[2*i] * RetVal[2*(2*i-1)]   - c[2*i+1] * RetVal[2*(2*i-1)+1];
        RetVal[2*(2*i+1)+1] += c[2*i] * RetVal[2*(2*i-1)+1] + c[2*i+1] * RetVal[2*(2*i-1)];

        for( j = 2*i; j > 1; --j )
        {
            RetVal[2*j]   += b[2*i] * RetVal[2*(j-1)]   - b[2*i+1] * RetVal[2*(j-1)+1] +
                c[2*i] * RetVal[2*(j-2)]   - c[2*i+1] * RetVal[2*(j-2)+1];
            RetVal[2*j+1] += b[2*i] * RetVal[2*(j-1)+1] + b[2*i+1] * RetVal[2*(j-1)] +
                c[2*i] * RetVal[2*(j-2)+1] + c[2*i+1] * RetVal[2*(j-2)];
        }

        RetVal[2] += b[2*i] * RetVal[0] - b[2*i+1] * RetVal[1] + c[2*i];
        RetVal[3] += b[2*i] * RetVal[1] + b[2*i+1] * RetVal[0] + c[2*i+1];
        RetVal[0] += b[2*i];
        RetVal[1] += b[2*i+1];
    }

    return RetVal;
}

double* plotwindow::ComputeNumCoeffs(int FilterOrder)
{
    double *TCoeffs;
    double *NumCoeffs;
    int i;

    NumCoeffs = (double *)calloc( 2*FilterOrder+1, sizeof(double) );
    if( NumCoeffs == NULL ) return( NULL );

    TCoeffs = ComputeHP(FilterOrder);
    if( TCoeffs == NULL ) return( NULL );

    for( i = 0; i < FilterOrder; ++i)
    {
        NumCoeffs[2*i] = TCoeffs[i];
        NumCoeffs[2*i+1] = 0.0;
    }
    NumCoeffs[2*FilterOrder] = TCoeffs[FilterOrder];

    free(TCoeffs);

    return NumCoeffs;
}

double* plotwindow::ComputeNumCoeffs(int FilterOrder, double Lcutoff, double Ucutoff, double *DenC)
{
    double *TCoeffs;
    double *NumCoeffs;
    std::complex<double> *NormalizedKernel;
    double Numbers[17]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int i;

    NumCoeffs = (double *)calloc( 2*FilterOrder+1, sizeof(double) );
    if( NumCoeffs == NULL ) return( NULL );

    TCoeffs = ComputeHP(FilterOrder);
    if( TCoeffs == NULL ) return( NULL );

    NormalizedKernel = (std::complex<double> *)calloc( 2*FilterOrder+1, sizeof(std::complex<double>) );
    if( NormalizedKernel == NULL ) return( NULL );

    for( i = 0; i < FilterOrder; ++i)
    {
        NumCoeffs[2*i] = TCoeffs[i];
        NumCoeffs[2*i+1] = 0.0;
    }
    NumCoeffs[2*FilterOrder] = TCoeffs[FilterOrder];
    double cp[2];
    double Wn;
    cp[0] = 2*2.0*tan(PI * Lcutoff / 2.0);
    cp[1] = 2*2.0*tan(PI * Ucutoff / 2.0);
    //double Bw;
   // Bw = cp[1] - cp[0];
    //center frequency
    Wn = sqrt(cp[0]*cp[1]);
    Wn = 2*atan2(Wn,4);
    const std::complex<double> result = std::complex<double>(-1,0);

    for(int k = 0; k<FilterOrder*2+1; k++)
    {
        NormalizedKernel[k] = std::exp(-sqrt(result)*Wn*Numbers[k]);
    }

    double b=0;
    double den=0;
    for(int d = 0; d<FilterOrder*2+1; d++)
    {
        b+=real(NormalizedKernel[d]*NumCoeffs[d]);
        den+=real(NormalizedKernel[d]*DenC[d]);
    }
    for(int c = 0; c<FilterOrder*2+1; c++)
    {
        NumCoeffs[c]=(NumCoeffs[c]*den)/b;
    }

    free(TCoeffs);
    return NumCoeffs;
}

double* plotwindow::ComputeDenCoeffs( int FilterOrder, double Lcutoff, double Ucutoff )
{
    int k;            // loop variables
    double theta;     // PI * (Ucutoff - Lcutoff) / 2.0
    double cp;        // cosine of phi
    double st;        // sine of theta
    double ct;        // cosine of theta
    double s2t;       // sine of 2*theta
    double c2t;       // cosine 0f 2*theta
    double *RCoeffs;     // z^-2 coefficients
    double *TCoeffs;     // z^-1 coefficients
    double *DenomCoeffs;     // dk coefficients
    double PoleAngle;      // pole angle
    double SinPoleAngle;     // sine of pole angle
    double CosPoleAngle;     // cosine of pole angle
    double a;         // workspace variables

    cp = cos(PI * (Ucutoff + Lcutoff) / 2.0);
    theta = PI * (Ucutoff - Lcutoff) / 2.0;
    st = sin(theta);
    ct = cos(theta);
    s2t = 2.0*st*ct;        // sine of 2*theta
    c2t = 2.0*ct*ct - 1.0;  // cosine of 2*theta

    RCoeffs = (double *)calloc( 2 * FilterOrder, sizeof(double) );
    TCoeffs = (double *)calloc( 2 * FilterOrder, sizeof(double) );

    for( k = 0; k < FilterOrder; ++k )
    {
        PoleAngle = PI * (double)(2*k+1)/(double)(2*FilterOrder);
        SinPoleAngle = sin(PoleAngle);
        CosPoleAngle = cos(PoleAngle);
        a = 1.0 + s2t*SinPoleAngle;
        RCoeffs[2*k] = c2t/a;
        RCoeffs[2*k+1] = s2t*CosPoleAngle/a;
        TCoeffs[2*k] = -2.0*cp*(ct+st*SinPoleAngle)/a;
        TCoeffs[2*k+1] = -2.0*cp*st*CosPoleAngle/a;
    }

    DenomCoeffs = TrinomialMultiply(FilterOrder, TCoeffs, RCoeffs );
    free(TCoeffs);
    free(RCoeffs);

    DenomCoeffs[1] = DenomCoeffs[0];
    DenomCoeffs[0] = 1.0;
    for( k = 3; k <= 2*FilterOrder; ++k )
        DenomCoeffs[k] = DenomCoeffs[2*k-2];

    return DenomCoeffs;
}

void plotwindow::filter(int ord, double *a, double *b, int np, double *x, double *y)
{
    int i,j;
    y[0]=b[0] * x[0];
    for (i=1;i<ord+1;i++)
    {
        y[i]=0.0;
        for (j=0;j<i+1;j++)
            y[i]=y[i]+b[j]*x[i-j];
        for (j=0;j<i;j++)
            y[i]=y[i]-a[j+1]*y[i-j-1];
    }
    for (i=ord+1;i<np+1;i++)
    {
        y[i]=0.0;
        for (j=0;j<ord+1;j++)
            y[i]=y[i]+b[j]*x[i-j];
        for (j=0;j<ord;j++)
            y[i]=y[i]-a[j+1]*y[i-j-1];
    }
}

void plotwindow::butterfiltcoefs(int lcut, int hcut, int order, int sampr)
{
    // https://stackoverflow.com/questions/10373184/bandpass-butterworth-filter-implementation-in-c
    //Frequency bands is a vector of values - Lower Frequency Band and Higher Frequency Band
    //First value is lower cutoff and second value is higher cutoff
    // 10 Hz = 10/(500/2), 50 Hz = 50/(500/2)

    double lc=(double)lcut/(sampr/2);
    double hc=(double)hcut/(sampr/2);
    double FrequencyBands[2] = {lc,hc};   // these values are as a ratio of f/fs, where fs is sampling rate, and f is cutoff frequency
                                             // and therefore should lie in the range [0 1]

    DenC = ComputeDenCoeffs(order, FrequencyBands[0], FrequencyBands[1]); // is A in matlab function
  /*  cout<<"DenC is: ";
    for (int k = 0; k<butterord*2+1; k++)
        cout<<DenC[k]<<" ";    
    cout<<endl; */

    NumC = ComputeNumCoeffs(order,lc,hc,DenC); // is B in matlab
  /*  cout<<"NumC is: ";
    for (int k = 0; k<butterord*2+1; k++)
        cout<<NumC[k]<<" ";    
    cout<<endl; */

    double y[order];
    double x[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    filter(order, DenC, NumC, order, x, y);
}

// = end of butterfiltcoeffs implementation = //

// ==== filtfilt implementation ==== //
// https://stackoverflow.com/questions/17675053/matlabs-filtfilt-algorithm

void plotwindow::add_index_range(vectori &indices, int beg, int end)
{
    int inc=1;
    for (int i = beg; i <= end; i += inc)
       indices.push_back(i);
}

void plotwindow::add_index_const(vectori &indices, int value, size_t numel)
{
    while (numel--)
        indices.push_back(value);
}

void plotwindow::append_vector(vectord &vec, const vectord &tail)
{
    vec.insert(vec.end(), tail.begin(), tail.end());
}

vectord plotwindow::subvector_reverse(const vectord &vec, int idx_end, int idx_start)
{
    vectord result(&vec[idx_start], &vec[idx_end+1]);
    std::reverse(result.begin(), result.end());
    return result;
}

inline int max_val(const vectori& vec)
{
    return std::max_element(vec.begin(), vec.end())[0];
}

void plotwindow::filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi)
{
    if (A.empty())
        throw std::domain_error("The feedback filter coefficients are empty.");
    if (std::all_of(A.begin(), A.end(), [](double coef){ return coef == 0; }))
        throw std::domain_error("At least one of the feedback filter coefficients has to be non-zero.");
    if (A[0] == 0)
        throw std::domain_error("First feedback coefficient has to be non-zero.");

    // Normalize feedback coefficients if a[0] != 1;
    auto a0 = A[0];
    if (a0 != 1.0)
    {
        std::transform(A.begin(), A.end(), A.begin(), [a0](double v) { return v / a0; });
        std::transform(B.begin(), B.end(), B.begin(), [a0](double v) { return v / a0; });
    }

    size_t input_size = X.size();
    size_t filter_order = std::max(A.size(), B.size());
    B.resize(filter_order, 0);
    A.resize(filter_order, 0);
    Zi.resize(filter_order, 0);
    Y.resize(input_size);

    const double *x = &X[0];
    const double *b = &B[0];
    const double *a = &A[0];
    double *z = &Zi[0];
    double *y = &Y[0];

    for (size_t i = 0; i < input_size; ++i)
    {
        size_t order = filter_order - 1;
        while (order)
        {
            if (i >= order)
                z[order - 1] = b[order] * x[i - order] - a[order] * y[i - order] + z[order];
            --order;
        }
        y[i] = b[0] * x[i] + z[0];
    }
    Zi.resize(filter_order - 1);
}

void plotwindow::filtfilt(vectord B, vectord A, const vectord &X, vectord &Y)
{

    int len = X.size();     // length of input
    int na = A.size();
    int nb = B.size();
    int nfilt = (nb > na) ? nb : na;
    int nfact = 3 * (nfilt - 1); // length of edge transients

    if (len <= nfact)
        throw std::domain_error("Input data too short! Data must have length more than 3 times filter order.");

    // set up filter's initial conditions to remove DC offset problems at the
    // beginning and end of the sequence
    B.resize(nfilt, 0);
    A.resize(nfilt, 0);

    vectori rows, cols;
    //rows = [1:nfilt-1           2:nfilt-1             1:nfilt-2];
    add_index_range(rows, 0, nfilt - 2);
    if (nfilt > 2)
    {
        add_index_range(rows, 1, nfilt - 2);
        add_index_range(rows, 0, nfilt - 3);
    }
    //cols = [ones(1,nfilt-1)         2:nfilt-1          2:nfilt-1];
    add_index_const(cols, 0, nfilt - 1);
    if (nfilt > 2)
    {
        add_index_range(cols, 1, nfilt - 2);
        add_index_range(cols, 1, nfilt - 2);
    }
    // data = [1+a(2)         a(3:nfilt)        ones(1,nfilt-2)    -ones(1,nfilt-2)];

    auto klen = rows.size();
    vectord data;
    data.resize(klen);
    data[0] = 1 + A[1];  int j = 1;
    if (nfilt > 2)
    {
        for (int i = 2; i < nfilt; i++)
            data[j++] = A[i];
        for (int i = 0; i < nfilt - 2; i++)
            data[j++] = 1.0;
        for (int i = 0; i < nfilt - 2; i++)
            data[j++] = -1.0;
    }

    vectord leftpad = subvector_reverse(X, nfact, 1);
    double _2x0 = 2 * X[0];
    std::transform(leftpad.begin(), leftpad.end(), leftpad.begin(), [_2x0](double val) {return _2x0 - val; });

    vectord rightpad = subvector_reverse(X, len - 2, len - nfact - 1);
    double _2xl = 2 * X[len-1];
    std::transform(rightpad.begin(), rightpad.end(), rightpad.begin(), [_2xl](double val) {return _2xl - val; });

    double y0;
    vectord signal1, signal2, zi;

    signal1.reserve(leftpad.size() + X.size() + rightpad.size());
    append_vector(signal1, leftpad);
    append_vector(signal1, X);
    append_vector(signal1, rightpad);

    // Calculate initial conditions
    MatrixXd sp = MatrixXd::Zero(max_val(rows) + 1, max_val(cols) + 1);
    for (size_t k = 0; k < klen; ++k)
    {
        sp(rows[k], cols[k]) = data[k];
    }
    auto bb = VectorXd::Map(B.data(), B.size());
    auto aa = VectorXd::Map(A.data(), A.size());
    MatrixXd zzi = (sp.inverse() * (bb.segment(1, nfilt - 1) - (bb(0) * aa.segment(1, nfilt - 1))));
    zi.resize(zzi.size());

    // Do the forward and backward filtering
    y0 = signal1[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val){ return val*y0; });
    filter(B, A, signal1, signal2, zi);
    std::reverse(signal2.begin(), signal2.end());
    y0 = signal2[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val){ return val*y0; });
    filter(B, A, signal2, signal1, zi);
    Y = subvector_reverse(signal1, signal1.size() - nfact - 1, nfact);
}

// = end of filtfilt implementation = //

void plotwindow::zerophaseinit(int lcut, int hcut, int order, int sampr)
{
    butterfiltcoefs(lcut, hcut, order, sampr);
    acoeffs = vector<double>(order*2+1);
    for(int k = 0; k<order*2+1; k++)
    {
    //   cout<<DenC[k]<<" ";
        acoeffs[k]=DenC[k];
    }
    // cout<<endl;
    bcoeffs = vector<double>(order*2+1);
    for(int k = 0; k<order*2+1; k++)
    {
    //    cout<<NumC[k]<<" ";
        bcoeffs[k]=NumC[k];
    }
    // cout<<endl;
}

void plotwindow::zerophasefilt(int posstim, int length)
{       
    vectord input_signal = vector<double>(length);
    for(int k = 0; k<length; k++)
        input_signal[k]=arrc.amp0[posstim+k];
    vectord y_filter_out, y_filtfilt_out; vectord zi = {0};
    filter(bcoeffs, acoeffs, input_signal, y_filter_out, zi);
    filtfilt(bcoeffs, acoeffs, input_signal, y_filtfilt_out);
    for(int k = 0; k<length; k++)
        arrc.amp0[posstim+k]=y_filtfilt_out[k];
}

void plotwindow::getcoeffs(vectord& acc, vectord& bcc, int ord)
{
    for(int k = 0; k<ord*2+1; k++)
    {
        acc[k]=acoeffs[k];
        bcc[k]=bcoeffs[k];
    }
    for(int k = 0; k<ord*2+1; k++)
        cout<<acc[k]<<" ";
    cout<<endl;
    for(int k = 0; k<ord*2+1; k++)
        cout<<bcc[k]<<" ";
    cout<<endl;
    cout<<endl;
}


MatrixXf plotwindow::readdata(int snums, int ssize)
{
    MatrixXf inp(samplenums,samplesize);
    QFile inputFile("C:/EEGdat/Xinp.dat");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream fin(&inputFile);
    QString line;
    QStringList sl;
    for (int j=0; j<snums; j++)
    {
        line = fin.readLine();
        sl = line.split(QRegExp("\\,"), QString::SkipEmptyParts);
        for (int i=0; i < ssize; i++)
            inp(j,i)=sl[i].toDouble();
    }
    inputFile.close();
    return inp;
}

void plotwindow::playtank1(QString tonesset)
{
    // b B g G d D f# E C a
    if (tonesset.contains("b"))
    {
        play_b();
      //  delay(chorddelay);
    }
    if (tonesset.contains("B"))
    {
        play_Blow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("g"))
    {
        play_g();
     //   delay(chorddelay);
    }
    if (tonesset.contains("G"))
    {
        play_Glow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("d"))
    {
        play_d();
      //  delay(chorddelay);
    }
    if (tonesset.contains("D"))
    {
        play_Dlow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("f#"))
    {
        play_fdiez();
      //  delay(chorddelay);
    }
    if (tonesset.contains("E"))
    {
        play_Elow();
      // delay(chorddelay);
    }
    if (tonesset.contains("C"))
    {
        play_Clow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("a"))
    {
        play_a();
      //  delay(chorddelay);
    }
}

void plotwindow::playtank2(QString tonesset)
{
    //   B G  C  E  D  g f# a  d  b
    //   B D# G# F# C# b d# g# f# c#
    if (tonesset.contains("c#"))
    {
        play_b();
       // delay(chorddelay);
    }
    if (tonesset.contains("B"))
    {
        play_Blow();
       // delay(chorddelay);
    }
    if (tonesset.contains("b"))
    {
        play_g();
       // delay(chorddelay);
    }
    if (tonesset.contains("D#"))
    {
        play_Glow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("f#"))
    {
        play_d();
      //  delay(chorddelay);
    }
    if (tonesset.contains("C#"))
    {
        play_Dlow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("d"))
    {
        play_fdiez();
      //  delay(chorddelay);
    }
    if (tonesset.contains("F#"))
    {
        play_Elow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("G#"))
    {
        play_Clow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("g#"))
    {
        play_a();
      //  delay(chorddelay);
    }
}

void plotwindow::playspace(QString tonesset)
{
    //
    if (tonesset.contains("F4"))
    {
        play_Blow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("D3"))
    {
        play_Glow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("E4"))
    {
        play_d();
      //  delay(chorddelay);
    }
    if (tonesset.contains("F3"))
    {
        play_Dlow();
       // delay(chorddelay);
    }
    if (tonesset.contains("A3"))
    {
        play_fdiez();
      //  delay(chorddelay);
    }
    if (tonesset.contains("D4"))
    {
        play_Elow();
      //  delay(chorddelay);
    }
    if (tonesset.contains("C4"))
    {
        play_Clow();
     //   delay(chorddelay);
    }
    if (tonesset.contains("A4"))
    {
        play_a();
    //    delay(chorddelay);
    }
}

void plotwindow::pushleft()
{
    INPUT ip;

    Sleep(psleep);

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = 0x25; // virtual-key code
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

void plotwindow::pushright()
{
    INPUT ip;

    Sleep(psleep);

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = 0x27; // virtual-key code
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

void plotwindow::pushup()
{
    INPUT ip;

    Sleep(psleep);

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = 0x26; // virtual-key code
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

void plotwindow::pushdown()
{
    INPUT ip;

    Sleep(psleep);

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = 0x28; // virtual-key code
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

void plotwindow::pushenter()
{
    INPUT ip;

    Sleep(psleep*3);

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = 0x0D; // virtual-key code
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

/* ui processing */

void plotwindow::on_pushButton_clicked()
{

}

void plotwindow::on_checkBox_3_clicked()
{
    if (ui->checkBox_3->isChecked())
        usefiltering=true;
    else
        usefiltering=false;
}

void plotwindow::on_widget_destroyed()
{
    delete hnt;
}

void plotwindow::on_pushButton_4_clicked()
{
    slSettings();
}

void plotwindow::on_checkBox_4_clicked()
{
    if (!ui->checkBox_4->isChecked())
    {
        mindplay=false;
        ui->checkBox_5->setVisible(false);
        ui->pushButton_7->setVisible(false);
        ui->pushButton_8->setVisible(false);
        ui->pushButton_9->setVisible(false);
        ui->pushButton_10->setVisible(false);
        ui->pushButton_11->setVisible(false);
        ui->pushButton_12->setVisible(false);
        ui->pushButton_13->setVisible(false);
        ui->pushButton_14->setVisible(false);
        ui->pushButton_15->setVisible(false);
        ui->pushButton_16->setVisible(false);
    } else
    {
        if (recparts>0)
            mindplay=true;        
        ui->checkBox_5->setVisible(true);
        if (!ui->checkBox_5->isChecked())
        {
            ui->pushButton_7->setVisible(true);
            ui->pushButton_8->setVisible(true);
            ui->pushButton_9->setVisible(true);
            ui->pushButton_10->setVisible(true);
            ui->pushButton_11->setVisible(true);
            ui->pushButton_12->setVisible(true);
            ui->pushButton_13->setVisible(true);
            if ((tank1mode) || (tank2mode))
            {
                ui->pushButton_14->setVisible(true);
                ui->pushButton_16->setVisible(true);
            }
            ui->pushButton_15->setVisible(true);
        }
    }

}

void plotwindow::on_spinBox_7_valueChanged(int arg1)
{
    //if (!tim->isActive())
    hnt->numst=arg1;
}

void plotwindow::on_spinBox_5_valueChanged(int arg1)
{
    if (mindwstart)
    {
        hnt->imlength=arg1/1.953125;
       // tryplay->setInterval(arg1);
        ui->horizontalSlider->setValue(arg1);
    } else
    if (simeeg)
    {
       hnt->imlength=arg1/2;
       ui->horizontalSlider->setValue(arg1);
    }
   // qDebug()<<hnt->imlength;
}

void plotwindow::on_spinBox_6_valueChanged(int arg1)
{
    hnt->stampl=arg1;
}

void plotwindow::on_spinBox_8_valueChanged(int arg1)
{
    maxtones=arg1;
}

void plotwindow::on_pushButton_6_clicked()
{
    // DO WHAT YOU WANT
  /*  QSoundEffect* effect = new QSoundEffect();
    effect->setSource(QUrl::fromLocalFile(":/sounds/sounds/A3_.wav"));
    effect->setVolume((qreal)(30+qrand()%70)/100);
    effect->play(); */
    backimageloaded=true;
    int rimg = rand() % imglist.length();
    QString filename=folderpath+"/"+imglist.at(rimg);
  //  if (opencvstart)
  //  {
       // QPixmap pxm = ui->widget->grab();
    //    mw->setsourceimg(filename);
       // mw->setsourceimgd(pxm.toImage());
  //  }
    backimg.load(filename);
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();
//    ui->checkBox_6->setChecked(false);

  /*  ui->widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    ui->widget->showFullScreen();
    ui->widget->setWindowState(Qt::WindowFullScreen);
    ui->widget->setGeometry(QApplication::desktop()->screenGeometry()); */
}

void plotwindow::on_pushButton_16_clicked()
{
    play_g();
}

void plotwindow::on_pushButton_15_clicked()
{
    play_Blow();
}

void plotwindow::on_pushButton_14_clicked()
{
    play_b();
}

void plotwindow::on_pushButton_13_clicked()
{
    play_Dlow();
}

void plotwindow::on_pushButton_12_clicked()
{
    play_d();
}

void plotwindow::on_pushButton_11_clicked()
{
    play_Elow();
}

void plotwindow::on_pushButton_10_clicked()
{
    play_a();
}

void plotwindow::on_pushButton_9_clicked()
{
    play_Clow();
}

void plotwindow::on_pushButton_8_clicked()
{
    play_fdiez();
}

void plotwindow::on_pushButton_7_clicked()
{
    play_Glow();
}

void plotwindow::on_spinBox_7_editingFinished()
{
    ui->widget->graph(0)->setName("EEG: [0 "+QString::number((double)hnt->numst/((double)hnt->srfr/hnt->imlength),'g',2)+"] secs");
    intlen=(double)hnt->numst/((double)hnt->srfr/hnt->imlength);
  //  ui->widget->xAxis->setRange(145, hnt->imlength * 12);
  //  ui->widget->replot();
}

void plotwindow::on_spinBox_5_editingFinished()
{
    ui->widget->graph(0)->setName("EEG: [0 "+QString::number((double)hnt->numst/((double)hnt->srfr/hnt->imlength),'g',2)+"] secs");
    intlen=(double)hnt->numst/((double)hnt->srfr/hnt->imlength);
}

void plotwindow::on_checkBox_5_clicked()
{
    if (ui->checkBox_5->isChecked())
    {
        hidebutt=true;
        ui->pushButton_7->setVisible(false);
        ui->pushButton_8->setVisible(false);
        ui->pushButton_9->setVisible(false);
        ui->pushButton_10->setVisible(false);
        ui->pushButton_11->setVisible(false);
        ui->pushButton_12->setVisible(false);
        ui->pushButton_13->setVisible(false);
        ui->pushButton_14->setVisible(false);
        ui->pushButton_15->setVisible(false);
        ui->pushButton_16->setVisible(false);
    } else
    {
        hidebutt=false;
        ui->pushButton_7->setVisible(true);
        ui->pushButton_8->setVisible(true);
        ui->pushButton_9->setVisible(true);
        ui->pushButton_10->setVisible(true);
        ui->pushButton_11->setVisible(true);
        ui->pushButton_12->setVisible(true);
        ui->pushButton_13->setVisible(true);
        ui->pushButton_15->setVisible(true);
        if ((tank1mode) || (tank2mode))
        {
            ui->pushButton_14->setVisible(true);
            ui->pushButton_16->setVisible(true);
        }
    }
}

void plotwindow::on_checkBox_6_clicked()
{
    if (!backimageloaded)
    {
        if (ui->checkBox_6->isChecked())
        {
            backimg.load(":/pics/pics.jpg");
        } else
        if (tank1mode)
            backimg.load(":/pics/pics/oriongmaj.jpg");
        else
        if (tank2mode)
            backimg.load(":/pics/pics/zodiac0.jpg");
        else
            backimg.load(":/pics/pics/spacedmin.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->xAxis->grid()->setVisible(ui->checkBox_6->isChecked());
        ui->widget->yAxis->grid()->setVisible(ui->checkBox_6->isChecked());
        ui->widget->replot();
    }
}

void plotwindow::on_checkBox_7_clicked()
{
    if (!ui->checkBox_7->isChecked())
        blink=false;
    else
        blink=true;
}

void plotwindow::on_horizontalSlider_valueChanged(int value)
{
    ui->spinBox_5->setValue(value);
}

void plotwindow::on_horizontalSlider_sliderReleased()
{
    on_spinBox_5_editingFinished();
}

void plotwindow::on_spinBox_2_valueChanged(int arg1)
{
    tvals[0]=arg1;
}

void plotwindow::on_spinBox_3_valueChanged(int arg1)
{
    tvals[1]=arg1;
}

void plotwindow::on_spinBox_4_valueChanged(int arg1)
{
    tvals[2]=arg1;
}

void plotwindow::on_spinBox_9_valueChanged(int arg1)
{
    tvals[3]=arg1;
}

void plotwindow::on_spinBox_10_valueChanged(int arg1)
{
    tvals[4]=arg1;
}

void plotwindow::on_spinBox_11_valueChanged(int arg1)
{
    tvals[5]=arg1;
}

void plotwindow::on_spinBox_12_valueChanged(int arg1)
{
    tvals[6]=arg1;
}

void plotwindow::on_spinBox_13_valueChanged(int arg1)
{
    tvals[7]=arg1;
}

void plotwindow::on_spinBox_14_valueChanged(int arg1)
{
    tvals[8]=arg1;
}

void plotwindow::on_spinBox_15_valueChanged(int arg1)
{
    tvals[9]=arg1;
}

void plotwindow::on_checkBox_clicked()
{
    if (!ui->checkBox->isChecked())
    {
        tunemode=false;
        ui->spinBox_2->setDisabled(true);
        ui->spinBox_3->setDisabled(true);
        ui->spinBox_4->setDisabled(true);
        ui->spinBox_9->setDisabled(true);
        ui->spinBox_10->setDisabled(true);
        ui->spinBox_11->setDisabled(true);
        ui->spinBox_12->setDisabled(true);
        ui->spinBox_13->setDisabled(true);
        ui->spinBox_14->setDisabled(true);
        ui->spinBox_15->setDisabled(true);
    } else
    {
        tunemode=true;
        ui->spinBox_2->setDisabled(false);
        ui->spinBox_3->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
        ui->spinBox_9->setDisabled(false);
        ui->spinBox_10->setDisabled(false);
        ui->spinBox_11->setDisabled(false);
        ui->spinBox_12->setDisabled(false);
        ui->spinBox_13->setDisabled(false);
        ui->spinBox_14->setDisabled(false);
        ui->spinBox_15->setDisabled(false);
    }
}

void plotwindow::on_pushButton_17_clicked()
{
    tvals[0]=tv1; tvals[1]=tv2;
    tvals[2]=tv3; tvals[3]=tv4;
    tvals[4]=tv5; tvals[5]=tv6;
    tvals[6]=tv7; tvals[7]=tv8;
    tvals[8]=tv9; tvals[9]=tv10;
    ui->spinBox_2->setValue(tvals[0]);
    ui->spinBox_3->setValue(tvals[1]);
    ui->spinBox_4->setValue(tvals[2]);
    ui->spinBox_9->setValue(tvals[3]);
    ui->spinBox_10->setValue(tvals[4]);
    ui->spinBox_11->setValue(tvals[5]);
    ui->spinBox_12->setValue(tvals[6]);
    ui->spinBox_13->setValue(tvals[7]);
    ui->spinBox_14->setValue(tvals[8]);
    ui->spinBox_15->setValue(tvals[9]);
}

void plotwindow::on_pushButton_18_clicked()
{
    QString filename=QFileDialog::getSaveFileName(this,tr("Save File"),"D:/MINDWAVE//","Data file (*.dat);;All files (*.*)");
    if (filename!="")
        savescaletofile(filename);
}

void plotwindow::on_pushButton_19_clicked()
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),"D:/MINDWAVE/","Data file (*.dat);;All files (*.*)");
    if (filename!="")
        loadscalefromfile(filename);
}

void plotwindow::on_pushButton_20_clicked()
{
    setrandomscale();
}

void plotwindow::on_checkBox_2_clicked()
{
    if (!ui->checkBox_2->isChecked())
    {
        scaletim->stop();
        scalechange=false;
    } else
    {
        scaletim->start();
        scalechange=true;
    }

}

void plotwindow::on_spinBox_16_valueChanged(int arg1)
{
    scaletimeout=arg1;
    scaletim->setInterval(scaletimeout*1000);
}

void plotwindow::on_checkBox_8_clicked()
{
    // b B g G d D f# E C a
    // G D3, f# A3, C C4, a A4, E D4, d E4, D F3, b, B F4, g
    if (!ui->checkBox_8->isChecked())
    {
        spacemode=false;
        ui->label->setVisible(true);
        ui->label_4->setVisible(true);
        ui->label_2->setText("B");
        ui->label_3->setText("G");
        ui->label_8->setText("d");
        ui->label_5->setText("D");
        ui->label_14->setText("f#");
        ui->label_13->setText("E");
        ui->label_16->setText("C");
        ui->label_15->setText("a");
        ui->pushButton_7->setText("G");
        ui->pushButton_8->setText("f#");
        ui->pushButton_9->setText("C");
        ui->pushButton_10->setText("a");
        ui->pushButton_11->setText("E");
        ui->pushButton_12->setText("d");
        ui->pushButton_13->setText("D");
        ui->pushButton_14->setText("b");
        ui->pushButton_15->setText("B");
        ui->pushButton_16->setText("g");
        ui->spinBox_2->setVisible(true);
        ui->spinBox_4->setVisible(true);
        ui->pushButton_14->setVisible(true);
        ui->pushButton_16->setVisible(true);
    }
    else
    {
        spacemode=true;
        ui->label->setVisible(false);
        ui->label_4->setVisible(false);
        ui->label_2->setText("F4");
        ui->label_3->setText("D3");
        ui->label_8->setText("E4");
        ui->label_5->setText("F3");
        ui->label_14->setText("A3");
        ui->label_13->setText("D4");
        ui->label_16->setText("C4");
        ui->label_15->setText("A4");
        ui->pushButton_7->setText("D3");
        ui->pushButton_8->setText("A3");
        ui->pushButton_9->setText("C4");
        ui->pushButton_10->setText("A4");
        ui->pushButton_11->setText("D4");
        ui->pushButton_12->setText("E4");
        ui->pushButton_13->setText("F3");
        ui->pushButton_14->setText("");
        ui->pushButton_14->setVisible(false);
        ui->pushButton_15->setText("F4");
        ui->pushButton_16->setText("");
        ui->pushButton_16->setVisible(false);
        ui->spinBox_2->setVisible(false);
        ui->spinBox_4->setVisible(false);
    }
    if (ui->checkBox_6->isChecked())
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else
    if (!spacemode)
    {
        backimg.load(":/pics/pics/oriongmaj.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    } else
    {
        backimg.load(":/pics/pics/spacedmin.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    }
}

void plotwindow::on_pushButton_21_clicked()
{
    if (!recordstarted)
    {
        // ADD recording with filename related to current date and loading of last recording
        QAudioEncoderSettings audioSettings;

      //  QString selectedInput = audioRecorder->defaultAudioInput();
      //  QStringList inputs = audioRecorder->audioInputs();
      //  foreach (QString input, inputs)
      //     qDebug()<<audioRecorder->audioInputDescription(input);

        if (!micrecord)
            audioRecorder->setAudioInput("Line 1 (Virtual Audio Cable)");
        else
            audioRecorder->setAudioInput("Mikrofon (Realtek High Definiti");

        audioSettings.setCodec("audio/wav");
        audioSettings.setQuality(QMultimedia::HighQuality);
        audioRecorder->setEncodingSettings(audioSettings);

        audioRecorder->setOutputLocation(QUrl::fromLocalFile("rec.wav"));
        audioRecorder->record();

        recordstarted=true;

        QPalette* palette21 = new QPalette();
        palette21->setColor(QPalette::ButtonText,Qt::red);
        ui->pushButton_21->setPalette(*palette21);
        ui->pushButton_21->setText("Stop rec");
        ui->pushButton_22->setVisible(true);
    } else
    {
        audioRecorder->stop();
        recordstarted=false;
        Bells = new QSound("C:/Users/willi/Documents/rec.wav");
        QPalette* palette21 = new QPalette();
        palette21->setColor(QPalette::ButtonText,Qt::darkGreen);
        ui->pushButton_21->setPalette(*palette21);
        ui->pushButton_21->setText("Record..");
    }
}

void plotwindow::on_pushButton_22_clicked()
{
    if (!recplaying)
    {
        recplaying=true;
        ui->pushButton_22->setText("Stop");
        Bells->setLoops(loopnum);
        Bells->play();
    } else
    {
        Bells->stop();
        ui->pushButton_22->setText("Play rec");
        recplaying=false;
    }
}


void plotwindow::on_spinBox_17_valueChanged(int arg1)
{
    chorddelay=arg1;
}

void plotwindow::on_checkBox_9_clicked()
{
    if (ui->checkBox_9->isChecked())
        antirepeat=true;
    else
        antirepeat=false;
}

void plotwindow::on_spinBox_18_valueChanged(int arg1)
{
    memorylength=arg1;
}

void plotwindow::on_spinBox_19_valueChanged(int arg1)
{
    maxtonerepeats=arg1;
}

void plotwindow::on_checkBox_10_clicked()
{
    if (!ui->checkBox_10->isChecked())
    {
        mxttim->stop();
        randmxt=false;
    } else
    {
        mxttim->start();
        randmxt=true;
    }
}

void plotwindow::on_spinBox_20_valueChanged(int arg1)
{
    mxttimeout=arg1;
    mxttim->setInterval(mxttimeout*1000);
}

void plotwindow::on_spinBox_21_valueChanged(int arg1)
{
    volume = (qreal)(arg1) / 100;
    settonesvolume();
    ui->horizontalSlider_3->setValue(arg1);
}

void plotwindow::on_horizontalSlider_3_valueChanged(int value)
{
    volume = (qreal) (value) / 100;
    settonesvolume();
    ui->spinBox_21->setValue(value);
}

void plotwindow::on_radioButton_clicked()
{
    spacemode=false;  tank1mode=true; tank2mode=false;
 //   if (mw->psstart)
    pss->paintf->setsoundtype(0);
    ui->label->setVisible(true);
    ui->label_4->setVisible(true);
    ui->label->setText("b");
    ui->label_4->setText("g");
    ui->label_2->setText("B");
    ui->label_3->setText("G");
    ui->label_8->setText("d");
    ui->label_5->setText("D");
    ui->label_14->setText("f#");
    ui->label_13->setText("E");
    ui->label_16->setText("C");
    ui->label_15->setText("a");
    ui->pushButton_7->setText("G");
    ui->pushButton_8->setText("f#");
    ui->pushButton_9->setText("C");
    ui->pushButton_10->setText("a");
    ui->pushButton_11->setText("E");
    ui->pushButton_12->setText("d");
    ui->pushButton_13->setText("D");
    ui->pushButton_14->setText("b");
    ui->pushButton_15->setText("B");
    ui->pushButton_16->setText("g");
    ui->spinBox_2->setVisible(true);
    ui->spinBox_4->setVisible(true);
  //  if ((!backimageloaded) && (mindplay) && (!hidebutt))
    if ((!hidebutt) && (mindplay))
    {
        ui->pushButton_14->setVisible(true);
        ui->pushButton_16->setVisible(true);
    }
    if ((ui->checkBox_6->isChecked()) && (!backimageloaded))
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else if (!backimageloaded)
    {
        backimg.load(":/pics/pics/oriongmaj.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    }
}

void plotwindow::on_radioButton_3_clicked()
{
    spacemode=true; tank1mode=false; tank2mode=false;
   // if (mw->psstart)
    pss->paintf->setsoundtype(2);
    ui->label->setVisible(false);
    ui->label_4->setVisible(false);
    ui->label_2->setText("F4");
    ui->label_3->setText("D3");
    ui->label_8->setText("E4");
    ui->label_5->setText("F3");
    ui->label_14->setText("A3");
    ui->label_13->setText("D4");
    ui->label_16->setText("C4");
    ui->label_15->setText("A4");
    ui->pushButton_7->setText("D3");
    ui->pushButton_8->setText("A3");
    ui->pushButton_9->setText("C4");
    ui->pushButton_10->setText("A4");
    ui->pushButton_11->setText("D4");
    ui->pushButton_12->setText("E4");
    ui->pushButton_13->setText("F3");
    ui->pushButton_14->setText("");
    ui->pushButton_14->setVisible(false);
    ui->pushButton_15->setText("F4");
    ui->pushButton_16->setText("");
    ui->pushButton_16->setVisible(false);
    ui->spinBox_2->setVisible(false);
    ui->spinBox_4->setVisible(false);
    if ((ui->checkBox_6->isChecked()) && (!backimageloaded))
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else if (!backimageloaded)
    {
        backimg.load(":/pics/pics/spacedmin.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    }
}

void plotwindow::on_radioButton_2_clicked()
{
    //   B G  C  E  D  g f# a  d  b
    //   B D# G# F# C# b d# g# f# c#
    spacemode=false; tank1mode=false; tank2mode=true;
    if (addmodeon)
        pss->paintf->setsoundtype(1);
    ui->label->setVisible(true);
    ui->label_4->setVisible(true);
    ui->label->setText("c#");
    ui->label_4->setText("b");
    ui->label_2->setText("B");
    ui->label_3->setText("D#");
    ui->label_8->setText("f#");
    ui->label_5->setText("C#");
    ui->label_14->setText("d#");
    ui->label_13->setText("F#");
    ui->label_16->setText("G#");
    ui->label_15->setText("g#");
    ui->pushButton_7->setText("D#");
    ui->pushButton_8->setText("d#");
    ui->pushButton_9->setText("G#");
    ui->pushButton_10->setText("g#");
    ui->pushButton_11->setText("F#");
    ui->pushButton_12->setText("f#");
    ui->pushButton_13->setText("C#");
    ui->pushButton_14->setText("c#");
    ui->pushButton_15->setText("B");
    ui->pushButton_16->setText("b");
    ui->spinBox_2->setVisible(true);
    ui->spinBox_4->setVisible(true);
  //  if ((!backimageloaded) && (mindplay) && (!hidebutt))
    if ((!hidebutt) && (mindplay))
    {
        ui->pushButton_14->setVisible(true);
        ui->pushButton_16->setVisible(true);
    }
    if ((ui->checkBox_6->isChecked()) && (!backimageloaded))
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else if (!backimageloaded)
    {
        backimg.load(":/pics/pics/zodiac0.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    }
}

void plotwindow::on_pushButton_23_clicked()
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),"D://","All files (*.*)");
    if (filename!="")
    {
        backimg.load(filename);        
        backimg.scaled(1400,700,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
        ui->widget->replot();
        ui->checkBox_6->setChecked(false);
        backimageloaded=true;
      //  if (opencvstart)
      //      mw->setsourceimg(filename);
    }
}

void plotwindow::on_checkBox_11_clicked()
{
    filteringback=!filteringback;
    colorizeback=false;
    ui->checkBox_14->setEnabled(!ui->checkBox_11->isChecked());
}

void plotwindow::on_comboBox_currentIndexChanged(int index)
{
    if (index==0)
    {
        attentmodul=true;
        ui->progressBar->setPalette(sp1);
        ui->label_23->setVisible(true);
        ui->label_24->setVisible(false);
    }
    else
    {
        attentmodul=false;
        ui->progressBar->setPalette(sp2);
        ui->label_23->setVisible(false);
        ui->label_24->setVisible(true);
    }
}

void plotwindow::on_checkBox_12_clicked()
{
    soundmodul=!soundmodul;
}

void plotwindow::on_pushButton_24_clicked()
{
    if (pssstart)
    {
        pmx = ui->widget->grab();
        pss->paintf->setbackimage(pmx);
        pss->paintf->mainpic=pmx;
        pss->paintf->pmain=pmx;
        this->hide();
        pss->paintf->show();
        pss->paintf->setFocus();
    }
}

void plotwindow::on_spinBox_22_valueChanged(int arg1)
{
    picchangeborder=arg1;
}

void plotwindow::on_checkBox_13_clicked()
{
    fixback=!fixback;
}

void plotwindow::on_pushButton_25_clicked()
{
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->replot();
}

void plotwindow::on_checkBox_14_clicked()
{
    filteringback=!filteringback;
    colorizeback=!colorizeback;
    if (opencvstart)
        ui->checkBox_11->setEnabled(!ui->checkBox_14->isChecked());
}
