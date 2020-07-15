/* source file for MindPlay window class -
   responsible for raw EEG signal processing,
   brain waves flow drawing, image filters application and sound translations */

#include "plotwindow.h"
#include "mainwindow.h"
#include "ui_plotwindow.h"
#include <iostream>
#include <QShortcut>
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
#include "QSoundEffect"
#include <bits/stdc++.h>
#include <windows.h>
#include "QThread"
#include <numeric>


const complex<double> I(0.0,1.0);
typedef std::complex<double> Complex;

plotwindow::plotwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plot)
{          
    brainflow_on = false; musicmode_on = false; // flags for signal processing and sound translation
    flowblinking = true; usefiltering = true;  // change of brain flow color, filtering of raw signal
    tank1mode = false; tank2mode = true; spacemode = false;  // different sets of sound samples
    hidebutt = false; antirepeat = true; // hiding tones buttons, flag for tones antirepeat

    numst = 8; // number of intervals in one EEG flow line
    points_for_mean = 60; // number of intervals for estimation of mean brain waves expressions
    imlength = 256; srfr = 512; // length of single EEG interval, sampling rate
    maxtonerepeats = 0; memorylength = 2; // max number of tones repeat in last [memorylength] tones
    drawshift = -150; graphcount = 0; // shift of brain waves flow lines, number of brain waves lines
    counter = 0; // counter of acquired data points in current interval
    recparts = 0; // number of recorded intervals in current brain waves flow line
    chnums = 3, sampleblock = 4; // (BCI2000): number of channels, samples in block
    chorddelay = 0; // delay in chord, only when max tones at one moment > 1
    volume = 1; // volume of sound samples
    maxshown_eeglines = 8; // number of max displaying eeg lines
    nextdrawshift = 200; // shift of y-axis when graphcount > eegintervals-2 (visual adjustment)
    simsrfr = 500; // frequency of simulated EEG flow
    picchangeborder = 70; // border for changing back image by attention>border

    simeeg = false; // simulated EEG flow
    tunemode = true; // allow to change parameters of deviations from mean brain waves expressions
    paintfstart = false; // flag for MindDraw window start
    rawsignalabove = true; // flag for position of raw signal plot
    backimageloaded = false; // background image loaded
    colorizeback = false; // colorization effect of background image
    blurback = false; // blurring background imag
    canbackchange = true;
    // flag to prevent constant change of background image when attention > border:
    // change only when attention becomes > border after it was less
    attention_modulation = true; // attention / meditation modulation
    attention_interval = false; // attention / meditation modulated length of acquired EEG intervals
    attention_volume = true; // attention modulated volume of tones
    minvolumebord = 25; // min value of volume if attention modulated
    opencvstart = false; // MindOCV window start flag
    mindwstart = false;  // flag for EEG device connection
    paintfstart = false; // flag for MindDraw window start
    adaptivenumparts = true; // attention / meditation modulated number of intervals in flow line
    filteringback = false; // filter background image
    fixback = true; // fix background image independently of attention values
    camerainp = false; // camera input for background
    updatewavesplot = true; // plotting new EEG intervals

    strLstM2 = new QStringListModel();      // list of determined tones
    strLstM2->setStringList(strLst2);

    // indexes = new int [32];     // matrix of indexes for multi-channel data acquisition
    // for (int i=0; i<32; i++)
    //     indexes[i]=0;

    for (int i=0; i<20; i++)    // initialization of vectors for brain waves flow storage
    {
        eegdata[i] = QVector<double>(NMAX); // max length of single EEG line
        for (int j=0; j<NMAX; j++)
            eegdata[i][j]=0;
    }       

    lcutoff = 4; hcutoff = 70; butterord = 2; srfr = 512; // parameters for filter
    filtercl = new filters(butterord,lcutoff,hcutoff,srfr); // filter constructor

    arrc.xc = QVector<double>(NMAX);    // vectors for current EEG data
    arrc.amp0 = QVector<double>(NMAX);    
    for (int j=0; j<NMAX; j++)
    {
        arrc.amp0[j] = 0;
        arrc.xc[j] = j;
    }

    ui->setupUi(this);
    ui->widget->installEventFilter(this);

    scaletimeout = 5;
    scaletim = new QTimer(this); // timer for changing tones scale
    scaletim->connect(scaletim, SIGNAL(timeout()), this, SLOT(scaletimerUpdate()));
    scaletim->setInterval(scaletimeout*1000);

    randmxt = false; // flag for random max tones mode
    maxtones = 1; mxttimeout = 5; // max tones in one moment, interval for timer
    mxttim = new QTimer(this);  // timer for changing max tones number
    mxttim->connect(mxttim, SIGNAL(timeout()), this, SLOT(mxttimerUpdate()));
    mxttim->setInterval(mxttimeout*1000);

    camera_interval = 50;
    camerainput = new QTimer(this); // timer for camera input
    camerainput->connect(camerainput,SIGNAL(timeout()), this, SLOT(camerainput_Update()));
    camerainput->setInterval(camera_interval);

    // emulation of some controls on neMehanika interactive animations: www.nemehanika.ru
    neuro_neMehanika_camera = new QTimer(this); // timer for neMehanika animation camera control
    neuro_neMehanika_camera->connect(neuro_neMehanika_camera,SIGNAL(timeout()), this, SLOT(neuro_neMeh_camera_update()));
    neuro_neMehanika_camera->setInterval(1000);
   // neuro_neMehanika_camera->start();
    keys_emulated = false; // flag for keybord press emulation for neMehanika controls (KEY_Q/KEY_E)
    neuro_neMehanika_colors = new QTimer(this); // timer for neMehanika animation colors change
    neuro_neMehanika_colors->connect(neuro_neMehanika_colors,SIGNAL(timeout()), this, SLOT(neuro_neMeh_colors_update()));
    neuro_neMehanika_colors->setInterval(200);
   // neuro_neMehanika_colors->start();

    tonescheck = 20; // interval for determining tones from brain waves expression
    init_timersinthread(); // init timers in separate threads for tones determining    

    // current brain waves expressions (relative power from FFT)
    delta=0; theta=0; alpha=0; beta=0; gamma=0; sgamma=0;
    // total sum of brain waves expressions
    sdelta=0; stheta=0; salpha=0; sbeta=0; sgamma=0; shgamma=0;
    // mean brain waves expressions
    meandelta=0; meantheta=0; meanalpha=0; meanbeta=0; meangamma=0; meanhgamma=0;

    // array of mean brain waves expression deviation parameters - define how often each tone plays
    tvals = new int[10];
    tv1=4; tv2=4;  tv3=5; tv4=5;  tv5=5; tv6=5;  tv7=6; tv8=6;  tv9=6; tv10=6; // default "scale"
    // delta        theta            alpha           beta            gamma
    tvals[0]=tv1; tvals[1]=tv2; tvals[2]=tv3; tvals[3]=tv4; tvals[4]=tv5;
    tvals[5]=tv6; tvals[6]=tv7; tvals[7]=tv8; tvals[8]=tv9; tvals[9]=tv10;

    // pointers on QPixmap and QPainter for filtering, allocated and deleted each filtering time
    pmvr = new QPixmap();
    sceneforfilt.addItem(&itemforfilt);
    resforfilt=QImage(QSize(1500, 800), QImage::Format_ARGB32);
    ptr = new QPainter(&resforfilt);

    splayer.init(); // initialization of sound player: tones and play slots in separate threads           
}

