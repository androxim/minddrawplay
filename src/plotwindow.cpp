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
#include "OscOutboundPacketStream.h"
#include "UdpSocket.h"

const complex<double> I(0.0,1.0);
typedef std::complex<double> Complex;

sf::SoundBuffer buf1, buf2, buf3, buf4;
sf::Sound thetasound, alphasound, betasound, gammasound;

plotwindow::plotwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plot)
{          
    brainflow_on = false; musicmode_on = false; // flags for signal processing and sound translation
    flowblinking = true; usefiltering = true;  // change of brain flow color, filtering of raw signal
    tank1mode = false; tank2mode = true; spacemode = false;  // different sets of sound samples
    hidebutt = false; antirepeat = true; // hiding tones buttons, flag for tones antirepeat

    numst = 8; // number of intervals in one EEG flow line
    points_for_mean = 30; // number of intervals for estimation of mean brain waves expressions
    imlength = 256; srfr = 512; // length of single EEG interval, sampling rate
    maxtonerepeats = 0; memorylength = 2; // max number of tones repeat in last [memorylength] tones
    drawshift = -150; graphcount = 0; // shift of brain waves flow lines, number of brain waves lines
    counter = 0; // counter of acquired data points in current interval
    recparts = 0; // number of recorded intervals in current brain waves flow line
    chnums = 3, sampleblock = 4; // (BCI2000): number of channels, samples in block (should be adapted for other than MindWave devices)
    nums_waves_values = 0; // number of waves value in array for waves expression
    chorddelay = 0; // delay in chord, only when max tones at one moment > 1
    volume = 1; // volume of sound samples
    maxshown_eeglines = 8; // number of max displaying eeg lines
    nextdrawshift = 200; // shift of y-axis when graphcount > eegintervals-2 (visual adjustment)
    simsrfr = 500; // frequency of simulated EEG flow
    picchangeborder = 70; // border for changing back image by attention>border
    buffercount = 0; // number of points in data buffer
    nemehanika_bord = 64; // border of color changes in neMehanika emulation    
    wavessound_volume = 30; // volume value for additional background 4 sounds
    total_intervals = 0; // total number of processed intervals of data
    tonesets_border1 = 30; // border1 for tones set switch by attention/meditation
    tonesets_border2 = 70; // border2 for tones set switch by attention/meditation
    prev_att = curr_att = 0;        // previous and current values of attention (from EEG device)
    prev_medit = curr_medit = 0;    // previous and current values of meditation (from EEG device)
    prev_estatt = curr_estatt = 0;  // previous and current values of attention (from FFT relative power)
    ogl_angle_change = 0.0;         // openGl rotation angle increment
    ogl_scale = 1.0;                // openGL scale parameter

    simeeg = false; // simulated EEG flow    
    rawsignalabove = true; // flag for position of raw signal plot
    backimageloaded = false; // background image loaded
    colorizeback = false; // colorization effect of background image
    blurback = false; // blurring background imag
    canbackchange = true;
    // flag to prevent constant change of background image when attention > border:
    // change only when attention becomes > border after it was less
    attention_modulation = true; // attention / meditation modulation
    est_attention_modulation = false; // modulation by estimated attention from FFT relative powers
    attention_interval = false; // attention / meditation modulated length of acquired EEG intervals
    attention_volume = true; // attention / meditation modulated volume of tones
    minvolumebord = 20; // min value of volume if attention modulated
    opencvstart = false; // MindOCV window start flag
    mindwstart = false;  // flag for EEG device connection
    paintfstart = false; // flag for MindDraw window start
    adaptivenumparts = true; // attention / meditation modulated number of intervals in flow line
    filteringback = false; // filter background image
    fixback = true; // fix background image independently of attention values
    camerainp = false; // camera input for background
    updatewavesplot = true; // plotting new EEG intervals
    oscstreaming = false;   // streaming attention, meditation and waves expression levels via OSC
    adaptivepicsborder = false; // adaptive border based on attention/meditation for background pics change
    savewavestofile = true; // saving brain waves data to file
    switchtonesset_by_att = false; // switch tones set by attention/meditation values intervals
    playthetavibe = false;      // play sound sample for theta wave expression, stringsample8
    playalphavibe = false;      // play sound sample for alpha wave expression, stringsample1
    playbetavibe = false;       // play sound sample for beta wave expression, stringsample7
    playgammavibe = false;      // play sound sample for gamma wave expression, stringsample2
    waves_sound_modul = false;  // waves background sound modulation by attention / meditation

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
   // arrc.amp0 = QVector<double>(NMAX);
    for (int j=0; j<NMAX; j++)
    {
       // arrc.amp0[j] = 0;
        arrc.xc[j] = j;
    }

    ui->setupUi(this);
    ui->widget->installEventFilter(this);

    recordwaves_rate = 200; // ms
    record_waves_tofile = new QTimer(this); // timer for saving brain waves expression and mental activity to file
    record_waves_tofile->connect(record_waves_tofile, SIGNAL(timeout()), this, SLOT(recordwaves_tofile_Update()));
    record_waves_tofile->setInterval(recordwaves_rate);    

    mactivation_timeout = 40; // ms
    mental_activations = new QTimer(this); // timer for smoothing mental activity levels
    mental_activations->connect(mental_activations, SIGNAL(timeout()), this, SLOT(mental_activations_Update()));
    mental_activations->setInterval(mactivation_timeout);

    scaletimeout = 5; // s
    scaletim = new QTimer(this); // timer for changing tones scale
    scaletim->connect(scaletim, SIGNAL(timeout()), this, SLOT(scaletimerUpdate()));
    scaletim->setInterval(scaletimeout*1000);

    randmxt = false; // flag for random max tones mode
    maxtones = 1; mxttimeout = 5; // max tones in one moment, interval for timer (s)
    mxttim = new QTimer(this);  // timer for changing max tones number
    mxttim->connect(mxttim, SIGNAL(timeout()), this, SLOT(mxttimerUpdate()));
    mxttim->setInterval(mxttimeout*1000);

    camera_interval = 50; // ms
    camerainput = new QTimer(this); // timer for camera input
    camerainput->connect(camerainput,SIGNAL(timeout()), this, SLOT(camerainput_Update()));
    camerainput->setInterval(camera_interval);

    // emulation of some controls on neMehanika interactive animations: www.nemehanika.ru
    neuro_neMehanika_camera = new QTimer(this); // timer for neMehanika animation camera control
    neuro_neMehanika_camera->connect(neuro_neMehanika_camera,SIGNAL(timeout()), this, SLOT(neuro_neMeh_camera_update()));
    neuro_neMehanika_camera->setInterval(500);
    keys_emulated = false; // flag for keybord press emulation for neMehanika controls (KEY_Q/KEY_E)
    neuro_neMehanika_colors = new QTimer(this); // timer for neMehanika animation colors change
    neuro_neMehanika_colors->connect(neuro_neMehanika_colors,SIGNAL(timeout()), this, SLOT(neuro_neMeh_colors_update()));
    neuro_neMehanika_colors->setInterval(200);

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

    // array of additional delays for each tone (pauses after tone is played)
    tonedelays = new int[10];
    for (int i=0; i<10; i++)
        tonedelays[i] = 0;

    // pointers on QPixmap and QPainter for filtering, allocated and deleted each filtering time
    pmvr = new QPixmap();
    sceneforfilt.addItem(&itemforfilt);
    resforfilt = QImage(QSize(1500, 800), QImage::Format_ARGB32);
    ptr = new QPainter(&resforfilt);

    splayer = new soundplayer();    // sound player for tones playing
    splayer->init(); // initialization of sound player: tones and play slots in separate threads  

    // initialization of brain waves background sounds
    initbacksounds();
}