plotwindow::~plotwindow()
{
    delete ui;
}

void plotwindow::doplot() // configure ui elements
{
    ui->widget->setGeometry(30,50,1500,750);

    ui->label_11->setGeometry(127,874,50,25);
    ui->progressBar_2->setGeometry(177,880,120,18);
    ui->label_26->setGeometry(317,874,50,25);
    ui->progressBar_3->setGeometry(367,880,120,18);
    ui->label_27->setGeometry(507,874,50,25);
    ui->progressBar_4->setGeometry(557,880,120,18);
    ui->label_28->setGeometry(697,874,50,25);
    ui->progressBar_5->setGeometry(747,880,120,18);
    ui->label_29->setGeometry(877,874,60,25);
    ui->progressBar_6->setGeometry(937,880,120,18);
    ui->label_32->setGeometry(117,904,50,25);
    ui->progressBar_10->setGeometry(177,910,120,18);
    ui->label_33->setGeometry(307,904,55,25);
    ui->progressBar_11->setGeometry(367,910,120,18);
    ui->label_31->setGeometry(497,904,55,25);
    ui->progressBar_9->setGeometry(557,910,120,18);
    ui->label_34->setGeometry(687,904,50,25);
    ui->progressBar_7->setGeometry(747,910,120,18);
    ui->label_30->setGeometry(866,904,68,25);
    ui->progressBar_8->setGeometry(937,910,120,18);

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

    sp1.setColor(QPalette::Window, Qt::white);
    sp1.setColor(QPalette::WindowText, Qt::red);
    sp2.setColor(QPalette::Window, Qt::white);
    sp2.setColor(QPalette::WindowText, Qt::darkGreen);

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

    ui->label_23->setPalette(sp1);
    ui->label_23->setGeometry(140,20,181,26);
    ui->label_24->setPalette(sp2);
    ui->label_24->setGeometry(350,20,181,26);
    ui->label_24->setVisible(false);
    ui->progressBar->setGeometry(700,26,236,20);
    ui->horizontalSlider_2->setGeometry(700,26,160,20);
    ui->horizontalSlider_2->setValue(picchangeborder);
    ui->horizontalSlider_2->setVisible(false);
    ui->progressBar->setPalette(sp1);

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
    ui->spinBox_22->setGeometry(1522,920,40,20);
    ui->label_25->setGeometry(1430,915,91,35);
    ui->checkBox_13->setGeometry(1430,950,80,20);

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

    ui->listView_2->setGeometry(1055,869,135,45);
    ui->listView_2->setModel(strLstM2);
    ui->listView_2->setAutoScroll(true);

    ui->label_7->setGeometry(675,810,140,25);
    ui->label_7->setVisible(false);
    ui->checkBox_3->setGeometry(800,810,100,25);
    ui->pushButton_24->setGeometry(920,810,90,25);
    ui->pushButton_4->setGeometry(1140,810,70,25);
    ui->pushButton_2->setGeometry(1020,810,90,25);
    ui->spinBox_7->setGeometry(610,810,50,25);
    ui->spinBox_7->setEnabled(false);

    ui->checkBox_6->setChecked(false);
    ui->checkBox_4->setGeometry(1210,840,125,25);
    ui->checkBox_6->setGeometry(1210,860,125,25);
    ui->checkBox_7->setGeometry(1210,880,125,25);
    ui->checkBox_5->setGeometry(1210,900,125,25);

    ui->radioButton->setGeometry(1330,846,95,20);
    ui->pushButton_25->setGeometry(1286,810,120,25);
    ui->checkBox_15->setGeometry(1430,813,110,21);
    ui->checkBox_15->setChecked(blurback);
    ui->checkBox_14->setGeometry(1430,833,130,20);
    ui->checkBox_11->setGeometry(1430,853,145,20);
    ui->checkBox_12->setGeometry(1430,873,130,20);
    ui->comboBox->setGeometry(1430,893,90,20);
    ui->radioButton_2->setGeometry(1330,866,95,20);
    ui->radioButton_3->setGeometry(1330,886,95,20);

    ui->pushButton->setGeometry(1200,880,70,25);
    ui->pushButton_6->setGeometry(15,869,75,25);
    ui->pushButton_23->setGeometry(15,889,75,25);
    ui->pushButton_6->setVisible(true);
    ui->pushButton->setVisible(false);

    ui->spinBox_22->setValue(picchangeborder);
    ui->checkBox_13->setChecked(fixback);
    ui->checkBox_7->setChecked(flowblinking);
    ui->widget->setFocus();
    ui->spinBox_7->setValue(numst);
    ui->checkBox_3->setChecked(usefiltering);
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
        ui->horizontalSlider->setValue(imlength*1.953125);
    else if (simeeg)
        ui->horizontalSlider->setValue(imlength*2);
    ui->horizontalSlider->setTickInterval(50);
    ui->horizontalSlider->setSingleStep(50);
    ui->spinBox_5->setGeometry(425,810,50,25);
    ui->label_9->setGeometry(50,810,150,25);
    ui->label_18->setGeometry(50,835,140,25);
    ui->spinBox_8->setValue(maxtones);
    stepsPerPress = 10;
    if (mindwstart)
        ui->spinBox_5->setValue(imlength*1.953125);
    else if (simeeg)
        ui->spinBox_5->setValue(imlength*2);
    if (opencvstart)
        enablehue();

    fd.setPath(folderpath+"/");
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files);

    plot(ui->widget);
}