plotwindow::~plotwindow()
{    
    delete ui;
}

void plotwindow::initbacksounds() // initialization of brain waves background sounds
{
    QResource thetaSound(":/sounds/sounds/strings-sample-8.wav");
    buf1.loadFromMemory(thetaSound.data(), thetaSound.size());
    thetasound.setBuffer(buf1);
    thetasound.setLoop(true);
    thetasound.setVolume(wavessound_volume);
    QResource alphaSound(":/sounds/sounds/strings-sample-1.wav");
    buf2.loadFromMemory(alphaSound.data(), alphaSound.size());
    alphasound.setBuffer(buf2);
    alphasound.setLoop(true);
    alphasound.setVolume(wavessound_volume);
    QResource betaSound(":/sounds/sounds/strings-sample-7.wav");
    buf3.loadFromMemory(betaSound.data(), betaSound.size());
    betasound.setBuffer(buf3);
    betasound.setLoop(true);
    betasound.setVolume(wavessound_volume);
    QResource gammaSound(":/sounds/sounds/strings-sample-2.wav");
    buf4.loadFromMemory(gammaSound.data(), gammaSound.size());
    gammasound.setBuffer(buf4);
    gammasound.setLoop(true);
    gammasound.setVolume(wavessound_volume);
}

void plotwindow::doplot() // configure ui elements
{
    ui->widget->setGeometry(30,50,1500,750);

    ui->label_11->setGeometry(127,869,50,25);
    ui->progressBar_2->setGeometry(177,875,120,18);
    ui->label_26->setGeometry(317,869,50,25);
    ui->progressBar_3->setGeometry(367,875,120,18);
    ui->label_27->setGeometry(507,869,50,25);
    ui->progressBar_4->setGeometry(557,875,120,18);
    ui->label_28->setGeometry(697,869,50,25);
    ui->progressBar_5->setGeometry(747,875,120,18);
    ui->label_29->setGeometry(877,869,60,25);
    ui->progressBar_6->setGeometry(937,875,120,18);

    ui->label_32->setGeometry(117,894,50,25);
    ui->progressBar_10->setGeometry(177,900,120,18);
    ui->label_33->setGeometry(307,894,55,25);
    ui->progressBar_11->setGeometry(367,900,120,18);
    ui->label_31->setGeometry(497,894,55,25);
    ui->progressBar_9->setGeometry(557,900,120,18);
    ui->label_34->setGeometry(687,894,50,25);
    ui->progressBar_7->setGeometry(747,900,120,18);
    ui->label_30->setGeometry(866,894,68,25);
    ui->progressBar_8->setGeometry(937,900,120,18);

    QPalette* palette1 = new QPalette();
    palette1->setColor(QPalette::ButtonText,Qt::blue);
    ui->pushButton_7->setPalette(*palette1);
    ui->pushButton_7->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette2 = new QPalette();
    palette2->setColor(QPalette::ButtonText,Qt::darkGreen);
    ui->pushButton_8->setPalette(*palette2);
    ui->pushButton_8->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette3 = new QPalette();
    palette3->setColor(QPalette::ButtonText,Qt::red);
    ui->pushButton_9->setPalette(*palette3);
    ui->pushButton_9->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette4 = new QPalette();
    palette4->setColor(QPalette::ButtonText,Qt::darkBlue);
    ui->pushButton_10->setPalette(*palette4);
    ui->pushButton_10->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette5 = new QPalette();
    palette5->setColor(QPalette::ButtonText,Qt::yellow);
    ui->pushButton_11->setPalette(*palette5);
    ui->pushButton_11->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QColor orangeColor(255,165,0);
    QPalette* palette6 = new QPalette();
    palette6->setColor(QPalette::ButtonText,orangeColor);
    ui->pushButton_12->setPalette(*palette6);
    ui->pushButton_12->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette7 = new QPalette();
    palette7->setColor(QPalette::ButtonText,orangeColor);
    ui->pushButton_13->setPalette(*palette7);
    ui->pushButton_13->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette8 = new QPalette();
    palette8->setColor(QPalette::ButtonText,Qt::darkMagenta);
    ui->pushButton_14->setPalette(*palette8);
    ui->pushButton_14->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette9 = new QPalette();
    palette9->setColor(QPalette::ButtonText,Qt::darkMagenta);
    ui->pushButton_15->setPalette(*palette9);
    ui->pushButton_15->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    QPalette* palette10 = new QPalette();
    palette10->setColor(QPalette::ButtonText,Qt::blue);
    ui->pushButton_16->setPalette(*palette10);
    ui->pushButton_16->setStyleSheet("background-color: rgba(255, 255, 255, 80);");

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
    ui->label_23->setVisible(false);
    ui->checkBox_13->setGeometry(950,26,81,20);
    ui->checkBox_15->setGeometry(1046,26,91,20);
    ui->checkBox_15->setChecked(blurback);
    ui->checkBox_14->setGeometry(1141,26,91,20);
    ui->checkBox_11->setGeometry(1241,26,181,20);
    ui->progressBar->setGeometry(700,26,236,20);
    ui->horizontalSlider_2->setGeometry(700,26,172,20);
    ui->horizontalSlider_2->setValue(picchangeborder);
    ui->horizontalSlider_2->setVisible(false);
    ui->progressBar->setPalette(sp1);

    ui->comboBox_2->setCurrentIndex(1);
    ui->label_35->setGeometry(161,928,95,20);
    ui->label_10->setGeometry(165,949,80,20);

    ui->label->setGeometry(250,938,25,21);
    ui->spinBox_6->setGeometry(280,928,40,21);
    ui->spinBox_2->setGeometry(280,950,40,21);

    ui->label_2->setGeometry(333,938,25,21);
    ui->spinBox_23->setGeometry(360,928,40,21);
    ui->spinBox_3->setGeometry(360,950,40,21);

    ui->label_4->setGeometry(415,938,25,21);
    ui->spinBox_24->setGeometry(440,928,40,21);
    ui->spinBox_4->setGeometry(440,950,40,21);    

    ui->label_3->setGeometry(490,938,25,21);
    ui->spinBox_25->setGeometry(520,928,40,21);
    ui->spinBox_9->setGeometry(520,950,40,21);    

    ui->label_8->setGeometry(570,938,25,21);
    ui->spinBox_26->setGeometry(600,928,40,21);
    ui->spinBox_10->setGeometry(600,950,40,21);    

    ui->label_5->setGeometry(650,938,25,21);
    ui->spinBox_27->setGeometry(680,928,40,21);
    ui->spinBox_11->setGeometry(680,950,40,21);    

    ui->label_14->setGeometry(730,938,25,21);
    ui->spinBox_28->setGeometry(760,928,40,21);
    ui->spinBox_12->setGeometry(760,950,40,21);    

    ui->label_13->setGeometry(810,938,25,21);
    ui->spinBox_29->setGeometry(840,928,40,21);
    ui->spinBox_13->setGeometry(840,950,40,21);    

    ui->label_16->setGeometry(890,938,25,21);
    ui->spinBox_30->setGeometry(920,928,40,21);
    ui->spinBox_14->setGeometry(920,950,40,21);    

    ui->label_15->setGeometry(970,938,25,21);
    ui->spinBox_31->setGeometry(1000,928,40,21);
    ui->spinBox_15->setGeometry(1000,950,40,21);    

    ui->pushButton_18->setGeometry(1050,950,71,22);
    ui->pushButton_19->setGeometry(1129,950,71,22);
    ui->pushButton_17->setGeometry(1215,950,88,22);
    ui->pushButton_20->setGeometry(1312,950,88,22);
    ui->spinBox_22->setGeometry(1522,920,40,20);
    ui->label_25->setGeometry(1430,915,91,35);   
    ui->label_25->setVisible(false);
    ui->spinBox_22->setVisible(false);
    ui->checkBox_2->setGeometry(1050,920,164,25);
    ui->label_17->setGeometry(1212,922,61,20);
    ui->spinBox_16->setGeometry(1282,922,35,20);
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
    ui->pushButton_24->setGeometry(920,810,90,25);
    ui->pushButton_2->setGeometry(1020,810,90,25);
    ui->pushButton_4->setGeometry(1030,840,76,25);
    ui->pushButton_25->setGeometry(1150,810,120,25);
    ui->pushButton_6->setGeometry(1290,810,75,25);
    ui->pushButton_23->setGeometry(1375,810,75,25);   
    ui->spinBox->setValue(wavessound_volume);
    ui->verticalSlider->setGeometry(1500,800,20,40);        
    ui->verticalSlider->setVisible(false);
    ui->spinBox_7->setEnabled(false);

    ui->checkBox_6->setChecked(false);
    ui->checkBox_4->setGeometry(1210,840,125,25);
    ui->checkBox_6->setGeometry(1210,860,125,25);
    ui->checkBox_7->setGeometry(1210,880,125,25);
    ui->checkBox_5->setGeometry(1210,900,125,25);

    ui->checkBox->setGeometry(1435,840,125,25);
    ui->spinBox->setGeometry(1460,825,40,18);
    ui->checkBox_16->setGeometry(1435,860,81,25);
    ui->checkBox_17->setGeometry(1435,880,81,25);
    ui->checkBox_18->setGeometry(1435,900,81,25);
    ui->checkBox_19->setGeometry(1435,920,91,25);

    ui->comboBox->setGeometry(580,26,110,20);
    ui->radioButton->setGeometry(1330,846,95,20);    
    ui->radioButton_2->setGeometry(1330,866,95,20);
    ui->radioButton_3->setGeometry(1330,886,95,20);
    ui->checkBox_8->setGeometry(1330,906,100,20);

    ui->pushButton->setGeometry(1200,880,70,25);
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
    ui->label_12->setGeometry(285,810,92,25);
    ui->spinBox_7->setGeometry(385,810,40,25);
    ui->spinBox_7->setStyleSheet("QSpinBox { background-color: yellow; }");
    ui->label_22->setGeometry(460,805,50,25);
    ui->horizontalSlider_3->setGeometry(460,827,50,12);
    ui->spinBox_21->setGeometry(520,810,45,20);
    ui->spinBox_21->setStyleSheet("QSpinBox { background-color: yellow; }");    
    ui->comboBox_2->setGeometry(580,810,101,20);
    ui->checkBox_9->setGeometry(285,840,130,25);
    ui->label_19->setGeometry(425,840,80,25);
    ui->spinBox_18->setGeometry(510,840,45,25);
    ui->spinBox_18->setValue(memorylength);    
    ui->label_20->setGeometry(570,840,85,25);
    ui->spinBox_19->setGeometry(666,840,45,25);
    ui->checkBox_10->setGeometry(730,840,140,25);
    ui->checkBox_3->setGeometry(730,810,140,25);
    ui->label_21->setGeometry(875,840,90,25);
    ui->spinBox_20->setGeometry(970,840,35,25);    
    ui->horizontalSlider_3->setValue(100);
    ui->spinBox_20->setValue(mxttimeout);
    ui->spinBox_19->setValue(maxtonerepeats);
    ui->checkBox_9->setChecked(antirepeat);

    ui->label_6->setGeometry(85,25,168,26);
    ui->horizontalSlider->setGeometry(100,41,100,12);
    ui->horizontalSlider->setVisible(false);
    ui->spinBox_5->setGeometry(260,28,50,20);
    ui->checkBox_12->setGeometry(330,28,190,20);

    st_stylesheet = ui->checkBox_12->styleSheet();

    if (mindwstart)
        ui->horizontalSlider->setValue(imlength*1.953125);
    else if (simeeg)
        ui->horizontalSlider->setValue(imlength*2);
    ui->horizontalSlider->setTickInterval(50);
    ui->horizontalSlider->setSingleStep(50);

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

    playthetavibe = true;
    ui->checkBox->setChecked(playthetavibe);
    thetasound.play();
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
   // customPlot->graph(0)->setPen(QPen(Qt::green));
   // customPlot->graph(0)->setData(arrc.xc, arrc.amp0);
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
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton)     // setfocus on plot
        {
            ui->widget->setFocus();
            numst=ui->spinBox_7->value();
        }
        if (mouseEvent->button() == Qt::MiddleButton)   // turn on / off music mode
        {
            musicmode_on=!musicmode_on;
            ui->checkBox_4->setChecked(musicmode_on);
            ui->checkBox_5->setChecked(!musicmode_on);
            on_checkBox_5_clicked();
            numst=ui->spinBox_7->value();
        }
        // start / stop processing brain waves flow
        if ((mouseEvent->button() == Qt::RightButton) && ((appcn->ready) || (mindwstart) || (simeeg)))
        {
            if ((nums_waves_values==0) && (savewavestofile))
                write_recfile_head();

            numst=ui->spinBox_7->value();

            if (brainflow_on)
            {
                brainflow_on=false;
                record_waves_tofile->stop();
            }
            else
            if (!brainflow_on)
            {
                recparts=0;
                numst=ui->spinBox_7->value();
                brainflow_on=true;
                record_waves_tofile->start();
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

        if ((keyEvent->key()==Qt::Key_K) && (!filteringback))     // start / stop camera input
            camerainp_on_off();

        if (keyEvent->key()==Qt::Key_H) // start emulation of neMehanika controls
        {
            neuro_neMehanika_camera->start();
            neuro_neMehanika_colors->start();
            brl->show_attentionborder();
        }

        if (keyEvent->key()==Qt::Key_M) // show main window
        {
            this->hide();
            QApplication::setActiveWindow(mw);
        }

        // tones play by keys
        if (keyEvent->key()==Qt::Key_B)
        {
            cleanbuttons();
            play_tone2();
        }
        if (keyEvent->key()==Qt::Key_C)
        {
            cleanbuttons();
            play_tone5();
        }
        if (keyEvent->key()==Qt::Key_F)
        {
            cleanbuttons();
            play_tone9();
        }
        if (keyEvent->key()==Qt::Key_E)
        {
            cleanbuttons();
            play_tone8();
        }
        if (keyEvent->key()==Qt::Key_V)
        {
            cleanbuttons();
            play_tone3();
        }
        if (keyEvent->key()==Qt::Key_D)
        {
            cleanbuttons();
            play_tone7();
        }
        if (keyEvent->key()==Qt::Key_X)
        {
            cleanbuttons();
            play_tone6();
        }
        if (keyEvent->key()==Qt::Key_G)
        {
            cleanbuttons();
            play_tone1();
        }
        if (keyEvent->key()==Qt::Key_T)
        {
            cleanbuttons();
            play_tone10();
        }
        if (keyEvent->key()==Qt::Key_Space)
        {
            cleanbuttons();
            play_tone4();
        }

        if (keyEvent->key()==Qt::Key_Tab) // switch to MindDraw window
        {
            this->hide();
            paintf->show();
            paintf->setFocus();
            QApplication::setActiveWindow(paintf);
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

void plotwindow::write_recfile_head()
{
    start_sessiom_time = QDateTime::currentDateTime().toString("ddMMyyyy-hhmmss");
    recfilename  = QCoreApplication::applicationDirPath() + "/mdp_" + start_sessiom_time + ".dat";
    recordFile.setFileName(recfilename);
    recordFile.open(QIODevice::WriteOnly);
    std::string session_type;
    if (simeeg)
        session_type = "simulated";
    else
        session_type = "recorded";
    streamrec << "Session started: " << start_sessiom_time.toStdString() << " " << session_type << "\n";
    streamrec << "est_attention, attention, meditation, delta, theta, alpha, beta, gamma, hgamma" << endl;
    recordFile.write(streamrec.str().data(), streamrec.str().length());
    recordFile.flush();
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

void plotwindow::cameraoff() // turn off camera (if it was on) when exit the app
{
    if (camerainp)
    {
        camera.release();
        camerainp = false;
        camerainput->stop();
    }
}

double euclidean_dist(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

void plotwindow::camerainput_Update() // processing camera input
{
    camera >> trp;
    flip(trp,trp,1);        

    QPixmap pm = QPixmap::fromImage(Mat2QImagRGB(trp));
    setbackimage(pm,false);
    if ((paintfstart) && (paintf->grabmindplayflow))
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
    emit splayer->stopThread();
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
        eegdata[graphcount][start+i]=drawshift+arrc.amp0[buffercount-imlength+i];
}

void plotwindow::update_openglflow() //  update openGL flow parameters
{
    if (attent>95)
        oglw->changeSpaceTexture();

    ogl_angle_change = ((double)(100-attent)/100)*5;
    if (ogl_angle_change==0)
        ogl_angle_change = 0.1;

    if ((attent<50) && (ogl_scale>0.4))
        ogl_scale -= 0.03;
    else if ((attent>50) && (ogl_scale<1.3))
        ogl_scale += 0.03;

    oglw->set_angle_scale_incs(ogl_angle_change,ogl_scale);
}

void plotwindow::mental_activations_Update()   // timer to update attention / meditation smoothly
{
    if (abs(curr_att-prev_att)>0)
    {
        if (curr_att<prev_att)
            prev_att-=1;
        else
            prev_att+=1;
        if (!est_attention_modulation)
        {
            attent = prev_att;
            if (attention_modulation)
                update_attention(prev_att);
        }
    }

    if (abs(curr_estatt-prev_estatt)>0)
    {
        if (curr_estatt<prev_estatt)
            prev_estatt-=1;
        else
            prev_estatt+=1;
        if ((est_attention_modulation) || (simeeg))
        {
            attent = prev_estatt;
            if (attention_modulation)
                update_attention(prev_estatt);
        }
    }

    if (abs(curr_medit-prev_medit)>1)
    {
        if (curr_medit<prev_medit)
            prev_medit-=1;
        else
            prev_medit+=1;
        meditt = prev_medit;
        if (!attention_modulation)
            update_meditation(prev_medit);
    }   

    if ((brl->attention_2nd) || (simeeg))    // update mental activity values on brainlevels form
        brl->updatelevels(prev_estatt,prev_medit);
    else
        brl->updatelevels(prev_att,prev_medit);  
}

void plotwindow::update_curr_attention(int t)
{
    curr_att = t;
}

// set background sounds playing by attention / meditation value changes
void plotwindow::set_backsounds_mode(int val)
{
    if (val>50)
    {
        playalphavibe = true;
        ui->checkBox_16->setChecked(true);    
    }
    else
    {
        playalphavibe = false;
        alphasound.setVolume(0);
        ui->checkBox_16->setChecked(false);
    }
    if (val>65)
    {
        playbetavibe = true;
        ui->checkBox_17->setChecked(true);    
    }
    else
    {
        playbetavibe = false;
        betasound.setVolume(0);
        ui->checkBox_17->setChecked(false);
    }
    if (val>80)
    {
        playgammavibe = true;
        ui->checkBox_18->setChecked(true);    
    }
    else
    {
        playgammavibe = false;
        gammasound.setVolume(0);
        ui->checkBox_18->setChecked(false);
    }
}

// updated attention value, check conditions on volume, back image and tones change
void plotwindow::update_attention(int t)
{
    // tones volume modulation
    if ((attention_volume) && (t>minvolumebord))
        on_horizontalSlider_3_valueChanged(t);
    ui->progressBar->setValue(t);

    // canbackchange - flag to prevent constant change of back image when attention > border:
    // change only when attention becomes > border after it was less
    if ((!fixback) && (backimageloaded) && (t<picchangeborder))
        if (!canbackchange)
            canbackchange=true;

    if (((filteringback) || (colorizeback) || (blurback)) && (!backimg.isNull()))
        applyfilteronback();  // filtering background image

    // back pic change modulation
    if (!fixback)
    {
        if (adaptivepicsborder)
        {
            int mlevel = paintf->getaverage_mentallevel(3,true);
            picchangeborder = 8 + mlevel;
            if (picchangeborder > 100)
                picchangeborder = 100;
            ui->horizontalSlider_2->setValue(picchangeborder);
        }
        if ((canbackchange) && (t>picchangeborder))
        {
            on_pushButton_6_clicked();
            canbackchange=false;
        }
    }

    // switch of tones sets modulation
    if ((!brl->attention_2nd) && (switchtonesset_by_att))
    {
        if (attent<tonesets_border1)
            on_radioButton_clicked();   // play tank1
        else if (attent<tonesets_border2)
            on_radioButton_3_clicked(); // play space
        else
            on_radioButton_2_clicked(); // play tank2
    }

    // waves background sound modulation
    if (waves_sound_modul)
        set_backsounds_mode(attent);    
    if (ui->checkBox_16->isChecked())
        alphasound.setVolume(attent-10);
    if (ui->checkBox_17->isChecked())
        betasound.setVolume(attent-15);
    if (ui->checkBox_18->isChecked())
        gammasound.setVolume(attent-20);

    // opengl flow modulation
    update_openglflow();
}

void plotwindow::update_curr_meditation(int t)
{
    curr_medit = t;
}

// updated meditation value, check conditions on volume, back image and tones change
void plotwindow::update_meditation(int t)
{    
    // tones volume modulation
    if ((attention_volume) && (t>minvolumebord))
        on_horizontalSlider_3_valueChanged(t);
    ui->progressBar->setValue(t);

    // canbackchange - flag to prevent constant change of back image when attention > border:
    // change only when attention becomes > border after it was less
    if ((!fixback) && (backimageloaded) && (t<picchangeborder))
        if (!canbackchange)
            canbackchange=true;

    // back pic change modulation
    if (!fixback)
    {
        if (adaptivepicsborder)
        {
            int mlevel = paintf->getaverage_mentallevel(3,false);
            picchangeborder = 8 + mlevel;
            if (picchangeborder > 100)
                picchangeborder = 100;
            ui->horizontalSlider_2->setValue(picchangeborder);
        }
        if ((canbackchange) && (t>picchangeborder))
        {
            on_pushButton_6_clicked();
            canbackchange=false;
        }
    }

    // switch of tones sets modulation
    if (switchtonesset_by_att)
    {
        if (meditt<tonesets_border1)
            on_radioButton_clicked();   // play tank1
        else if (meditt<tonesets_border2)
            on_radioButton_3_clicked(); // play space
        else
            on_radioButton_2_clicked(); // play tank2
    }

    // waves background sound modulation
    if (waves_sound_modul)
        set_backsounds_mode(meditt);
    if (ui->checkBox_16->isChecked())
        alphasound.setVolume(meditt-10);
    if (ui->checkBox_17->isChecked())
        betasound.setVolume(meditt-15);
    if (ui->checkBox_18->isChecked())
        gammasound.setVolume(meditt-20);
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
    if (fl)
        ui->spinBox_7->setStyleSheet("QSpinBox { background-color: white; }");
    else
        ui->spinBox_7->setStyleSheet("QSpinBox { background-color: yellow; }");
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
    splayer->setvolume(volume);
}

// playing tones from soundplayer
void plotwindow::play_tone1()
{
    ui->pushButton_7->setDown(true);
    if (spacemode)
        emit splayer->playD3();
    else if (tank1mode)    
        emit splayer->play_Fsharp();
    else
        emit splayer->playtone_Dlowsh();
}

void plotwindow::play_tone2()
{
    ui->pushButton_8->setDown(true);
    if (spacemode)
        emit splayer->playA3();
    else if (tank1mode)
        emit splayer->play_d_highsharp();
    else
        emit splayer->playtone_dsh();
}

void plotwindow::play_tone3()
{
    ui->pushButton_9->setDown(true);
    if (spacemode)
        emit splayer->playC4();
    else if (tank1mode)
        emit splayer->play_Asharp();
    else
        emit splayer->playtone_Glowsh();

}

void plotwindow::play_tone4()
{
    ui->pushButton_10->setDown(true);
    if (spacemode)
        emit splayer->playA4();
    else if (tank1mode)
        emit splayer->play_f_highsharp();
    else
        emit splayer->playtone_gsh();
}

void plotwindow::play_tone5()
{
    ui->pushButton_11->setDown(true);
    if (spacemode)
        emit splayer->playD4();
    else if (tank1mode)
        emit splayer->play_Gsharp();
    else
        emit splayer->playtone_Flowsh();
}

void plotwindow::play_tone6()
{
    ui->pushButton_12->setDown(true);
    if (spacemode)
        emit splayer->playE4();
    else if (tank1mode)
        emit splayer->play_f_high();
    else
        emit splayer->playtone_fsh();
}

void plotwindow::play_tone7()
{
    ui->pushButton_13->setDown(true);
    if (spacemode)
        emit splayer->playF3();
    else if (tank1mode)
        emit splayer->play_F();
    else
        emit splayer->playtone_Clowsh();
}

void plotwindow::play_tone8()
{    
    if (tank1mode)            
    {
        ui->pushButton_14->setDown(true);
        emit splayer->play_bhigh();
    }
    else if (tank2mode)
    {
        ui->pushButton_14->setDown(true);
        emit splayer->playtone_csh();
    }
}

void plotwindow::play_tone9()
{
    ui->pushButton_15->setDown(true);
    if (spacemode)
        emit splayer->playF4();
    else if (tank1mode)
        emit splayer->play_Dsharp();
    else    
        emit splayer->playtone_Blow();
}

void plotwindow::play_tone10()
{
    if (tank1mode)
    {
        ui->pushButton_16->setDown(true); 
        emit splayer->play_c_highsharp();
    } else
    if (tank2mode)
    {
        ui->pushButton_16->setDown(true); 
        emit splayer->playtone_b();
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
    if ((keys_emulated) && (attent>nemehanika_bord))
        pushenter();
}

void plotwindow::neuro_neMeh_camera_update() // detect start of keys emulation, camera moves in neMehanika
{
    if ((GetKeyState('Q') & 0x8000) && (keys_emulated==false))
        keys_emulated=true;
    if ((GetKeyState('E') & 0x8000) && (keys_emulated))
        keys_emulated=false;

    psleep = 20;

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
        if ((delta<meandelta-tvals[1]) && (!tones.contains("D#")) && (!tones.contains("B")) && (!tones.contains("F4")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="D# ";
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
        if ((theta>meantheta+tvals[2]) && (!spacemode) && (!tones.contains("c#")) && (!tones.contains("b")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="c# ";
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
        if ((theta<meantheta-tvals[3]) && (!tones.contains("F#")) && (!tones.contains("D#")) && (!tones.contains("D3")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="F# ";
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
        if ((alpha>meanalpha+tvals[4]) && (!tones.contains("f")) && (!tones.contains("f#")) && (!tones.contains("E4")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="f ";
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
        if ((alpha<meanalpha-tvals[5]) && (!tones.contains("F")) && (!tones.contains("C#")) && (!tones.contains("F3")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="F ";
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
        if ((beta>meanbeta+tvals[6]) && (!tones.contains("d#")) && (!tones.contains("A3")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if ((tank1mode) || (tank2mode))
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
        if ((gamma<meangamma-tvals[7]) && (!tones.contains("G#")) && (!tones.contains("F#")) && (!tones.contains("D4")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="G# ";
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
        if ((gamma>meangamma+tvals[8]) && (!tones.contains("A#")) && (!tones.contains("G#")) && (!tones.contains("C4")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="A# ";
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
        if ((hgamma>meangamma+tvals[9]) && (!tones.contains("f#")) && (!tones.contains("g#")) && (!tones.contains("A4")))
        {
            tonenumbers_increase();
            if (tonenumbers<=maxtones)
            {
                if (tank1mode)
                    tones+="f# ";
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
    // tank1: b D# c# F# f F G# A# d# f#
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
        t[i].real(arrc.amp0[buffercount-imlength+i]);

    for (int i = imlength; i<length; i++)  // zero-padding
        t[i].real(0);

    for (int i = 0; i<length; i++)
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
        if ((i>=8*4) && (i<14*4))
            alphafr+=temppow;
        if ((i>=14*4) && (i<33*4))
            betafr+=temppow;
        if ((i>=33*4) && (i<55*4))
            gammafr+=temppow;
        if ((i>=55*4))
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

void plotwindow::update_waves_meanvalues() // update mean values of brain waves expression
{
    if ((brainflow_on) || (musicmode_on))
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

        nums_waves_values = delta_vals.size();
        meandelta=(double)sdelta/nums_waves_values;
        meantheta=(double)stheta/nums_waves_values;
        meanalpha=(double)salpha/nums_waves_values;
        meanbeta=(double)sbeta/nums_waves_values;
        meangamma=(double)sgamma/nums_waves_values;
    }
}

void plotwindow::savewaves() // saving brain waves expressions and mental activity to file
{            
    streamrec.str(std::string());
    streamrec << (int)paintf->getestattval() << "," <<
                 attent << "," << meditt << "," <<
                 delta << "," << theta << "," <<
                 alpha << "," << beta << "," <<
                 gamma << "," << hgamma << "\n";
    recordFile.write(streamrec.str().data(), streamrec.str().length());
    recordFile.flush();
}

void plotwindow::recordwaves_tofile_Update() // timer for recording data to file
{
    if ((savewavestofile) && (nums_waves_values>0))
        savewaves();
}

void plotwindow::analyse_interval() // main function for processing intervals of EEG data
{  
    determine_brainwaves_expression();

    update_waves_meanvalues();   

    if (brainflow_on)
        total_intervals++;    

   // if (((filteringback) || (colorizeback) || (blurback)) && (!backimg.isNull()))
    //    applyfilteronback();  // filtering background image

    if ((brl->attention_2nd) && (attention_modulation) && (switchtonesset_by_att)) // attention modulated switch of tones sets
    {
        if (paintf->getestattval()<tonesets_border1)
            on_radioButton_clicked();   // play tank1
        else if (paintf->getestattval()<tonesets_border2)
            on_radioButton_3_clicked(); // play space
        else
            on_radioButton_2_clicked(); // play tank2
    }

    if (oscstreaming)       // streaming data through OSC
        osc_streaming(attent,meditt,delta,theta,alpha,beta,gamma,hgamma);

    if (paintfstart) // update brain waves expression arrays and plot in MindDraw
    {        
        paintf->updatefreqarrs(delta,theta,alpha,beta,gamma,hgamma);
       // if (opencvstart)  // in case of using estimated attention from each interval
           // mw->setoverlay(paintf->getestattval());
           // mw->setattent(paintf->getestattval());
    }

    curr_estatt = paintf->getestattval(); // update current estimated attention value

    if (simeeg) // update attention and meditation values for simulated data
    {
        curr_medit = paintf->estmedit; // emulate meditation estimation for simulated data
        paintf->updatemeditation(curr_medit);
        paintf->updateattentionplot(curr_estatt);
    }

    if ((paintfstart) && (paintf->bfiltmode) && (!paintf->game_findsame) && (!paintf->flowmode))
    {
        // filter main image in MindDraw
        if (!pss->spacedview)
            pss->applyfilter();
        else
            pss->applyfilteronbackimg();
    }

    if ((paintfstart) && (paintf->bfiltmode) && (paintf->game_findsame))
    {
        // filter puzzles in MindDraw game mode
        paintf->filtering_allpuzzles(5);
        paintf->filteringmain_ingame(5);
    }  

    if ((paintfstart) && (paintf->grabmindplayflow))
    {
        // streaming of MindPlay image and flow to MindDraw
        pmx = ui->widget->grab();
        paintf->setbackimage(pmx);
        paintf->mainpic=pmx;
        paintf->pmain=pmx;
    }

    // update progress bars of brain waves expression
    update_brainexp_levels(delta, meandelta, theta, meantheta, alpha, meanalpha, beta, meanbeta, gamma, meangamma);

    if (musicmode_on)    
        playtones();  // playing of tones    

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
    cleanbuttons();
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
                ui->spinBox_7->setValue(numst);
            }

           // numst = qrand() % 16; // completely random case
        }
    }
}

void plotwindow::getandprocess_eeg_data(double d1) // getting and processing EEG data
{
    arrc.amp0.push_back(d1);
    counter++;
    buffercount++;
    if (buffercount==srfr*4)
    {
        arrc.amp0.pop_front();
        buffercount--;
    }
    if ((buffercount>=imlength) && (counter>=imlength))
        process_eeg_data();
  //  xraw=(int)d1;
}

void plotwindow::process_eeg_data() // processing EEG data
{
     // check if new fragment is ready
     if (counter>=imlength)
     {        
         if (usefiltering)
             filtercl->zerophasefilt(buffercount-imlength,imlength,arrc.amp0);
         if (updatewavesplot)
             plot_interval();         
         analyse_interval();
         if (attention_interval) // attention modulated length of interval
         {
             if (attention_modulation)
                 curmodval = attent; // paintf->estattn; // in case of estimated attention
             else
                 curmodval = meditt;
             if (curmodval<10)
                 curmodval=10;
             imlength = (curmodval+1)*4;
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
    if (tvals[0]>0)
        tvals[0] = 3 + qrand() % 5; // > mdelta + []
    if (tvals[1]>0)
        tvals[1] = 3 + qrand() % 5; // < mdelta - []
    if (tvals[2]>0)
        tvals[2] = 4 + qrand() % 5; // > mtheta + []
    if (tvals[3]>0)
        tvals[3] = 4 + qrand() % 5; // < mtheta - []
    if (tvals[4]>0)
        tvals[4] = 4 + qrand() % 5; // > malpha + []
    if (tvals[5]>0)
        tvals[5] = 4 + qrand() % 5; // < malpha - []
    if (tvals[6]>0)
        tvals[6] = 5 + qrand() % 6; // > mbeta + []
    if (tvals[7]>0)
        tvals[7] = 5 + qrand() % 6; // < mbeta - []
    if (tvals[8]>0)
        tvals[8] = 5 + qrand() % 6; // > gamma + []
    if (tvals[9]>0)
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
        play_tone8();
        delay(tonedelays[0]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D#"))
    {
        play_tone9();
        delay(tonedelays[1]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("c#"))
    {
        play_tone10();
        delay(tonedelays[2]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("F#"))
    {
        play_tone1();
        delay(tonedelays[3]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("f") && (!tonesset.contains("f#")))
    {
        play_tone6();
        delay(tonedelays[4]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("F") && (!tonesset.contains("F#")))
    {
        play_tone7();
        delay(tonedelays[5]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("d#"))
    {
        play_tone2();
        delay(tonedelays[6]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("G#"))
    {
        play_tone5();
        delay(tonedelays[7]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("A#"))
    {
        play_tone3();
        delay(tonedelays[8]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("f#"))
    {
        play_tone4();
        delay(tonedelays[9]);
        if (chorddelay>0)
            delay(chorddelay);
    }
}

void plotwindow::playtank2(QString tonesset) // playing of tankdrum2 (Bmaj) tones
{
    if (tonesset.contains("c#"))
    {
        play_tone8();
        delay(tonedelays[0]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("B"))
    {
        play_tone9();
        delay(tonedelays[1]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("b"))
    {
        play_tone10();
        delay(tonedelays[2]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D#"))
    {
        play_tone1();
        delay(tonedelays[3]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("f#"))
    {
        play_tone6();
        delay(tonedelays[4]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("C#"))
    {
        play_tone7();
        delay(tonedelays[5]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("d"))
    {
        play_tone2();
        delay(tonedelays[6]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("F#"))
    {
        play_tone5();
        delay(tonedelays[7]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("G#"))
    {
        play_tone3();
        delay(tonedelays[8]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("g#"))
    {
        play_tone4();
        delay(tonedelays[9]);
        if (chorddelay>0)
            delay(chorddelay);
    }
}

void plotwindow::playspace(QString tonesset) // playing of space drum Dmin tones
{    
    if (tonesset.contains("F4"))
    {
        play_tone9();
        delay(tonedelays[1]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D3"))
    {
        play_tone1();
        delay(tonedelays[3]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("E4"))
    {
        play_tone6();
        delay(tonedelays[4]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("F3"))
    {
        play_tone7();
        delay(tonedelays[5]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("A3"))
    {
        play_tone2();
        delay(tonedelays[6]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("D4"))
    {
        play_tone5();
        delay(tonedelays[7]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("C4"))
    {
        play_tone3();
        delay(tonedelays[8]);
        if (chorddelay>0)
            delay(chorddelay);
    }
    if (tonesset.contains("A4"))
    {
        play_tone4();
        delay(tonedelays[9]);
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
        paintf->turn_music_checkbox(true);
    } else
    {        
        if (delta_vals.size()>0)
        {
            musicmode_on=true;
            brainflow_on=true;
        }
        paintf->turn_music_checkbox(false);
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

void plotwindow::osc_streaming(int attent, int meditt, int delta, int theta, int alpha, int beta, int gamma, int hgamma)
// streaming brain data through OSC
{
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );

    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

    int estatt = 0;
    if (paintfstart)
        estatt = paintf->estattn;

    p.Clear();
    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/attention1") << (int)attent << osc::EndMessage
        << osc::BeginMessage("/attention2") << (int)estatt << osc::EndMessage
        << osc::BeginMessage("/meditation") << (int)meditt << osc::EndMessage
        << osc::BeginMessage("/delta") << (int)delta << osc::EndMessage
        << osc::BeginMessage("/theta") << (int)theta << osc::EndMessage
        << osc::BeginMessage("/alpha") << (int)alpha << osc::EndMessage
        << osc::BeginMessage("/beta") << (int)beta << osc::EndMessage
        << osc::BeginMessage("/gamma") << (int)gamma << osc::EndMessage
        << osc::BeginMessage("/hgamma") << (int)hgamma << osc::EndMessage
      << osc::EndBundle;
    transmitSocket.Send(p.Data(),p.Size());
}

void plotwindow::set_nemehanika_bord(int t)  // set border for color changes in neMehanika emulation
{
    nemehanika_bord = t;
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
    if ((mindwstart) || (simeeg))
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
    backimageloaded = true;
    int rimg = rand() % imglist.length();
    currpicfilename = folderpath+"/"+imglist.at(rimg);
    backimg.load(currpicfilename);
    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(false);
    ui->widget->yAxis->grid()->setVisible(false);
    ui->widget->replot();    
    ui->checkBox_6->setChecked(false);
}

void plotwindow::on_pushButton_16_clicked()
{
    play_tone10();
}

void plotwindow::on_pushButton_15_clicked()
{
    play_tone9();
}

void plotwindow::on_pushButton_14_clicked()
{
    play_tone8();
}

void plotwindow::on_pushButton_13_clicked()
{
    play_tone7();
}

void plotwindow::on_pushButton_12_clicked()
{
    play_tone6();
}

void plotwindow::on_pushButton_11_clicked()
{
    play_tone5();
}

void plotwindow::on_pushButton_10_clicked()
{
    play_tone4();
}

void plotwindow::on_pushButton_9_clicked()
{
    play_tone3();
}

void plotwindow::on_pushButton_8_clicked()
{
    play_tone2();
}

void plotwindow::on_pushButton_7_clicked()
{
    play_tone1();
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
    if (ui->checkBox_6->isChecked())
        backimg.load(":/pics/pics.jpg");
    else if (backimageloaded)
        backimg.load(currpicfilename);
    else if ((!backimageloaded) && (tank1mode))
        backimg.load(":/pics/pics/zodiac1.jpg");
    else if ((!backimageloaded) && (tank2mode))
        backimg.load(":/pics/pics/zodiac0.jpg");
    else if (!backimageloaded)
        backimg.load(":/pics/pics/spacedmin.jpg");

    ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
    ui->widget->xAxis->grid()->setVisible(ui->checkBox_6->isChecked());
    ui->widget->yAxis->grid()->setVisible(ui->checkBox_6->isChecked());
    ui->widget->replot();
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
    if (arg1 == 0)
    {
        ui->spinBox_2->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_14->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_2->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_14->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_3_valueChanged(int arg1)
{
    tvals[1]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_3->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_2->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_15->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_3->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_2->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_15->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_4_valueChanged(int arg1)
{
    tvals[2]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_4->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_4->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_16->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_4->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_4->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_16->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_9_valueChanged(int arg1)
{
    tvals[3]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_9->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_3->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_7->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_9->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_3->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_7->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_10_valueChanged(int arg1)
{
    tvals[4]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_10->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_8->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_12->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_10->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_8->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_12->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_11_valueChanged(int arg1)
{
    tvals[5]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_11->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_5->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_13->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_11->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_5->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_13->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_12_valueChanged(int arg1)
{
    tvals[6]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_12->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_14->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_8->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_12->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_14->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_8->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_13_valueChanged(int arg1)
{
    tvals[7]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_13->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_13->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_11->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_13->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_13->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_11->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_14_valueChanged(int arg1)
{
    tvals[8]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_14->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_16->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_9->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_14->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_16->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_9->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
    }
}

void plotwindow::on_spinBox_15_valueChanged(int arg1)
{
    tvals[9]=arg1;
    if (arg1 == 0)
    {
        ui->spinBox_15->setStyleSheet("QSpinBox {background-color : gray;}");
        ui->label_15->setStyleSheet("QLabel {color : green;}");
        ui->pushButton_10->setStyleSheet("background-color: rgba(255, 255, 255, 150);");
    } else
    {
        ui->spinBox_15->setStyleSheet("QSpinBox {background-color : white;}");
        ui->label_15->setStyleSheet("QLabel {color : black;}");
        ui->pushButton_10->setStyleSheet("background-color: rgba(255, 255, 255, 80);");
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
    if (value>100)
        value = 100;
    // if (value<5)
    //    value = 5;
    volume = (qreal) (value) / 100;    
    settonesvolume();
    ui->spinBox_21->setValue(value);
}

void plotwindow::on_radioButton_clicked() // tankdrum1 (Gmaj) mode
{
    spacemode=false;  tank1mode=true; tank2mode=false;
    paintf->setsoundtype(0);
    ui->label->setVisible(true);
    ui->label_4->setVisible(true);
    ui->label->setText("b");
    ui->label_4->setText("c#");
    ui->label_2->setText("D#");
    ui->label_3->setText("F#");
    ui->label_8->setText("f");
    ui->label_5->setText("F");
    ui->label_14->setText("d#");
    ui->label_13->setText("G#");
    ui->label_16->setText("A#");
    ui->label_15->setText("f#");
    ui->pushButton_7->setText("F#");
    ui->pushButton_8->setText("d#");
    ui->pushButton_9->setText("A#");
    ui->pushButton_10->setText("f#");
    ui->pushButton_11->setText("G#");
    ui->pushButton_12->setText("f");
    ui->pushButton_13->setText("F");
    ui->pushButton_14->setText("b");
    ui->pushButton_15->setText("D#");
    ui->pushButton_16->setText("c#");
    ui->spinBox_2->setVisible(true);
    ui->spinBox_4->setVisible(true);
    ui->spinBox_6->setVisible(true);
    ui->spinBox_24->setVisible(true);
  //  if ((!backimageloaded) && (mindplay) && (!hidebutt))
    if ((!hidebutt) && ((musicmode_on) || (delta_vals.size()==0)))
    {
        ui->pushButton_14->setVisible(true);
        ui->pushButton_16->setVisible(true);
    }
    if ((ui->checkBox_6->isChecked()) && (!backimageloaded))
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        if (!camerainp)
            ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else if (!backimageloaded)
    {
        backimg.load(":/pics/pics/zodiac1.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        if (!camerainp)
            ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    }
}


void plotwindow::on_radioButton_2_clicked()  // tankdrum2 (Bmaj) mode
{ 
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
    ui->spinBox_6->setVisible(true);
    ui->spinBox_24->setVisible(true);
  //  if ((!backimageloaded) && (mindplay) && (!hidebutt))
    if ((!hidebutt) && ((musicmode_on) || (delta_vals.size()==0)))
    {
        ui->pushButton_14->setVisible(true);
        ui->pushButton_16->setVisible(true);
    }
    if ((ui->checkBox_6->isChecked()) && (!backimageloaded))
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        if (!camerainp)
            ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else if (!backimageloaded)
    {
        backimg.load(":/pics/pics/zodiac0.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        if (!camerainp)
            ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(false);
        ui->widget->yAxis->grid()->setVisible(false);
    }
}

void plotwindow::on_radioButton_3_clicked() // spacedrum Dmin mode
{
    spacemode=true; tank1mode=false; tank2mode=false;
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
    ui->pushButton_14->setVisible(false);
    ui->pushButton_15->setText("F4");    
    ui->pushButton_16->setVisible(false);
    ui->spinBox_2->setVisible(false);
    ui->spinBox_4->setVisible(false);
    ui->spinBox_6->setVisible(false);
    ui->spinBox_24->setVisible(false);
    if ((ui->checkBox_6->isChecked()) && (!backimageloaded))
    {
        backimg.load(":/pics/pics/empty.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        if (!camerainp)
            ui->widget->replot();
        ui->widget->xAxis->grid()->setVisible(true);
        ui->widget->yAxis->grid()->setVisible(true);
    } else if (!backimageloaded)
    {
        backimg.load(":/pics/pics/spacedmin.jpg");
        ui->widget->setBackground(backimg,true,Qt::IgnoreAspectRatio);
        if (!camerainp)
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
    filteringback = !filteringback;
    colorizeback = false;
    ocvf->showmenu = false;
    ui->checkBox_13->setEnabled(!ui->checkBox_11->isChecked());
    ui->checkBox_14->setEnabled(!ui->checkBox_11->isChecked());
    ui->checkBox_15->setEnabled(!ui->checkBox_11->isChecked());
}

void plotwindow::on_comboBox_currentIndexChanged(int index) // attention / meditation modulation
{
    if ((index==0) || (index==1))
    {
        attention_modulation=true;
        if (index==1)
            est_attention_modulation=true;
        else
            est_attention_modulation=false;
        ui->progressBar->setPalette(sp1);
        ui->checkBox_12->setText("attention modulation");   
        ui->checkBox_8->setText("by attention");
        ui->checkBox_19->setText("by attention");
        ui->comboBox_2->setItemText(1,"by attention");
        brl->settonesbordervisible(switchtonesset_by_att,true);
       // ui->label_23->setVisible(true);
       // ui->label_24->setVisible(false);
    }
    else
    {
        attention_modulation=false;
        ui->progressBar->setPalette(sp2);
        ui->checkBox_12->setText("meditation modulation");
        ui->checkBox_8->setText("by meditation");
        ui->checkBox_19->setText("by meditation");
        ui->comboBox_2->setItemText(1,"by meditation");
        brl->settonesbordervisible(switchtonesset_by_att,false);
       // ui->label_23->setVisible(false);
       // ui->label_24->setVisible(true);
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
    adaptivepicsborder = true;
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

void plotwindow::turn_music_checkbox(bool fl)
{
    ui->checkBox_4->setChecked(fl);
}

void plotwindow::on_horizontalSlider_2_sliderPressed()
{
    adaptivepicsborder = false;
}

void plotwindow::on_comboBox_2_currentIndexChanged(int index)
{
    if (index == 0)
    {       
        attention_volume = false;
        ui->spinBox_21->setStyleSheet("QSpinBox { background-color: white; }");
    } else
    if (index == 1)
    {
        attention_volume = true;
        ui->spinBox_21->setStyleSheet("QSpinBox { background-color: yellow; }");
    }
}

void plotwindow::on_checkBox_8_clicked()
{
    switchtonesset_by_att = !switchtonesset_by_att;
    if (!switchtonesset_by_att)
    {
        brl->settonesbordervisible(switchtonesset_by_att,true);
        ui->radioButton->setEnabled(true);
        ui->radioButton_2->setEnabled(true);
        ui->radioButton_3->setEnabled(true);
        ui->radioButton->setStyleSheet(st_stylesheet);
        ui->radioButton_2->setStyleSheet(st_stylesheet);
        ui->radioButton_3->setStyleSheet(st_stylesheet);
    }
    else
    {
        if (attention_modulation)
            brl->settonesbordervisible(switchtonesset_by_att,true);
        else
            brl->settonesbordervisible(switchtonesset_by_att,false);
        ui->radioButton->setEnabled(false);
        ui->radioButton_2->setEnabled(false);
        ui->radioButton_3->setEnabled(false);
        ui->radioButton->setStyleSheet("QRadioButton { background-color: yellow; }");
        ui->radioButton_2->setStyleSheet("QRadioButton { background-color: yellow; }");
        ui->radioButton_3->setStyleSheet("QRadioButton { background-color: yellow; }");
    }
}

void plotwindow::on_spinBox_6_valueChanged(int arg1)
{
    tonedelays[0] = arg1;
}

void plotwindow::on_spinBox_23_valueChanged(int arg1)
{
    tonedelays[1] = arg1;
}

void plotwindow::on_spinBox_24_valueChanged(int arg1)
{
    tonedelays[2] = arg1;
}

void plotwindow::on_spinBox_25_valueChanged(int arg1)
{
    tonedelays[3] = arg1;
}

void plotwindow::on_spinBox_26_valueChanged(int arg1)
{
    tonedelays[4] = arg1;
}

void plotwindow::on_spinBox_27_valueChanged(int arg1)
{
    tonedelays[5] = arg1;
}

void plotwindow::on_spinBox_28_valueChanged(int arg1)
{
    tonedelays[6] = arg1;
}

void plotwindow::on_spinBox_29_valueChanged(int arg1)
{
    tonedelays[7] = arg1;
}

void plotwindow::on_spinBox_30_valueChanged(int arg1)
{
    tonedelays[8] = arg1;
}

void plotwindow::on_spinBox_31_valueChanged(int arg1)
{
    tonedelays[9] = arg1;
}

void plotwindow::on_checkBox_clicked()
{
    playthetavibe = !playthetavibe;
    if (playthetavibe)
        thetasound.play();
    else
        thetasound.stop();
}

void plotwindow::on_checkBox_16_clicked()
{
    playalphavibe = !playalphavibe;
    if (playalphavibe)
    {
       alphasound.setVolume(wavessound_volume);
       alphasound.play();
    }
    else
       alphasound.stop();
}

void plotwindow::on_checkBox_17_clicked()
{
    playbetavibe = !playbetavibe;
    if (playbetavibe)
    {
        betasound.setVolume(wavessound_volume);
        betasound.play();
    }
    else
        betasound.stop();
}

void plotwindow::on_checkBox_18_clicked()
{
    playgammavibe = !playgammavibe;
    if (playgammavibe)
    {
        gammasound.setVolume(wavessound_volume);
        gammasound.play();
    }
    else
        gammasound.stop();
}

void plotwindow::on_spinBox_valueChanged(int arg1)
{
    wavessound_volume = arg1;
    thetasound.setVolume(wavessound_volume);   
}

void plotwindow::on_checkBox_19_clicked()
{
    waves_sound_modul = !waves_sound_modul;
    if (waves_sound_modul)
    {
        alphasound.setVolume(0);
        alphasound.play();        
        betasound.setVolume(0);
        betasound.play();
        gammasound.setVolume(0);
        gammasound.play();
        ui->checkBox_16->setEnabled(false);
        ui->checkBox_17->setEnabled(false);
        ui->checkBox_18->setEnabled(false);
        ui->checkBox_16->setStyleSheet("QCheckBox { background-color: yellow; }");
        ui->checkBox_17->setStyleSheet("QCheckBox { background-color: yellow; }");
        ui->checkBox_18->setStyleSheet("QCheckBox { background-color: yellow; }");
    }
    else
    {
        alphasound.stop();
        playalphavibe = false;
        betasound.stop();
        playbetavibe = false;
        gammasound.stop();
        playgammavibe = false;
        ui->checkBox_16->setChecked(false);
        ui->checkBox_17->setChecked(false);
        ui->checkBox_18->setChecked(false);
        ui->checkBox_16->setEnabled(true);
        ui->checkBox_17->setEnabled(true);
        ui->checkBox_18->setEnabled(true);
        ui->checkBox_16->setStyleSheet(st_stylesheet);
        ui->checkBox_17->setStyleSheet(st_stylesheet);
        ui->checkBox_18->setStyleSheet(st_stylesheet);
    }
}