void plotwindow::plot(QCustomPlot *customPlot) // configure plot for brain waves flow
{
    if (start) return;
    sw = new Settings();
    customPlot->addGraph();
    customPlot->legend->setVisible(false);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    legendFont.setBold(true);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    customPlot->graph(0)->setPen(QPen(Qt::green));
    customPlot->graph(0)->setData(arrc.xc, arrc.amp0);
    graphcount=0;
    ui->checkBox_4->setChecked(true);
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->rescaleAxes();
    customPlot->xAxis->setRange(150, imlength * 22);
    customPlot->yAxis->setRange(-1600, 0);
    customPlot->xAxis->moveRange(- srfr / 2);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->axisRect()->setRangeZoom(Qt::Vertical);
    customPlot->xAxis->grid()->setVisible(false);
    customPlot->yAxis->grid()->setVisible(false);
    customPlot->repaint();
    customPlot->replot();
    start=true;
    tr1->start();
}

void plotwindow::init_timersinthread()  // initialize timers and move to separate thread
{
    tn1 = new QTimer(0);
    tn1->connect(tn1, SIGNAL(timeout()), this, SLOT(tn1Update()));
    tn1->setInterval(tonescheck);
    tn2 = new QTimer(0);
    tn2->connect(tn2, SIGNAL(timeout()), this, SLOT(tn2Update()));
    tn2->setInterval(tonescheck);
    tn3 = new QTimer(0);
    tn3->connect(tn3, SIGNAL(timeout()), this, SLOT(tn3Update()));
    tn3->setInterval(tonescheck);
    tn4 = new QTimer(0);
    tn4->connect(tn4, SIGNAL(timeout()), this, SLOT(tn4Update()));
    tn4->setInterval(tonescheck);
    tn5 = new QTimer(0);
    tn5->connect(tn5, SIGNAL(timeout()), this, SLOT(tn5Update()));
    tn5->setInterval(tonescheck);
    tn6 = new QTimer(0);
    tn6->connect(tn6, SIGNAL(timeout()), this, SLOT(tn6Update()));
    tn6->setInterval(tonescheck);
    tn7 = new QTimer(0);
    tn7->connect(tn7, SIGNAL(timeout()), this, SLOT(tn7Update()));
    tn7->setInterval(tonescheck);
    tn8 = new QTimer(0);
    tn8->connect(tn8, SIGNAL(timeout()), this, SLOT(tn8Update()));
    tn8->setInterval(tonescheck);
    tn9 = new QTimer(0);
    tn9->connect(tn9, SIGNAL(timeout()), this, SLOT(tn9Update()));
    tn9->setInterval(tonescheck);
    tn10 = new QTimer(0);
    tn10->connect(tn10, SIGNAL(timeout()), this, SLOT(tn10Update()));
    tn10->setInterval(tonescheck);

    tr1 = new QThread(this);
    tn1->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn1, SLOT(start()));    
    connect(tr1, SIGNAL(finished()), tn1, SLOT(stop()));
    tn2->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn2, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn2, SLOT(stop()));
    tn3->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn3, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn3, SLOT(stop()));
    tn4->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn4, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn4, SLOT(stop()));
    tn5->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn5, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn5, SLOT(stop()));
    tn6->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn6, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn6, SLOT(stop()));
    tn7->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn7, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn7, SLOT(stop()));
    tn8->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn8, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn8, SLOT(stop()));
    tn9->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn9, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn9, SLOT(stop()));
    tn10->moveToThread(tr1);
    connect(tr1, SIGNAL(started()), tn10, SLOT(start()));
    connect(tr1, SIGNAL(finished()), tn10, SLOT(stop()));
}

bool plotwindow::eventFilter(QObject *target, QEvent *event)
{    
    if ((target == ui->widget) && (event->type() == QEvent::MouseButtonPress))
    {
        // adjust interval length for sampling rate from EEG device or simulated EEG generator
        // scaling of ms values to 512 s.rate points, for example 500 ms - 256 points
        if (mindwstart)
            imlength=ui->spinBox_5->value()/1.953125;
        else if (simeeg)
        {
            srfr=500;
            imlength=ui->spinBox_5->value()/2;
        }

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event); 

        if (mouseEvent->button() == Qt::LeftButton)     // setfocus on plot,
        {           
            ui->widget->setFocus();
            numst=ui->spinBox_7->value();            
        }
        if (mouseEvent->button() == Qt::MiddleButton)   // turn on / off music mode
        {      
            musicmode_on=!musicmode_on;
            ui->checkBox_4->setChecked(musicmode_on);
            numst=ui->spinBox_7->value();           

        }
        // start / stop processing brain waves flow
        if ((mouseEvent->button() == Qt::RightButton) && ((appcn->ready) || (mindwstart) || (simeeg)))
        {
            numst=ui->spinBox_7->value();         
            if (brainflow_on)
                brainflow_on=false;
            else
            if (!brainflow_on)
            {
                recparts=0;               
                numst=ui->spinBox_7->value();                                                        
                brainflow_on=true;
                if (ui->checkBox_4->isChecked())
                    musicmode_on=true;
            }            
        }        
    }


    if ((target == ui->widget) && (event->type() == QEvent::MouseButtonDblClick))    
        on_pushButton_6_clicked(); // change background pic randomly

    if ((target == ui->widget) && (event->type() == QEvent::KeyPress))
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        pressedKeys += ((QKeyEvent*)event)->key();

        // adjustment of deviation border parameters for tones determination

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

        if (opencvstart)
        {
            // change border parameter for image changes by attention > border condition in MindOCV window
            if (keyEvent->key() == Qt::Key_0)
                mw->setborder(100);
            if (keyEvent->key() == Qt::Key_9)
                mw->setborder(90);
            if (keyEvent->key() == Qt::Key_8)
                mw->setborder(80);
            if (keyEvent->key() == Qt::Key_7)
                mw->setborder(70);
        }

        if (keyEvent->key() == Qt::Key_Left)    // shift plot position to the left
        { double low = ui->widget->xAxis->range().lower;
            double high = ui->widget->xAxis->range().upper;
            ui->widget->xAxis->moveRange(-((high - low) / stepsPerPress));
            ui->widget->replot();
        }

        if (keyEvent->key() == Qt::Key_Right)   // shift plot position to the right
        {
            double low = ui->widget->xAxis->range().lower;
            double high = ui->widget->xAxis->range().upper;
            ui->widget->xAxis->moveRange((high - low) / stepsPerPress);
            ui->widget->replot();
        }

        if (keyEvent->key() == Qt::Key_P)       // fix current brain waves flow plot (doesn't stop flow)
        {
            QPixmap pmx = ui->widget->grab();
            setbackimage(pmx,true);
        }

        if (keyEvent->key() == Qt::Key_O)       // stop / start plotting new EEG intervals
            updatewavesplot = !updatewavesplot;

        if ((keyEvent->key() == Qt::Key_N) && (opencvstart))  // hide/show MindOCV controls form
        {
            if (ocvf->formshown)
                ocvf->hide();
            else
                ocvf->show();
            ocvf->formshown = !ocvf->formshown;
        }

        if (keyEvent->key()==Qt::Key_Control)     // switch to horizontal zoom with mouse wheel
            ui->widget->axisRect()->setRangeZoom(Qt::Horizontal);        
        if (keyEvent->key()==Qt::Key_Alt)         // switch to vertical zoom with mouse wheel
            ui->widget->axisRect()->setRangeZoom(Qt::Vertical);

        if (keyEvent->key()==Qt::Key_K)     // start / stop camera input
            camerainp_on_off();

        // tones play by keys
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

        if (keyEvent->key()==Qt::Key_Tab) // switch to MindDraw window
        {
            this->hide();
            paintf->show();
            paintf->setFocus();
        }

        if (keyEvent->key()==Qt::Key_CapsLock) // change raw signal plot position
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

QImage Mat2QImagRGB(cv::Mat const& srct)
{
     cv::Mat temp;
     cvtColor(srct, temp, COLOR_BGR2RGB);
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy
     return dest;
}

void plotwindow::camerainp_on_off() // turn on-off camera input
{
    if (!camerainp)
    {
        camera.open(0);
        camerainp = true;
        camerainput->start();
    }
    else
    {
        camera.release();
        camerainp = false;
        camerainput->stop();
    }
}

void plotwindow::camerainput_Update() // processing camera input
{
    camera >> trp;
    QPixmap pm = QPixmap::fromImage(Mat2QImagRGB(trp));
    setbackimage(pm,false);
    if (paintfstart)
        paintf->setbackimage(ui->widget->grab());
}

void plotwindow::print_tones(QString str)  // update list of played tones
{
    strLst2.push_front(str);
    strLstM2->setStringList(strLst2);
}

void plotwindow::quitthreads()  // quit thread with timers for tones determination
{
    tr1->quit();
}

void plotwindow::update_brainexp_levels(int d, int md, int t, int mt, int a, int ma, int b, int mb, int g, int mg)
{
    // update progress bars of brain waves expression values
    ui->progressBar_2->setValue(d);
    ui->progressBar_3->setValue(t);
    ui->progressBar_4->setValue(a);
    ui->progressBar_5->setValue(b);
    ui->progressBar_6->setValue(g);
    ui->progressBar_10->setValue(md);
    ui->progressBar_11->setValue(mt);
    ui->progressBar_9->setValue(ma);
    ui->progressBar_7->setValue(mb);
    ui->progressBar_8->setValue(mg);
}

void plotwindow::updatedata(int start) // update EEG data array with new interval data
{
    int pos=0;
    if ((recparts>0) && (eegdata[graphcount][start-2]==0))
    // shift data when interval is increased during flow to avoid space/break in the current line
    // position of each new interval is determined based on interval length and number of recparts
    {        
        for (int j=start-2; j>0; j--)
            if (!eegdata[graphcount][j]==0)
            {
                pos=j+1;
                break;
            }
        start=pos;
    }

    for (int i=0; i<imlength; i++)
        eegdata[graphcount][start+i]=drawshift+arrc.amp0[i];
}

void plotwindow::update_attention(int t)
{
    // updated attention value, check condition on back image change
    attent=t;
    ui->label_23->setText("ATTENTION: "+QString::number(t)+"%");
    if ((attention_volume) && (t>minvolumebord))
        on_horizontalSlider_3_valueChanged(t);

    if (attention_modulation)
        ui->progressBar->setValue(t);

    // canbackchange - flag to prevent constant change of back image when attention > border:
    // change only when attention becomes > border after it was less
    if ((!fixback) && (attention_modulation) && (backimageloaded) && (t<picchangeborder))
        if (!canbackchange)
            canbackchange=true;

    if ((!fixback) && (attention_modulation) && (backimageloaded) && (t>picchangeborder))
        if (canbackchange)
        {
            on_pushButton_6_clicked();
            canbackchange=false;
        }
}


void plotwindow::update_meditation(int t)
{
    // updated meditation value, check condition on back image change
    meditt = t;
    ui->label_24->setText("MEDITATION: "+QString::number(t)+"%");

    if (!attention_modulation)
        ui->progressBar->setValue(t);

    if ((!fixback) && (!attention_modulation) && (backimageloaded) && (t<picchangeborder))
        if (!canbackchange)
            canbackchange=true;

    if ((!fixback) && (!attention_modulation) && (backimageloaded) && (t>picchangeborder))
        if (canbackchange)
        {
            on_pushButton_6_clicked();
            canbackchange=false;
        }
}

void plotwindow::radiobut1()    // switch on tankdrum1 tones from MindDraw window
{
    on_radioButton_clicked();
    ui->radioButton->setChecked(true);
}

void plotwindow::radiobut2()    // switch on tankdrum2 tones from MindDraw window
{
    on_radioButton_2_clicked();
    ui->radioButton_2->setChecked(true);
}

void plotwindow::radiobut3()    // switch on spacedrum tones from MindDraw window
{
    on_radioButton_3_clicked();
    ui->radioButton_3->setChecked(true);
}

void plotwindow::enable_num_intervals(bool fl) // enable manual setting of intervals in line
{
    ui->spinBox_7->setEnabled(fl);
}

void plotwindow::enablehue()    // enable grab MindOCV flow if colorize effect turned off
{
    if (!ui->checkBox_14->isChecked())
        ui->checkBox_11->setEnabled(true);
}

void plotwindow::setpicfolder(QString fp)   // set foled for pics
{
    folderpath=fp;
    fd.setPath(fp+"/");
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
}

QImage plotwindow::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent)
{
    // apply QGraphicsEffect, allocate and delete memore for QPixmap each time to avoid memory leakage
    if (src.isNull()) return QImage();
    if (!effect) return src;
    delete pmvr;
    pmvr = new QPixmap(QPixmap::fromImage(src));
    itemforfilt.setPixmap(*pmvr);
    itemforfilt.setGraphicsEffect(effect);  
    resforfilt.fill(Qt::transparent);
    sceneforfilt.render(ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ),Qt::IgnoreAspectRatio);
    return resforfilt;
}

void plotwindow::setbackimg_fromleftpanel(QString fpath)  // set back image from left panel
{
    backimg.load(fpath);  
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();
    ui->checkBox_6->setChecked(false);
    if (!backimageloaded)
        backimageloaded=true;
}

void plotwindow::setbackimage(QPixmap pm, bool saveback) // set background image
{
    if (saveback)
    {
        backimageloaded=true;
        backimg=pm;
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    }
    else
        ui->widget->setBackground(pm,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();
    ui->checkBox_6->setChecked(false);
}

void plotwindow::updateimlength(int t)  // update length of interval (invoked from MindDraw window)
{
    imlength=t/1.953125;
    ui->spinBox_5->setValue(t);
}

void plotwindow::applyfilteronback()    // apply blurring or/and colorize effects to back image
{    
    if (colorizeback)
    {
        colorizep = new QGraphicsColorizeEffect;
        // colorize->setColor(QColor(pw->alpha*5,256-pw->beta*5,256-pw->gamma*6,pw->meditt*2));
        colorizep->setColor(QColor(beta*4,theta*4,alpha*4,meditt*2));
        colorizep->setStrength((double)attent/100);
    }

    QM = backimg.toImage();

    if (blurback)
    {
        blurp = new QGraphicsBlurEffect;
        blurp->setBlurRadius((100-attent)/8);
        qbim1 = applyEffectToImage(QM, blurp, 0);
    }

    if (colorizeback)
    {
        if (blurback)
            qbim2 = applyEffectToImage(qbim1, colorizep, 0);
        else
            qbim2 = applyEffectToImage(QM, colorizep, 0);
    }

    delete pmvr;
    if ((!colorizeback) && (!blurback))
        pmvr = new QPixmap(QPixmap::fromImage(QM));
    else if (!colorizeback)
        pmvr = new QPixmap(QPixmap::fromImage(qbim1));
    else
        pmvr = new QPixmap(QPixmap::fromImage(qbim2));

    ui->widget->setBackground(*pmvr,true,Qt::IgnoreAspectRatio);

}

void plotwindow::settonesvolume() // set tones volume for soundplayer
{
   splayer.setvolume(volume);
}

// playing tones from soundplayer
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

void plotwindow::cleanbuttons() // clean played tones buttons for new interval tones
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

void plotwindow::neuro_neMeh_colors_update() // emulate Enter press / change of colors in neMehanika
{
    if ((keys_emulated) && (attent>64))
        pushenter();
}

void plotwindow::neuro_neMeh_camera_update() // detect start of keys emulation, camera moves in neMehanika
{
    if ((GetKeyState('Q') & 0x8000) && (keys_emulated==false))
        keys_emulated=true;
    if ((GetKeyState('E') & 0x8000) && (keys_emulated))
        keys_emulated=false;

    psleep = 10;

    if ((mindwstart) && (keys_emulated))
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

void plotwindow::tonenumbers_increase() // increase number of determined tones
{
    // separate function in case of change for multri-threads -
    // then also adding Mutex.lock / unlock, since this function can be called from several threads
    if (tonenumbers<maxtones)
        tonenumbers++;
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn1Update()
{
    if (tonenumbers<maxtones)
    { 
        if ((delta>meandelta+tvals[0]) && (!spacemode) && (!tones.contains("b")) && (!tones.contains("c#")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="b ";
                else if (tank2mode)
                    tones+="c# ";
            }
        }
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn2Update()
{
    if (tonenumbers<maxtones)
    {
        if ((delta<meandelta-tvals[1]) && (!tones.contains("B")) && (!tones.contains("F4")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn3Update()
{
    if (tonenumbers<maxtones)
    {
        if ((theta>meantheta+tvals[2]) && (!spacemode) && (!tones.contains("g")) && (!tones.contains("b")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="g ";
                else if (tank2mode)
                    tones+="b ";
            }
        }
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn4Update()
{
    if (tonenumbers<maxtones)
    {
        if ((theta<meantheta-tvals[3]) && (!tones.contains("G")) && (!tones.contains("D#")) && (!tones.contains("D3")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn5Update()
{
    if (tonenumbers<maxtones)
    {
        if ((alpha>meanalpha+tvals[4]) && (!tones.contains("d")) && (!tones.contains("f#")) && (!tones.contains("E4")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn6Update()
{
    if (tonenumbers<maxtones)
    {
        if ((alpha<meanalpha-tvals[5]) && (!tones.contains("D")) && (!tones.contains("C#")) && (!tones.contains("F3")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn7Update()
{
    if (tonenumbers<maxtones)
    {
        if ((beta>meanbeta+tvals[6]) && (!tones.contains("f#")) && (!tones.contains("d#")) && (!tones.contains("A3")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn8Update()
{
    if (tonenumbers<maxtones)
    {
        if ((gamma<meangamma-tvals[7]) && (!tones.contains("E")) && (!tones.contains("F#")) && (!tones.contains("D4")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn9Update()
{
    if (tonenumbers<maxtones)
    {
        if ((gamma>meangamma+tvals[8]) && (!tones.contains("C")) && (!tones.contains("G#")) && (!tones.contains("C4")))
        {
            tonenumbers_increase();
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
    }
}

// determine tones based on brain waves expression values, all timers in separate thread
// repeating condition in case of change on multi-threads, when each timer in separate thread
void plotwindow::tn10Update()
{
    if (tonenumbers<maxtones)
    {
        if ((hgamma>meangamma+tvals[9]) && (!tones.contains("a")) && (!tones.contains("g#")) && (!tones.contains("A4")))
        {
            tonenumbers_increase();
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
}

void plotwindow::playtones() // process determined tones and play
{
    if (brainflow_on)
    {
        cleanbuttons();
        int z=0;
        if ((antirepeat) && (strLst2.length()>memorylength))
        {
            for (int i=0; i<memorylength; i++)
                if (strLst2.at(i).toLocal8Bit().constData()==tones)
                    z++;
            if (z>maxtonerepeats)            
                randomtone();
        }
        print_tones(tones);
        letsplay();
        tonenumbers=0;
        tones="";
    }
}


void plotwindow::randomtone() // random tone
{        
    // tank1: b B g G d D E C f# a
    // tank2: c# C# b B f# F# g# G# d# D#
    // space: F4 F3 D3 E4 D4 C4 A3 A4
    QString st;
    int num;
    if (tank1mode)
    {
        num = qrand() % 10;
        st = tank1[num];
    }
    else if (tank2mode)
    {
        num = qrand() % 10;
        st = tank2[num];
    }
    else
    {
        num = qrand() % 8;
        st = space[num];
    }
    tones=st;
}

void fft(CArray& x) // Fast Fourier Transform, Cooley–Tukey, size of x must be power of 2
{
    const size_t N = x.size();
    if (N <= 1) return;
    // divide
    CArray even = x[std::slice(0, N / 2, 2)];
    CArray  odd = x[std::slice(1, N / 2, 2)];
    // conquer
    fft(even);
    fft(odd);
    // combine
    for (size_t k = 0; k < N / 2; ++k)
    {
        ComplexM t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

void plotwindow::determine_brainwaves_expression()
{
    // determine relative power of brain waves based on FFT

    int length = 2048; int bordfreq=70;
    for (int i=0; i<imlength; i++)
        t[i].real(arrc.amp0[i]);

    for (int i=imlength; i<length; i++)  // zero-padding
        t[i].real(0);

    for (int i=0; i<length; i++)
        t[i].imag(0);

    cdata = CArray(t,length);
    fft(cdata);
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
}

void plotwindow::analyse_interval() // main function for processing interval of EEG data
{  
    determine_brainwaves_expression();

    if ((brainflow_on) || (musicmode_on)) // update mean values of brain waves expression
    {                
        if (delta_vals.size() > points_for_mean-1)
        {
            delta_vals.pop_front();
            theta_vals.pop_front();
            alpha_vals.pop_front();
            beta_vals.pop_front();
            gamma_vals.pop_front();
        }

        delta_vals.push_back(delta);
        theta_vals.push_back(theta);
        alpha_vals.push_back(alpha);
        beta_vals.push_back(beta);
        gamma_vals.push_back(gamma);

        sdelta = accumulate(delta_vals.begin(), delta_vals.end(), 0);
        stheta = accumulate(theta_vals.begin(), theta_vals.end(), 0);
        salpha = accumulate(alpha_vals.begin(), alpha_vals.end(), 0);
        sbeta = accumulate(beta_vals.begin(), beta_vals.end(), 0);
        sgamma = accumulate(gamma_vals.begin(), gamma_vals.end(), 0);

        nums = delta_vals.size();
        meandelta=(double)sdelta/nums;
        meantheta=(double)stheta/nums;
        meanalpha=(double)salpha/nums;
        meanbeta=(double)sbeta/nums;
        meangamma=(double)sgamma/nums;
    }

    if (((filteringback) || (colorizeback) || (blurback)) && (!backimg.isNull())) // filtering back image
        applyfilteronback();

    if (paintfstart) // update brain waves expression arrays and plot in MindDraw
    {        
        paintf->updatefreqarrs(delta,theta,alpha,beta,gamma,hgamma);
       // if (opencvstart)  // in case of using estimated attention from each interval
           // mw->setoverlay(paintf->getestattval());
           // mw->setattent(paintf->getestattval());
    }

    if ((paintfstart) && (paintf->bfiltmode) && (!paintf->gamemode) && (!paintf->flowmode))
    {
        // filter main image in MindDraw
        if (!pss->drawflow)
            pss->applyfilter();
        else
            pss->applyfilteronbackimg();
    }

    if ((paintfstart) && (paintf->bfiltmode) && (paintf->gamemode))
    {
        // filter puzzles in MindDraw game mode
        paintf->filtering_allpuzzles(5);
        paintf->filteringmain_ingame(5);
    }  

    if ((paintfstart) && (paintf->grabmindplayflow)) // streaming of MindPlay image and flow to MindDraw
    {
        pmx = ui->widget->grab();
        paintf->setbackimage(pmx);
        paintf->mainpic=pmx;
        paintf->pmain=pmx;
    }

    // update progress bars of brain waves expression
    update_brainexp_levels(delta, meandelta, theta, meantheta, alpha, meanalpha, beta, meanbeta, gamma, meangamma);

    if (musicmode_on)
        playtones();

    if (flowblinking) // change of brain flow lines colors
    {
        if (graphcount<8)
        for (int i=0; i<graphcount+1; i++)
            ui->widget->graph(i)->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
        else
        for (int i=graphcount-8; i<graphcount+1; i++)
            ui->widget->graph(i)->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
        if (!updatewavesplot)
            ui->widget->replot();
    }    
}

void plotwindow::letsplay() // playing of tones
{
    if (tank1mode)
        playtank1(tones);
    else if (tank2mode)
        playtank2(tones);
    else
        playspace(tones);
}

QPixmap plotwindow::grabmindplay() // grab current flow pic (invoked for camera overlay from MindOCV)
{
    return ui->widget->grab();
}

void plotwindow::scaletimerUpdate() // timer updating scale (deviation parameters) randomly
{
    setrandomscale();
}

void plotwindow::mxttimerUpdate() // timer updating max tones number randomly
{
    maxtones = 1 + qrand()%2;
    ui->spinBox_8->setValue(maxtones);
}

void plotwindow::delay(int temp)
{
    QTime dieTime = QTime::currentTime().addMSecs(temp);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void plotwindow::plot_interval()    // plotting of each EEG interval, shifting flow lines
{
    if (brainflow_on)
    {               
        updatedata(recparts*imlength);
        ui->widget->graph(graphcount)->setData(arrc.xc, eegdata[graphcount]);
        if ((!filteringback) || (colorizeback))
            ui->widget->replot();
        recparts++;           
        if (recparts >= numst)
        {
            recparts=0;
            if (graphcount>maxshown_eeglines+2)
            {
                ui->widget->removeGraph(graphcount-maxshown_eeglines);
                for (int i=0; i<6000; i++)
                    eegdata[graphcount][i]=0;
                graphcount--;
            }
            if (graphcount>maxshown_eeglines-2)
                 ui->widget->yAxis->moveRange(-nextdrawshift);            
            drawshift-=nextdrawshift;
            graphcount++;
            ui->widget->addGraph();
            ui->widget->graph(graphcount)->setPen(QColor(qrand() % 256, qrand() % 256, qrand() % 256));

            if (adaptivenumparts) // attention modulated number of intervals in next line
            {                
                if (attent<20)
                    numst = qrand() % 12 + 4;
                else if ((attent>20) && (attent<40))
                    numst = qrand() % 10 + 4;
                else if ((attent>40) && (attent<60))
                    numst = qrand() % 8 + 4;
                else if ((attent>60) && (attent<80))
                    numst = qrand() % 6 + 4;
                else if (attent>80)
                    numst = qrand() % 4 + 4;
            }

           // numst = qrand() % 16; // completely random case
        }
    }
}

void plotwindow::getandprocess_eeg_data(double d1) // getting and processing EEG data
{
    arrc.amp0[counter]=d1;
        counter++;
    if (counter>=imlength)
        process_eeg_data();
  //  xraw=(int)d1;
}

void plotwindow::process_eeg_data() // processing EEG data
{
     // check if new fragment is ready
     if (counter>=imlength)
     {        
         if (usefiltering)
             filtercl->zerophasefilt(0,imlength,arrc.amp0);
         if (updatewavesplot)
             plot_interval();
         analyse_interval();
         if (attention_interval) // attention modulated length of interval
         {
             if (attention_modulation)
                 curmodval = attent;
             else
                 curmodval = meditt;
             if (curmodval<20)
                 imlength=100;
             else if ((curmodval>20) && (curmodval<40))
                 imlength=150;
             else if ((curmodval>40) && (curmodval<60))
                 imlength=250;
             else if ((curmodval>60) && (curmodval<80))
                 imlength=400;
             else if (curmodval>80)
                 imlength=600;
             ui->spinBox_5->setValue(imlength*2);
             ui->horizontalSlider->setValue(imlength*2);
             paintf->update_estrate(imlength*2);
         }
         counter=0;
     }
}

void plotwindow::setrandomscale()
// random scale (deviation parameters which determines how often which tone plays)
{
    tvals[0] = 3 + qrand() % 5; // > mdelta + []
    tvals[1] = 3 + qrand() % 5; // < mdelta - []
    tvals[2] = 4 + qrand() % 5; // > mtheta + []
    tvals[3] = 4 + qrand() % 5; // < mtheta - []
    tvals[4] = 4 + qrand() % 5; // > malpha + []
    tvals[5] = 4 + qrand() % 5; // < malpha - []
    tvals[6] = 5 + qrand() % 6; // > mbeta + []
    tvals[7] = 5 + qrand() % 6; // < mbeta - []
    tvals[8] = 5 + qrand() % 6; // > gamma + []
    tvals[9] = 5 + qrand() % 6; // < gamma + []
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

void plotwindow::update_intervals_spinboxes()
{
    if (mindwstart)
        ui->spinBox_5->setValue(imlength*1.953125);
    else if (simeeg)
        ui->spinBox_5->setValue(imlength*2);
    ui->spinBox_7->setValue(numst);
}

void plotwindow::slSettings() // Settings window
{          
   sw->pwd=this;
   sw->setFixedSize(435,385);
   sw->init();
   sw->show();
}

void plotwindow::get_multichan_rawdata(int chn, double val) // multi-channel data acquisition (BCI2000)
{
    if ((chn==chnums-1) && (indexes[chn]==imlength))
    {      
        for (int i=0; i<chnums; i++)
            indexes[i]=0;
    }
    rawdata[chn][indexes[chn]]=val;
    indexes[chn]++;
}

void plotwindow::playtank1(QString tonesset) // playing of tankdrum1 (Gmaj) tones
{
    if (tonesset.contains("b"))
    {
        play_b();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("B"))
    {
        play_Blow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("g"))
    {
        play_g();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("G"))
    {
        play_Glow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("d"))
    {
        play_d();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D"))
    {
        play_Dlow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("f#"))
    {
        play_fdiez();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("E"))
    {
        play_Elow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("C"))
    {
        play_Clow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("a"))
    {
        play_a();
        if (chorddelay>0)
            delay(chorddelay);
    }
}

void plotwindow::playtank2(QString tonesset) // playing of tankdrum2 (Bmaj) tones
{
    if (tonesset.contains("c#"))
    {
        play_b();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("B"))
    {
        play_Blow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("b"))
    {
        play_g();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D#"))
    {
        play_Glow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("f#"))
    {
        play_d();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("C#"))
    {
        play_Dlow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("d"))
    {
        play_fdiez();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("F#"))
    {
        play_Elow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("G#"))
    {
        play_Clow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("g#"))
    {
        play_a();
        if (chorddelay>0)
            delay(chorddelay);
    }
}

void plotwindow::playspace(QString tonesset) // playing of space drum Dmin tones
{    
    if (tonesset.contains("F4"))
    {
        play_Blow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D3"))
    {
        play_Glow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("E4"))
    {
        play_d();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("F3"))
    {
        play_Dlow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("A3"))
    {
        play_fdiez();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D4"))
    {
        play_Elow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("C4"))
    {
        play_Clow();
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("A4"))
    {
        play_a();
        if (chorddelay>0)
            delay(chorddelay);
    }
}

void plotwindow::musicmode_on_off() // music mode on / off and change corresponded ui
{
    if (!ui->checkBox_4->isChecked())
    {
        musicmode_on=false;
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
        if (delta_vals.size()>0)
            musicmode_on=true;
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

void plotwindow::setmusicmode(bool fl) // set music mode and update corresponded checkBox (invoked from MindDraw)
{
    ui->checkBox_4->setChecked(fl);
    musicmode_on_off();
}

void plotwindow::pushleft()     // emulate left_key press
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

void plotwindow::pushright()    // emulate right_key press
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

void plotwindow::pushup()   // emulate up_key press
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

void plotwindow::pushdown()    // emulate down_key press
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

void plotwindow::pushenter() // emulate enter_key press
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

void plotwindow::on_checkBox_3_clicked()
{
    if (ui->checkBox_3->isChecked())
        usefiltering=true;
    else
        usefiltering=false;
}

void plotwindow::on_pushButton_4_clicked()
{
    slSettings();
}

void plotwindow::on_checkBox_4_clicked()    // music mode on / off
{
    musicmode_on_off();
    paintf->updatemusicmode(ui->checkBox_4->isChecked());
}

void plotwindow::on_spinBox_7_valueChanged(int arg1) // number of intervals in line
{
    numst=arg1;
}

void plotwindow::on_spinBox_5_valueChanged(int arg1) // length of interval
{
    if (mindwstart)
    {
        imlength=arg1/1.953125;        
        ui->horizontalSlider->setValue(arg1);
        paintf->update_estrate(arg1);
    } else
    if (simeeg)
    {
        imlength=arg1/2;
        ui->horizontalSlider->setValue(arg1);
    }
}

void plotwindow::on_spinBox_8_valueChanged(int arg1) // max tones number
{
    maxtones=arg1;
}

void plotwindow::on_pushButton_6_clicked()  // load random back image
{ 
    backimageloaded=true;
    int rimg = rand() % imglist.length();
    QString filename=folderpath+"/"+imglist.at(rimg);  
    backimg.load(filename);
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();
//    ui->checkBox_6->setChecked(false);  
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

void plotwindow::on_checkBox_5_clicked()    // hide buttons
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

void plotwindow::on_checkBox_6_clicked() // hide / show background image, white space with coordinates grids
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

void plotwindow::on_checkBox_7_clicked() // color change of brain waves flow lines
{
    if (!ui->checkBox_7->isChecked())
        flowblinking=false;
    else
        flowblinking=true;
}

void plotwindow::on_horizontalSlider_valueChanged(int value) // length of interval
{
    ui->spinBox_5->setValue(value);
}

// scale / deviation parameters
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

void plotwindow::on_checkBox_clicked() // adjust of scale (deviation parameters) on / off
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

void plotwindow::on_pushButton_17_clicked() // restore default scale (deviation parameters)
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

void plotwindow::on_pushButton_18_clicked() // save scale
{
    QString filename=QFileDialog::getSaveFileName(this,tr("Save File"),folderpath,"Data file (*.dat);;All files (*.*)");
    if (filename!="")
        savescaletofile(filename);
}

void plotwindow::on_pushButton_19_clicked() // load scale
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),folderpath,"Data file (*.dat);;All files (*.*)");
    if (filename!="")
        loadscalefromfile(filename);
}

void plotwindow::on_pushButton_20_clicked() // random scale
{
    setrandomscale();
}

void plotwindow::on_checkBox_2_clicked()    // change scale by timer
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

void plotwindow::on_radioButton_clicked() // tankdrum1 (Gmaj) mode
{
    spacemode=false;  tank1mode=true; tank2mode=false;
 //   if (mw->psstart)
    paintf->setsoundtype(0);
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
    if ((!hidebutt) && (musicmode_on))
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


void plotwindow::on_radioButton_2_clicked()  // tankdrum2 (Bmaj) mode
{
    //   B G  C  E  D  g f# a  d  b
    //   B D# G# F# C# b d# g# f# c#
    spacemode=false; tank1mode=false; tank2mode=true;
    if (brainflow_on)
        paintf->setsoundtype(1);
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
    if ((!hidebutt) && (musicmode_on))
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

void plotwindow::on_radioButton_3_clicked() // spacedrum Dmin mode
{
    spacemode=true; tank1mode=false; tank2mode=false;
   // if (mw->psstart)
    paintf->setsoundtype(2);
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

void plotwindow::on_pushButton_23_clicked()  // load backgroumd image from file
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),folderpath,"Images (*.png *.bmp *.jpg)");
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

void plotwindow::on_checkBox_11_clicked()  // grab MindOCV flow
{
    filteringback=!filteringback;
    colorizeback=false;
    ui->checkBox_14->setEnabled(!ui->checkBox_11->isChecked());
}

void plotwindow::on_comboBox_currentIndexChanged(int index) // attention / meditation modulation
{
    if (index==0)
    {
        attention_modulation=true;
        ui->progressBar->setPalette(sp1);
        ui->label_23->setVisible(true);
        ui->label_24->setVisible(false);
    }
    else
    {
        attention_modulation=false;
        ui->progressBar->setPalette(sp2);
        ui->label_23->setVisible(false);
        ui->label_24->setVisible(true);
    }
}

void plotwindow::on_checkBox_12_clicked()   // attention modulated length of interval
{
    attention_interval=!attention_interval;
    if (attention_interval)
    {
        paintf->adaptivespinrate(true);
        ui->spinBox_5->setStyleSheet("QSpinBox { background-color: yellow; }");
    }
    else
    {
        paintf->adaptivespinrate(false);
        ui->spinBox_5->setStyleSheet("QSpinBox { background-color: white; }");
    }
}

void plotwindow::on_pushButton_24_clicked() // set MindDraw pic from current MindPlay flow pic
{
    if (paintfstart)
    {
        pmx = ui->widget->grab();
        paintf->setbackimage(pmx);
        paintf->mainpic=pmx;
        paintf->pmain=pmx;
        this->hide();
        paintf->show();
        paintf->setFocus();
    }
}

void plotwindow::on_spinBox_22_valueChanged(int arg1)
// change border for back image change by attention/meditation
{
    picchangeborder=arg1;
}

void plotwindow::on_checkBox_13_clicked() // fix back image on / off (with attention > border)
{
    fixback=!fixback;
    ui->horizontalSlider_2->setVisible(!fixback);
}

void plotwindow::on_pushButton_25_clicked() // restore unfiltered back image
{
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->replot();
}

void plotwindow::on_checkBox_14_clicked() // colorize back image
{
    filteringback=!filteringback;
    colorizeback=!colorizeback;
    if (opencvstart)
        ui->checkBox_11->setEnabled(!ui->checkBox_14->isChecked());
}

void plotwindow::on_pushButton_2_clicked() // set MindOCV puc from current MindPlay flow pic
{
    if (opencvstart)
    {
        pmx = ui->widget->grab();        
        mw->setdstfromplay(pmx.toImage());
    }
}

void plotwindow::on_checkBox_15_clicked() // blurring of back image
{
    blurback=!blurback;
}
