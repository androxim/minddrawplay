#include "paintform.h"
#include "ui_paintform.h"
#include "paintscene.h"
#include <QColorDialog>
#include <QApplication>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include "random"

// TO DO:

// adaptive border for puzzle change
// saving history of waves

// DONE:
// hue filter
// flow mode: gather the whole pic from 15 fragments of it by focusing or relaxing
// focus mode: delay for one random graphicscene related with attention level
// and for all other pics delay is fixed.. when attention>85% change graphic scene on other random


paintform::paintform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paintform)
{
    ui->setupUi(this);
    scene = new paintScene();
    scene->paintf=this;
    fixedmain=true;
    changingpics=false;
    updatingpuzzle=false;
    flowmode=false;
    updateback=false;
    canpuzzlechange=true;
    fnameattent="D:/Androxim/attentflow.dat";
    fnamefreq="D:/Androxim/freqflow.dat";
    ui->checkBox_9->setChecked(true);
    scene->randcolor=false; scene->fixcolor=false; scene->freqcolor=true;
  //  ui->graphicsView->setGeometry(650,250,300,200);
    ui->graphicsView->setGeometry(50,50,1500,800);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setAlignment(Qt::AlignCenter);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_2->setAlignment(Qt::AlignCenter);
    ui->graphicsView_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_3->setAlignment(Qt::AlignCenter);
    ui->graphicsView_3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_4->setAlignment(Qt::AlignCenter);
    ui->graphicsView_4->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_4->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_5->setAlignment(Qt::AlignCenter);
    ui->graphicsView_5->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_5->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_6->setAlignment(Qt::AlignCenter);
    ui->graphicsView_6->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_6->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_7->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_7->setAlignment(Qt::AlignCenter);
    ui->graphicsView_7->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_8->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_8->setAlignment(Qt::AlignCenter);
    ui->graphicsView_8->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_9->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_9->setAlignment(Qt::AlignCenter);
    ui->graphicsView_9->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_10->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_10->setAlignment(Qt::AlignCenter);
    ui->graphicsView_10->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_11->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_11->setAlignment(Qt::AlignCenter);
    ui->graphicsView_11->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_12->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_12->setAlignment(Qt::AlignCenter);
    ui->graphicsView_12->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_13->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_13->setAlignment(Qt::AlignCenter);
    ui->graphicsView_13->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_14->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_14->setAlignment(Qt::AlignCenter);
    ui->graphicsView_14->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_15->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     ui->graphicsView_15->setAlignment(Qt::AlignCenter);
    ui->graphicsView_15->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsView->setFrameStyle(0);
    ui->graphicsView->setStyleSheet("background: transparent");
    ui->graphicsView_2->setFrameStyle(0);
    ui->graphicsView_3->setFrameStyle(0);
    ui->graphicsView_4->setFrameStyle(0);
    ui->graphicsView_5->setFrameStyle(0);
    ui->graphicsView_6->setFrameStyle(0);
    ui->graphicsView_7->setFrameStyle(0);
    ui->graphicsView_8->setFrameStyle(0);
    ui->graphicsView_9->setFrameStyle(0);
    ui->graphicsView_10->setFrameStyle(0);
    ui->graphicsView_11->setFrameStyle(0);
    ui->graphicsView_12->setFrameStyle(0);
    ui->graphicsView_13->setFrameStyle(0);
    ui->graphicsView_14->setFrameStyle(0);
    ui->graphicsView_15->setFrameStyle(0);

    ui->checkBox_16->setEnabled(false);

    QGraphicsScene* scene2 = new QGraphicsScene();  
    ui->graphicsView_2->setScene(scene2);
    QGraphicsScene* scene3 = new QGraphicsScene();
    scene3->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_3->setScene(scene3);
    QGraphicsScene* scene4 = new QGraphicsScene();
    scene4->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_4->setScene(scene4);
    QGraphicsScene* scene5 = new QGraphicsScene();
    scene5->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_5->setScene(scene5);    
    QGraphicsScene* scene6 = new QGraphicsScene();
    scene6->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_6->setScene(scene6);
    QGraphicsScene* scene7 = new QGraphicsScene();
    scene7->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_7->setScene(scene7);
    QGraphicsScene* scene8 = new QGraphicsScene();
    scene8->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_8->setScene(scene8);
    QGraphicsScene* scene9 = new QGraphicsScene();
    scene9->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_9->setScene(scene9);
    QGraphicsScene* scene10 = new QGraphicsScene();
    scene10->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_10->setScene(scene10);
    QGraphicsScene* scene11 = new QGraphicsScene();
    scene11->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_11->setScene(scene11);
    QGraphicsScene* scene12 = new QGraphicsScene();
    scene12->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_12->setScene(scene12);
    QGraphicsScene* scene13 = new QGraphicsScene();
    scene13->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_13->setScene(scene13);
    QGraphicsScene* scene14 = new QGraphicsScene();
    scene14->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_14->setScene(scene14);
    QGraphicsScene* scene15 = new QGraphicsScene();
    scene15->setSceneRect(QRectF(0,0,puzzlew,puzzleh));
    ui->graphicsView_15->setScene(scene15);

    ui->graphicsView_2->setVisible(false);
    ui->graphicsView_3->setVisible(false);
    ui->graphicsView_4->setVisible(false);
    ui->graphicsView_5->setVisible(false);
    ui->graphicsView_6->setVisible(false);
    ui->graphicsView_7->setVisible(false);
    ui->graphicsView_8->setVisible(false);
    ui->graphicsView_9->setVisible(false);
    ui->graphicsView_10->setVisible(false);
    ui->graphicsView_11->setVisible(false);
    ui->graphicsView_12->setVisible(false);
    ui->graphicsView_13->setVisible(false);
    ui->graphicsView_14->setVisible(false);
    ui->graphicsView_15->setVisible(false);
    ui->graphicsView_6->setGeometry(50,50,300,200);
    ui->graphicsView_7->setGeometry(350,50,300,200);
    ui->graphicsView_8->setGeometry(650,50,300,200);
    ui->graphicsView_9->setGeometry(950,50,300,200);
    ui->graphicsView_10->setGeometry(1250,50,300,200);

    ui->graphicsView_3->setGeometry(50,250,300,200);
    ui->graphicsView_2->setGeometry(350,250,300,200);
    ui->graphicsView_4->setGeometry(950,250,300,200);
    ui->graphicsView_5->setGeometry(1250,250,300,200);

    ui->graphicsView_11->setGeometry(50,450,300,200);
    ui->graphicsView_12->setGeometry(350,450,300,200);
    ui->graphicsView_13->setGeometry(650,450,300,200);
    ui->graphicsView_14->setGeometry(950,450,300,200);
    ui->graphicsView_15->setGeometry(1250,450,300,200);
    scene->randfixcolor=true;
    ui->radioButton_6->setChecked(true);

    rationmode = Qt::IgnoreAspectRatio;

    firstpuzzle = true;

    currentindexes = new int[14];
    for (int i=0; i<14; i++)
        currentindexes[i]=0;
    picsforchange=4;
    limitpicschange=false;

    borderlevel=77;
    borderpicchange=75;

    showestatt=true;

    ui->checkBox_17->setEnabled(false);

  //  scene->setSceneRect(ui->graphicsView->rect());
    scene->setSceneRect(QRectF(0,0, ui->graphicsView->width(), ui->graphicsView->height()));
    ui->graphicsView->setScene(scene);
    ui->pushButton_3->setGeometry(660,865,100,30);
    ui->radioButton_4->setGeometry(740,905,150,20);
    ui->radioButton_5->setGeometry(740,930,150,20);
    ui->pushButton->setGeometry(780,865,70,20);
    ui->pushButton_11->setGeometry(780,885,70,20);
    ui->label_4->setGeometry(850,920,120,20);
    ui->label_5->setGeometry(950,910,70,50);
    ui->pushButton_4->setGeometry(860,865,70,30);
    ui->pushButton_9->setGeometry(940,865,80,30);
    ui->checkBox_8->setGeometry(1070,905,100,20);
    ui->checkBox_8->setEnabled(false);
    ui->verticalSlider->setGeometry(1038,820,20,150);
    ui->checkBox_10->setGeometry(1070,930,160,20);
    ui->checkBox_18->setGeometry(1070,950,160,20);
    ui->checkBox_10->setEnabled(false);
    ui->spinBox_3->setGeometry(1225,929,50,20);
    ui->label_3->setGeometry(1280,929,30,20);
    ui->checkBox_9->setGeometry(1200,905,100,20);
    ui->pushButton_5->setGeometry(1070,865,80,30);
    ui->pushButton_2->setGeometry(1160,865,100,30);
    ui->pushButton_6->setGeometry(1270,865,100,30);
    ui->pushButton_7->setGeometry(1380,865,100,30);
    ui->checkBox_16->setGeometry(1490,855,85,30);
    ui->checkBox_17->setGeometry(1490,873,55,30);
    ui->pushButton_8->setGeometry(1310,900,100,20);
    ui->pushButton_10->setGeometry(1310,920,100,20);
    ui->checkBox_11->setGeometry(1310,940,120,25);
    ui->spinBox_4->setGeometry(1410,940,35,25);
    ui->pushButton_7->setEnabled(false);
    ui->comboBox->setGeometry(1420,900,130,25);
    ui->comboBox_2->setGeometry(1450,930,100,20);
    ui->checkBox->setGeometry(550,860,110,20);
    ui->checkBox_2->setGeometry(550,880,110,20);
    ui->radioButton->setGeometry(554,860,120,20);
    ui->radioButton_2->setGeometry(554,883,120,20);
    ui->radioButton_3->setGeometry(554,905,145,20);
    ui->radioButton_6->setGeometry(554,927,100,20);
  //  ui->radioButton->setChecked(scene->randcolor);
  //  ui->radioButton_2->setChecked(scene->fixcolor);
  //  ui->radioButton_3->setChecked(scene->freqcolor);
    ui->label->setGeometry(380,853,100,20);
    ui->spinBox->setGeometry(475,853,40,20);
    ui->label_2->setGeometry(260,853,60,20);
    ui->label_7->setGeometry(260,878,120,20);
    ui->spinBox_6->setGeometry(370,878,40,20);
    ui->checkBox_20->setGeometry(420,878,100,20);
    ui->checkBox_21->setGeometry(486,878,60,20);
    ui->checkBox_21->setVisible(false);
    ui->checkBox_4->setGeometry(260,905,120,20);
    ui->checkBox_4->setVisible(false);
    ui->checkBox_15->setGeometry(260,905,160,20);
    ui->spinBox_5->setGeometry(417,906,40,20);
    ui->checkBox_19->setGeometry(461,904,71,20);
    ui->verticalSlider_2->setGeometry(533,855,20,70);
    ui->spinBox_5->setEnabled(true);
    ui->verticalSlider_2->setEnabled(true);
    ui->checkBox_13->setGeometry(400,930,120,20);
    ui->checkBox_14->setGeometry(260,930,120,20);    
    ui->checkBox_5->setGeometry(400,905,120,20);
    ui->checkBox_5->setVisible(false);
    ui->checkBox_3->setGeometry(80,858,120,20);
    ui->widget->setGeometry(50,656,740,195);
    ui->widget->setVisible(true);
    ui->widget_2->setGeometry(805,656,745,195);
    ui->widget_2->setVisible(true);
    ui->widget_2->yAxis->setLabel("%");
    ui->widget_2->rescaleAxes();
  //  ui->widget->xAxis->setLabel("sec");
    ui->widget->yAxis->setLabel("%");
    ui->widget->rescaleAxes();
    ui->checkBox_6->setGeometry(80,883,140,20);
    ui->checkBox_7->setGeometry(80,908,140,20);
    ui->checkBox_12->setGeometry(80,933,99,20);

    ui->checkBox_12->setChecked(true);
    ui->spinBox_2->setGeometry(320,853,40,20);
    ui->checkBox->setChecked(scene->randcolor);
    ui->checkBox->setVisible(false);
    ui->checkBox_2->setChecked(scene->drawbpoints);
    ui->checkBox_2->setVisible(false);
    eegsize=2; pensize=3; bfiltmode=false;
    ui->spinBox->setValue(eegsize);
    ui->spinBox_2->setValue(pensize);
    erasepen=false; qimload=false; puzzlemode=false;
    ui->checkBox_8->setChecked(puzzlemode);
    ui->checkBox_13->setEnabled(false);
    ui->graphicsView->installEventFilter(this);
    ui->graphicsView_2->installEventFilter(this);
    ui->graphicsView_3->installEventFilter(this);
    ui->graphicsView_4->installEventFilter(this);
    ui->graphicsView_5->installEventFilter(this);
    ui->graphicsView_6->installEventFilter(this);
    ui->graphicsView_7->installEventFilter(this);
    ui->graphicsView_8->installEventFilter(this);
    ui->graphicsView_9->installEventFilter(this);
    ui->graphicsView_10->installEventFilter(this);
    ui->graphicsView_11->installEventFilter(this);
    ui->graphicsView_12->installEventFilter(this);
    ui->graphicsView_13->installEventFilter(this);
    ui->graphicsView_14->installEventFilter(this);
    ui->graphicsView_15->installEventFilter(this);

    numsamples=0;
    numfrsamples=0;
    setloaded=false;
    gamemode=false;
    musicactiv=false;
    puzzlegrabed=false;
    collectiveflow=true;
    spacedflow=false; 

    prevpict = -1;
    prevpuzzle = -1;

    attent_arr = QVector<double>(TPMAX);
    medit_arr = QVector<double>(TPMAX);
    border_arr = QVector<double>(TPMAX);

    delta_arr = QVector<double>(TFMAX);
    theta_arr = QVector<double>(TFMAX);
    alpha_arr = QVector<double>(TFMAX);
    beta_arr = QVector<double>(TFMAX);
    gamma_arr = QVector<double>(TFMAX);
    hgamma_arr = QVector<double>(TFMAX);
    estatt_arr = QVector<double>(TFMAX);
    fxc = QVector<double>(TFMAX);

    currimglist = QVector<QString>(15);

    for (int j=0; j<TPMAX; j++)
    {
        attent_arr[j]=0;
        medit_arr[j]=0;
        border_arr[j]=0;
    }

    for (int j=0; j<TFMAX; j++)
    {
        delta_arr[j]=0;
        theta_arr[j]=0;
        alpha_arr[j]=0;
        beta_arr[j]=0;
        gamma_arr[j]=0;
        hgamma_arr[j]=0;
        estatt_arr[j]=0;
    }

    xc = QVector<double>(TPMAX);
    for (int j=0; j<TPMAX; j++)
        xc[j]=0;

    fxc = QVector<double>(TFMAX);
    for (int j=0; j<TFMAX; j++)
        fxc[j]=0;

    puzzlew=300;
    puzzleh=200;
    ui->checkBox_11->setEnabled(false);
  //  ui->radioButton_3->setChecked(true);

    laststop=0;
    lenofinterval=2;

  // pmain = mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation);
  //  scene->addPixmap(mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
  //  scene->update();

  //  scene->clear();
  //  scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
  //  this->repaint();

    ui->widget->xAxis->setRange(0,32);
    ui->widget->yAxis->setRange(0,101);
    ui->widget->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(6);
    legendFont.setBold(false); 
    ui->widget->legend->setFont(legendFont);
    ui->widget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->widget->legend->setBrush(QBrush(QColor(255,255,255,190)));

    ui->widget_2->xAxis->setRange(0,32);
    ui->widget_2->yAxis->setRange(0,100);

    ui->widget->addGraph();
    ui->widget->graph(0)->setPen(QPen(Qt::darkRed));
    ui->widget->graph(0)->setName("Attention");
    ui->widget->graph(0)->setData(xc, attent_arr);
    ui->widget->graph(0)->selectionDecorator()->setPen(QPen(Qt::red));

    ui->widget->addGraph();
    ui->widget->setInteraction(QCP::iSelectPlottables, true);
    ui->widget->graph(1)->setPen(QPen(Qt::darkGreen));
    ui->widget->graph(1)->setName("Meditation");
    ui->widget->graph(1)->setData(xc, medit_arr);
    ui->widget->graph(1)->selectionDecorator()->setPen(QPen(Qt::green));
 //   ui->widget->setBackground(QBrush(QColor(255,255,255,110)));
    ui->widget->replot();
    ui->widget->setAutoFillBackground(false);

     ui->widget->addGraph();
     mypen.setColor("orange");
     ui->widget->graph(2)->setPen(QPen("orange"));
     ui->widget->graph(2)->setName("Border");
     ui->widget->graph(2)->setData(xc, border_arr);

  /*  ui->widget->setPalette(Qt::transparent);
    ui->widget->setWindowOpacity(0);
    ui->widget->setAttribute(Qt::WA_NoSystemBackground);
    ui->widget->setAttribute(Qt::WA_TranslucentBackground);
    ui->widget->setAttribute(Qt::WA_PaintOnScreen); */


    ui->widget_2->legend->setVisible(true);
    ui->widget_2->setInteraction(QCP::iSelectPlottables, true);
    ui->widget_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->widget_2->legend->setFont(legendFont);
 //   ui->widget_2->setBackground(QBrush(QColor(255,255,255,110)));
    ui->widget_2->legend->setBrush(QBrush(QColor(255,255,255,190)));
 /*   ui->widget_2->setAutoFillBackground(false);
    ui->widget_2->setPalette(Qt::transparent);
    ui->widget_2->setWindowOpacity(0);
    ui->widget_2->setAttribute(Qt::WA_NoSystemBackground);
    ui->widget_2->setAttribute(Qt::WA_TranslucentBackground);
    ui->widget_2->setAttribute(Qt::WA_PaintOnScreen); */

    ui->widget_2->addGraph();
    ui->widget_2->graph(0)->setPen(QPen(Qt::yellow));
    ui->widget_2->graph(0)->setName("Delta");
    ui->widget_2->graph(0)->setData(fxc, delta_arr);

    ui->widget_2->addGraph();
    ui->widget_2->graph(1)->setPen(QPen(Qt::cyan));
    ui->widget_2->graph(1)->setName("Theta");
    ui->widget_2->graph(1)->setData(fxc, theta_arr);

    ui->widget_2->addGraph();
    ui->widget_2->graph(2)->setPen(QPen(Qt::magenta));
    ui->widget_2->graph(2)->setName("Alpha");
    ui->widget_2->graph(2)->setData(fxc, alpha_arr);        
    ui->widget_2->graph(2)->selectionDecorator()->setPen(QPen(Qt::magenta));
    QPen qp = ui->widget_2->graph(2)->pen();
    qp.setWidth(3);
    ui->widget_2->graph(2)->selectionDecorator()->setPen(qp);

    ui->widget_2->addGraph();
    ui->widget_2->graph(3)->setPen(QPen(Qt::blue));
    ui->widget_2->graph(3)->setName("Beta");
    ui->widget_2->graph(3)->setData(fxc, beta_arr);

    ui->widget_2->addGraph();
    ui->widget_2->graph(4)->setPen(QPen(Qt::gray));
    ui->widget_2->graph(4)->setName("Gamma");
    ui->widget_2->graph(4)->setData(fxc, gamma_arr);

    ui->widget_2->addGraph();
    ui->widget_2->graph(5)->setPen(QPen(Qt::darkGray));
    ui->widget_2->graph(5)->setName("H-Gamma");
    ui->widget_2->graph(5)->setData(fxc, hgamma_arr);

    ui->widget_2->addGraph();
    ui->widget_2->graph(6)->setPen(QPen(Qt::darkRed));
    ui->widget_2->graph(6)->setName("Est-Attention");
    ui->widget_2->graph(6)->setData(fxc, estatt_arr);

    ui->widget->setAttribute(Qt::WA_TranslucentBackground,true);
    ui->widget->setWindowOpacity(0.5);
 //   ui->widget_2->setAttribute(Qt::WA_TranslucentBackground,true);
 //   ui->widget_2->setWindowOpacity(0.5);
    ui->widget_2->replot();

    sp1.setColor(QPalette::Window, Qt::white);
    sp1.setColor(QPalette::WindowText, Qt::red);

    sp2.setColor(QPalette::Window, Qt::white);
    sp2.setColor(QPalette::WindowText, Qt::darkGreen);

    ui->label_23->setGeometry(50,25,181,26);
    ui->label_23->setPalette(sp1);
    ui->label_24->setGeometry(250,25,181,26);
    ui->label_24->setPalette(sp2);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    for (int i=0; i<15; i++)
        stpic1.insert(i);

    folderpath="D:/PICS";
    fd.setPath(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    randnumb = vector<int>(imglist.length());
    iota(randnumb.begin(), randnumb.end(), 0);
    random_shuffle(randnumb.begin(), randnumb.end());

    minimode=false;
    pmarray.resize(imglist.length());
    onepicarr.resize(15);

    puzzlelocs = vector<int>(15);
    iota(puzzlelocs.begin(), puzzlelocs.end(), 0);
    random_shuffle(puzzlelocs.begin(), puzzlelocs.end());

    randpuzzlelocs = vector<int>(15);
    iota(randpuzzlelocs.begin(), randpuzzlelocs.end(), 0);
    random_shuffle(randpuzzlelocs.begin(), randpuzzlelocs.end());

    sortpuzzlelocs = vector<int>(15);

    ui->label_23->setVisible(true);
    ui->label_24->setVisible(false);   QDateTime current = QDateTime::currentDateTime();
    QString st = current.toLocalTime().toString();
    int lt = st.length();
    st = st.mid(lt-13,lt-9);
   // "D:/Androxim/freqflow.dat";
  //  fnameattent="D:/Androxim/"+st+"_attentflow.dat";
 //   fnamefreq="D:/Androxim/"+st+"_freqflow.dat";

    timepics=2000;
    ui->spinBox_3->setValue(timepics);
    tpicschange = new QTimer(this);
    tpicschange->connect(tpicschange, SIGNAL(timeout()), this, SLOT(tpicschangeUpdate()));
    tpicschange->setInterval(timepics);

    attentmodu=true;
    ui->radioButton_4->setChecked(true);
    ui->spinBox_6->setValue(borderpicchange);

    for (int i=0; i<108; i++)
    {
        centercoord[i][0]=qrand()%1500;
        centercoord[i][1]=qrand()%800;
        poltypearr[i]=3+qrand()%6;
    }

    adaptivebord=false;
  //  ui->comboBox_2->setCurrentIndex(1);
  //  scene->filllines();

    pt=5;

    pmain.load(":/pics/pics/empty.jpg");
    mainpic.load(":/pics/pics/empty.jpg");
    qim.load(":/pics/pics/empty.jpg");
    qimload = true;
    backloaded = false;
}

paintform::~paintform()
{
    delete ui;
}

void paintform::setdflowtime(int t)
{
    if (t<30)
        tpicschange->setInterval(300);
    else if ((t>30) && (t<40))
        tpicschange->setInterval(400);
    else if ((t>40) && (t<50))
        tpicschange->setInterval(500);
    else if ((t>50) && (t<60))
        tpicschange->setInterval(600);
    else if ((t>60) && (t<70))
        tpicschange->setInterval(700);
    else if ((t>70) && (t<80))
        tpicschange->setInterval(800);
    else if ((t>80) && (t<90))
        tpicschange->setInterval(1000);
    else if (t>90)
        tpicschange->setInterval(1200);
    ui->spinBox_3->setValue(tpicschange->interval());
}

void paintform::setflowspace(int t)
{
    if ((t<85) && (!canpuzzlechange))
    {
        delay(200);
        on_pushButton_6_clicked();
        on_checkBox_16_clicked();
        canpuzzlechange=true;
    }
    if (t<20)
       {
           picsforchange=12;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>20) && (t<30))
       {
           picsforchange=9;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>30) && (t<40))
       {
           if (collectiveflow)
               picsforchange=7;
           else
               picsforchange=8;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>40) && (t<50))
       {
           if (collectiveflow)
               picsforchange=5;
           else
               picsforchange=6;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>50) && (t<60))
       {
           if (collectiveflow)
               picsforchange=4;
           else
               picsforchange=5;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>60) && (t<70))
       {
           if (collectiveflow)
               picsforchange=3;
           else
               picsforchange=4;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>70) && (t<80))
       {
           if (collectiveflow)
               picsforchange=2;
           else
               picsforchange=3;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>80) && (t<85))
       {
           if (collectiveflow)
               picsforchange=1;
           else
               picsforchange=2;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if (t>85)
       {
           picsforchange=0;
           ui->spinBox_4->setValue(picsforchange);
           if ((flowmode) && (canpuzzlechange))
           {              
               matchpuzzle();               
               on_checkBox_16_clicked();
               canpuzzlechange=false;
           }
       }
}

bool paintform::getattentmode()
{
    return attentmodu;
}

double paintform::getestattval()
{
    if (numfrsamples>0)
        return estattn;
    else
        return 0;
}

void paintform::updatefreqarrs(double deltat, double thetat, double alphat, double betat, double gammat, double hgammat)
{
  //  qDebug()<<numfrsamples;
  //  if (numfrsamples>TFMAX)
   //     numfrsamples=0;

    /*
    QFile outputFile(fnamefreq);
    outputFile.open( QIODevice::Append);
    QTextStream fout(&outputFile);
    fout<<deltat<<",";
    fout<<thetat<<",";
    fout<<alphat<<",";
    fout<<betat<<",";
    fout<<gammat<<" "; */

    if (numfrsamples>32)
        ui->widget_2->xAxis->moveRange(1);

    delta_arr[numfrsamples]=deltat;
    theta_arr[numfrsamples]=thetat;
    alpha_arr[numfrsamples]=alphat;
    beta_arr[numfrsamples]=betat;
    gamma_arr[numfrsamples]=gammat;
    hgamma_arr[numfrsamples]=hgammat;

    thet=0;
    bet=0;

    if (numfrsamples<pt)
        estattn=(1-thetat/betat)*100;
    else
    {
        for (int i=0; i<pt; i++)
        {
            thet+=theta_arr[numfrsamples-i];
            bet+=beta_arr[numfrsamples-i];
        }
        thet/=pt;
        bet/=pt;
        estattn=((1-thet/bet)-0.4)/0.4*100;
    }
    if (estattn<0) estattn=0;
    if (estattn>100) estattn=100;
    estatt_arr[numfrsamples]=estattn;

    fxc[numfrsamples]=numfrsamples;

    ui->widget_2->graph(0)->setData(fxc, delta_arr);
    ui->widget_2->graph(1)->setData(fxc, theta_arr);
    ui->widget_2->graph(2)->setData(fxc, alpha_arr);
    ui->widget_2->graph(3)->setData(fxc, beta_arr);
    ui->widget_2->graph(4)->setData(fxc, gamma_arr);
    ui->widget_2->graph(5)->setData(fxc, hgamma_arr);
    ui->widget_2->graph(6)->setData(fxc, estatt_arr);

    ui->widget_2->replot();

  //  scene->clear();
  //  plane.clear();
 //   for (int i=0; i<5; i++)
 //   {
 //       centercoord[i][0]=qrand()%1500;
  //      centercoord[i][1]=qrand()%800;
   //     plane<<QPointF(centercoord[i][0],centercoord[i][1]);
   // }
 /*   scene->drawpolygon(3,centercoord[0][0],centercoord[0][1],deltat*3,0.5);
    scene->drawpolygon(5,centercoord[1][0],centercoord[1][1],thetat,0.5);
    scene->drawpolygon(6,centercoord[2][0],centercoord[2][1],alphat,0.5);
    scene->drawpolygon(7,centercoord[3][0],centercoord[3][1],betat,0.5);
    scene->drawpolygon(8,centercoord[4][0],centercoord[4][1],gammat*4,0.5); */
   // scene->addPolygon(plane,QPen(scene->drcolor, 2, Qt::SolidLine, Qt::RoundCap));

    numfrsamples++;
}

void paintform::startpolyt()
{
    polyt->start();
}

void paintform::loadempty()
{
    on_pushButton_clicked();
}

void paintform::initpics()
{
    QString filename;
    QPixmap temp;
    ui->graphicsView_2->setVisible(true);
    if ((puzzlemode) && (setloaded))
        temp = ui->graphicsView_2->grab(ui->graphicsView_2->sceneRect().toRect());
    ui->graphicsView_2->grab(ui->graphicsView_2->sceneRect().toRect());
    for (int i=0; i<imglist.length(); i++)
    {
        filename=folderpath+"/"+imglist.at(i);
        pm.load(filename);
       // ui->graphicsView_2->setStyleSheet("border: 0px");
       // ui->graphicsView_2->setContentsMargins(0,0,0,0);
        ui->graphicsView_2->scene()->clear();
        ui->graphicsView_2->setAlignment(Qt::AlignCenter);
        ui->graphicsView_2->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
       // ui->graphicsView_2->setAlignment(Qt::AlignCenter);
        pmarray[i] = ui->graphicsView_2->grab(ui->graphicsView_2->sceneRect().toRect());
    }
    minimode=true;
    ui->graphicsView_2->scene()->clear();
    if ((puzzlemode) && (setloaded))
        ui->graphicsView_2->scene()->addPixmap(temp.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
    if (!puzzlemode)
        ui->graphicsView_2->setVisible(false);
}

void paintform::updateattentionplot(int t)
{
    if (numsamples>32)
        ui->widget->xAxis->moveRange(1);
    attent_arr[numsamples]=t;
    border_arr[numsamples]=borderlevel;
    xc[numsamples]=numsamples;

    ui->widget->graph(0)->setData(xc, attent_arr);
    ui->widget->graph(2)->setData(xc, border_arr);

    ui->widget->replot();

    numsamples++;

    updateattention(t);
}

void paintform::updateattention(int t)
{
    scene->attentt=t;

  /*  QFile outputFile(fnameattent);
    outputFile.open( QIODevice::Append);
    QTextStream fout(&outputFile);
    fout<<attent_arr[numsamples]<<",";
    fout<<medit_arr[numsamples]<<",";
    fout<<border_arr[numsamples]<<" ";
    outputFile.close(); */

    if (attentmodu)
    {

        scene->drawrate=70-t;
        if (scene->drawrate<5)
            scene->drawrate=5;
        scene->tim->setInterval(scene->drawrate);
        ui->verticalSlider->setValue(t);

       if ((bfiltmode) || (!canpuzzlechange))
           setdflowtime(t);

       if ((bfiltmode) || (!canpuzzlechange))
           setflowspace(t);

       if ((numsamples-laststop>lenofinterval) && (adaptivebord))
       {
           avgv=0;
           for (int i=numsamples-lenofinterval; i<numsamples; i++)
               avgv+=attent_arr[i];
           avgv/=lenofinterval;
           if (avgv<95)
               borderlevel=avgv;
           else
               borderlevel=99;
           laststop=numsamples;
           ui->verticalSlider_2->setValue(borderlevel);
           ui->spinBox_5->setValue(borderlevel);
       }

       if (musicactiv)
       {
           if (t>borderlevel)
           {
               pw->addmodeon=true;
               pw->mindplay=true;
               mypen = ui->widget->graph(2)->pen();
               mypen.setWidth(3);
               ui->widget->graph(2)->setPen(mypen);
           } else
           {
               mypen.setWidth(1);
               ui->widget->graph(2)->setPen(mypen);
               ui->widget->graph(2)->pen().setWidth(1);
               pw->addmodeon=false;
               pw->mindplay=false;
           }
       }

       ui->label_23->setStyleSheet("QLabel { color : red; }");
       ui->radioButton_4->setStyleSheet("QRadioButton { color : red; }");

       ui->label_5->setText(QString::number(t)+"%");

       ui->label_23->setText("ATTENTION: "+QString::number(t)+"%");
       if ((!fixedmain) && (!flowmode) && (!gamemode) && (t>borderpicchange) && (bfiltmode))
           on_pushButton_6_clicked();
    }

  //  scene->clear();
 //   scene->drawpolygon(scene->attentt/10,700,400,scene->meditt,0.5);
}

void paintform::updatemeditation(int t)
{
    scene->meditt=t;

    medit_arr[numsamples]=t;
   // xc[numsamples]=numsamples;

    ui->widget->graph(1)->setData(xc, medit_arr);

    ui->widget->replot();

   // numsamples++;

    if (!attentmodu)
    {
      // scene->drawrate=70-t;
      // if (scene->drawrate<5)
      //     scene->drawrate=5;
      // scene->tim->setInterval(scene->drawrate);
       ui->verticalSlider->setValue(t);

       if ((bfiltmode) || (!canpuzzlechange))
           setdflowtime(t);

       if ((bfiltmode) || (!canpuzzlechange))
           setflowspace(t);

       if ((numsamples-laststop>lenofinterval) && (adaptivebord))
       {
           avgv=0;
           for (int i=numsamples-lenofinterval; i<numsamples; i++)
               avgv+=medit_arr[i];
           avgv/=lenofinterval;
           if (avgv<95)
               borderlevel=avgv;
           else
               borderlevel=99;
           laststop=numsamples;
           ui->verticalSlider_2->setValue(borderlevel);
           ui->spinBox_5->setValue(borderlevel);
       }

       if (musicactiv)
       {
           if (t>borderlevel)
           {
               pw->addmodeon=true;
               pw->mindplay=true;
               mypen = ui->widget->graph(2)->pen();
               mypen.setWidth(3);
               ui->widget->graph(2)->setPen(mypen);
           } else
           {
               mypen.setWidth(1);
               ui->widget->graph(2)->setPen(mypen);
               ui->widget->graph(2)->pen().setWidth(1);
               pw->addmodeon=false;
               pw->mindplay=false;
           }
       } else

       ui->label_24->setStyleSheet("QLabel { color : darkGreen; }");
       ui->radioButton_5->setStyleSheet("QRadioButton { color : darkGreen; }");

       ui->label_5->setText(QString::number(t)+"%");

       ui->label_24->setText("MEDITATION: "+QString::number(t)+"%");
       if ((!fixedmain) && (!flowmode) && (!gamemode) && (t>borderpicchange) && (bfiltmode))
           on_pushButton_6_clicked();
    }
}

void paintform::randompics()
{
    random_shuffle(randnumb.begin(), randnumb.end());
    if (!limitpicschange)
    for (int i=0; i<14; i++)
        currentindexes[i]=randnumb[i];
    {
        if ((limitpicschange) && (!flowmode))
        {
            random_shuffle(randnumb.begin(), randnumb.end());
            for (int i=0; i<picsforchange; i++)
            {
                int t = qrand() % 14;
                currentindexes[t]=randnumb[i];
            }

            ui->graphicsView_2->scene()->clear();
            ui->graphicsView_2->scene()->addPixmap(pmarray[currentindexes[0]]);
            currimglist[0]=folderpath+"/"+imglist.at(currentindexes[0]);

            ui->graphicsView_3->scene()->clear();
            ui->graphicsView_3->scene()->addPixmap(pmarray[currentindexes[1]]);
            currimglist[1]=folderpath+"/"+imglist.at(currentindexes[1]);

            ui->graphicsView_4->scene()->clear();
            ui->graphicsView_4->scene()->addPixmap(pmarray[currentindexes[2]]);
            currimglist[2]=folderpath+"/"+imglist.at(currentindexes[2]);

            ui->graphicsView_5->scene()->clear();
            ui->graphicsView_5->scene()->addPixmap(pmarray[currentindexes[3]]);
            currimglist[3]=folderpath+"/"+imglist.at(currentindexes[3]);

            ui->graphicsView_6->scene()->clear();
            ui->graphicsView_6->scene()->addPixmap(pmarray[currentindexes[4]]);
            currimglist[4]=folderpath+"/"+imglist.at(currentindexes[4]);

            ui->graphicsView_7->scene()->clear();
            ui->graphicsView_7->scene()->addPixmap(pmarray[currentindexes[5]]);
            currimglist[5]=folderpath+"/"+imglist.at(currentindexes[5]);

            ui->graphicsView_8->scene()->clear();
            ui->graphicsView_8->scene()->addPixmap(pmarray[currentindexes[6]]);
            currimglist[6]=folderpath+"/"+imglist.at(currentindexes[6]);

            ui->graphicsView_9->scene()->clear();
            ui->graphicsView_9->scene()->addPixmap(pmarray[currentindexes[7]]);
            currimglist[7]=folderpath+"/"+imglist.at(currentindexes[7]);

            ui->graphicsView_10->scene()->clear();
            ui->graphicsView_10->scene()->addPixmap(pmarray[currentindexes[8]]);
            currimglist[8]=folderpath+"/"+imglist.at(currentindexes[8]);

            ui->graphicsView_11->scene()->clear();
            ui->graphicsView_11->scene()->addPixmap(pmarray[currentindexes[9]]);
            currimglist[9]=folderpath+"/"+imglist.at(currentindexes[9]);

            ui->graphicsView_12->scene()->clear();
            ui->graphicsView_12->scene()->addPixmap(pmarray[currentindexes[10]]);
            currimglist[10]=folderpath+"/"+imglist.at(currentindexes[10]);

            ui->graphicsView_13->scene()->clear();
            ui->graphicsView_13->scene()->addPixmap(pmarray[currentindexes[11]]);
            currimglist[11]=folderpath+"/"+imglist.at(currentindexes[11]);

            ui->graphicsView_14->scene()->clear();
            ui->graphicsView_14->scene()->addPixmap(pmarray[currentindexes[12]]);
            currimglist[12]=folderpath+"/"+imglist.at(currentindexes[12]);

            ui->graphicsView_15->scene()->clear();
            ui->graphicsView_15->scene()->addPixmap(pmarray[currentindexes[13]]);
            currimglist[13]=folderpath+"/"+imglist.at(currentindexes[13]);
        }
        else if ((!flowmode) && (collectiveflow))
        {

            ui->graphicsView_2->scene()->clear();
            ui->graphicsView_2->scene()->addPixmap(pmarray[randnumb[0]]);
            currimglist[0]=folderpath+"/"+imglist.at(randnumb[0]);

            ui->graphicsView_3->scene()->clear();
            ui->graphicsView_3->scene()->addPixmap(pmarray[randnumb[1]]);
            currimglist[1]=folderpath+"/"+imglist.at(randnumb[1]);

            ui->graphicsView_4->scene()->clear();
            ui->graphicsView_4->scene()->addPixmap(pmarray[randnumb[2]]);
            currimglist[2]=folderpath+"/"+imglist.at(randnumb[2]);

            ui->graphicsView_5->scene()->clear();
            ui->graphicsView_5->scene()->addPixmap(pmarray[randnumb[3]]);
            currimglist[3]=folderpath+"/"+imglist.at(randnumb[3]);

            ui->graphicsView_6->scene()->clear();
            ui->graphicsView_6->scene()->addPixmap(pmarray[randnumb[4]]);
            currimglist[4]=folderpath+"/"+imglist.at(randnumb[4]);

            ui->graphicsView_7->scene()->clear();
            ui->graphicsView_7->scene()->addPixmap(pmarray[randnumb[5]]);
            currimglist[5]=folderpath+"/"+imglist.at(randnumb[5]);

            ui->graphicsView_8->scene()->clear();
            ui->graphicsView_8->scene()->addPixmap(pmarray[randnumb[6]]);
            currimglist[6]=folderpath+"/"+imglist.at(randnumb[6]);

            ui->graphicsView_9->scene()->clear();
            ui->graphicsView_9->scene()->addPixmap(pmarray[randnumb[7]]);
            currimglist[7]=folderpath+"/"+imglist.at(randnumb[7]);

            ui->graphicsView_10->scene()->clear();
            ui->graphicsView_10->scene()->addPixmap(pmarray[randnumb[8]]);
            currimglist[8]=folderpath+"/"+imglist.at(randnumb[8]);

            ui->graphicsView_11->scene()->clear();
            ui->graphicsView_11->scene()->addPixmap(pmarray[randnumb[9]]);
            currimglist[9]=folderpath+"/"+imglist.at(randnumb[9]);

            ui->graphicsView_12->scene()->clear();
            ui->graphicsView_12->scene()->addPixmap(pmarray[randnumb[10]]);
            currimglist[10]=folderpath+"/"+imglist.at(randnumb[10]);

            ui->graphicsView_13->scene()->clear();
            ui->graphicsView_13->scene()->addPixmap(pmarray[randnumb[11]]);
            currimglist[11]=folderpath+"/"+imglist.at(randnumb[11]);

            ui->graphicsView_14->scene()->clear();
            ui->graphicsView_14->scene()->addPixmap(pmarray[randnumb[12]]);
            currimglist[12]=folderpath+"/"+imglist.at(randnumb[12]);

            ui->graphicsView_15->scene()->clear();
            ui->graphicsView_15->scene()->addPixmap(pmarray[randnumb[13]]);
            currimglist[13]=folderpath+"/"+imglist.at(randnumb[13]);
        } else
        if ((!flowmode) && (!collectiveflow))
        {
            random_shuffle(randpuzzlelocs.begin(), randpuzzlelocs.end());

            ui->graphicsView_2->scene()->clear();
            ui->graphicsView_2->scene()->addPixmap(onepicarr[randpuzzlelocs[0]]);

            ui->graphicsView_3->scene()->clear();
            ui->graphicsView_3->scene()->addPixmap(onepicarr[randpuzzlelocs[1]]);

            ui->graphicsView_4->scene()->clear();
            ui->graphicsView_4->scene()->addPixmap(onepicarr[randpuzzlelocs[2]]);

            ui->graphicsView_5->scene()->clear();
            ui->graphicsView_5->scene()->addPixmap(onepicarr[randpuzzlelocs[3]]);

            ui->graphicsView_6->scene()->clear();
            ui->graphicsView_6->scene()->addPixmap(onepicarr[randpuzzlelocs[4]]);

            ui->graphicsView_7->scene()->clear();
            ui->graphicsView_7->scene()->addPixmap(onepicarr[randpuzzlelocs[5]]);

            ui->graphicsView_8->scene()->clear();
            ui->graphicsView_8->scene()->addPixmap(onepicarr[randpuzzlelocs[6]]);

            ui->graphicsView_9->scene()->clear();
            ui->graphicsView_9->scene()->addPixmap(onepicarr[randpuzzlelocs[7]]);

            ui->graphicsView_10->scene()->clear();
            ui->graphicsView_10->scene()->addPixmap(onepicarr[randpuzzlelocs[8]]);

            ui->graphicsView_11->scene()->clear();
            ui->graphicsView_11->scene()->addPixmap(onepicarr[randpuzzlelocs[9]]);

            ui->graphicsView_12->scene()->clear();
            ui->graphicsView_12->scene()->addPixmap(onepicarr[randpuzzlelocs[10]]);

            ui->graphicsView_13->scene()->clear();
            ui->graphicsView_13->scene()->addPixmap(onepicarr[randpuzzlelocs[11]]);

            ui->graphicsView_14->scene()->clear();
            ui->graphicsView_14->scene()->addPixmap(onepicarr[randpuzzlelocs[12]]);

            ui->graphicsView_15->scene()->clear();
            ui->graphicsView_15->scene()->addPixmap(onepicarr[randpuzzlelocs[13]]);

            scene->clear();
            scene->addPixmap(onepicarr[randpuzzlelocs[14]]);
        }
        if (flowmode)
        {
           // random_shuffle(randnumb.begin(), randnumb.end());

            if (collectiveflow)
            {
                ui->graphicsView_2->scene()->clear();
                ui->graphicsView_2->scene()->addPixmap(pmarray[randnumb[0]]);
                currimglist[0]=folderpath+"/"+imglist.at(randnumb[0]);

                ui->graphicsView_3->scene()->clear();
                ui->graphicsView_3->scene()->addPixmap(pmarray[randnumb[1]]);
                currimglist[1]=folderpath+"/"+imglist.at(randnumb[1]);

                ui->graphicsView_4->scene()->clear();
                ui->graphicsView_4->scene()->addPixmap(pmarray[randnumb[2]]);
                currimglist[2]=folderpath+"/"+imglist.at(randnumb[2]);

                ui->graphicsView_5->scene()->clear();
                ui->graphicsView_5->scene()->addPixmap(pmarray[randnumb[3]]);
                currimglist[3]=folderpath+"/"+imglist.at(randnumb[3]);

                ui->graphicsView_6->scene()->clear();
                ui->graphicsView_6->scene()->addPixmap(pmarray[randnumb[4]]);
                currimglist[4]=folderpath+"/"+imglist.at(randnumb[4]);

                ui->graphicsView_7->scene()->clear();
                ui->graphicsView_7->scene()->addPixmap(pmarray[randnumb[5]]);
                currimglist[5]=folderpath+"/"+imglist.at(randnumb[5]);

                ui->graphicsView_8->scene()->clear();
                ui->graphicsView_8->scene()->addPixmap(pmarray[randnumb[6]]);
                currimglist[6]=folderpath+"/"+imglist.at(randnumb[6]);

                ui->graphicsView_9->scene()->clear();
                ui->graphicsView_9->scene()->addPixmap(pmarray[randnumb[7]]);
                currimglist[7]=folderpath+"/"+imglist.at(randnumb[7]);

                ui->graphicsView_10->scene()->clear();
                ui->graphicsView_10->scene()->addPixmap(pmarray[randnumb[8]]);
                currimglist[8]=folderpath+"/"+imglist.at(randnumb[8]);

                ui->graphicsView_11->scene()->clear();
                ui->graphicsView_11->scene()->addPixmap(pmarray[randnumb[9]]);
                currimglist[9]=folderpath+"/"+imglist.at(randnumb[9]);

                ui->graphicsView_12->scene()->clear();
                ui->graphicsView_12->scene()->addPixmap(pmarray[randnumb[10]]);
                currimglist[10]=folderpath+"/"+imglist.at(randnumb[10]);

                ui->graphicsView_13->scene()->clear();
                ui->graphicsView_13->scene()->addPixmap(pmarray[randnumb[11]]);
                currimglist[11]=folderpath+"/"+imglist.at(randnumb[11]);

                ui->graphicsView_14->scene()->clear();
                ui->graphicsView_14->scene()->addPixmap(pmarray[randnumb[12]]);
                currimglist[12]=folderpath+"/"+imglist.at(randnumb[12]);

                ui->graphicsView_15->scene()->clear();
                ui->graphicsView_15->scene()->addPixmap(pmarray[randnumb[13]]);
                currimglist[13]=folderpath+"/"+imglist.at(randnumb[13]);

                scene->clear();
                scene->addPixmap(pmarray[randnumb[18]]);
            }

          //  matchpuzzle();
            scene->randr=qrand()%256;
            scene->randg=qrand()%256;
            scene->randb=qrand()%256;
            picsrestored = 15-picsforchange;
            stpic0.clear();
            for (int i=0; i<picsrestored; i++)
            {
                stpic0.insert(puzzlelocs[i]);
                //   int t = qrand() % 15;
                switch (puzzlelocs[i])
                {
                 case 0:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[0],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_6->scene()->addPixmap(pmtemp.scaled(300,200));//pmarray[randnumb[i]]);
                    } else
                    {
                        ui->graphicsView_6->scene()->clear();
                        ui->graphicsView_6->scene()->addPixmap(onepicarr[0]);
                    }
                    break;
                 }
                 case 1:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[1],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_7->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_7->scene()->clear();
                        ui->graphicsView_7->scene()->addPixmap(onepicarr[1]);
                    }
                    break;
                 }
                 case 2:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[2],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_8->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_8->scene()->clear();
                        ui->graphicsView_8->scene()->addPixmap(onepicarr[2]);
                    }
                    break;
                 }
                 case 3:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[3],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_9->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_9->scene()->clear();
                        ui->graphicsView_9->scene()->addPixmap(onepicarr[3]);
                    }
                    break;
                 }
                 case 4:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[4],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_10->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_10->scene()->clear();
                        ui->graphicsView_10->scene()->addPixmap(onepicarr[4]);
                    }
                    break;
                 }
                 case 5:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[5],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_3->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_3->scene()->clear();
                        ui->graphicsView_3->scene()->addPixmap(onepicarr[5]);
                    }
                    break;
                 }
                 case 6:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[6],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_2->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_2->scene()->clear();
                        ui->graphicsView_2->scene()->addPixmap(onepicarr[6]);
                    }
                    break;
                 }
                 case 7:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[7],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        scene->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        scene->clear();
                        scene->addPixmap(onepicarr[7]);
                    }
                    break;
                 }
                 case 8:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[8],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_4->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_4->scene()->clear();
                        ui->graphicsView_4->scene()->addPixmap(onepicarr[8]);
                    }
                    break;
                 }
                 case 9:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[9],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_5->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_5->scene()->clear();
                        ui->graphicsView_5->scene()->addPixmap(onepicarr[9]);
                    }
                    break;
                 }
                 case 10:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[10],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_11->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_11->scene()->clear();
                        ui->graphicsView_11->scene()->addPixmap(onepicarr[10]);
                    }
                    break;
                 }
                 case 11:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[11],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_12->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_12->scene()->clear();
                        ui->graphicsView_12->scene()->addPixmap(onepicarr[11]);
                    }
                    break;
                 }
                 case 12:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[12],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_13->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_13->scene()->clear();
                        ui->graphicsView_13->scene()->addPixmap(onepicarr[12]);
                    }
                    break;
                 }
                 case 13:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[13],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_14->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_14->scene()->clear();
                        ui->graphicsView_14->scene()->addPixmap(onepicarr[13]);
                    }
                    break;
                 }
                 case 14:
                 {
                    if (collectiveflow)
                    {
                        pmtemp = scene->filterpuzzle(onepicarr[14],scene->attentt,theta_arr[numsamples-1],beta_arr[numsamples-1],gamma_arr[numsamples-1],alpha_arr[numsamples-1]);
                        ui->graphicsView_15->scene()->addPixmap(pmtemp.scaled(300,200));
                    }
                    else
                    {
                        ui->graphicsView_15->scene()->clear();
                        ui->graphicsView_15->scene()->addPixmap(onepicarr[14]);
                    }
                    break;
                 }
                }
            }
            if (!collectiveflow)
            {
                stres.clear();
                set_difference(stpic1.begin(), stpic1.end(), stpic0.begin(), stpic0.end(),inserter(stres, stres.end()));
                setsize=stres.size();
                randpuzzlelocs.resize(setsize);
                iterst = stres.begin();
                for (int i=0; i<setsize; i++)
                {
                   randpuzzlelocs[i]=*iterst;
                   iterst++;
                }
                random_shuffle(randpuzzlelocs.begin(), randpuzzlelocs.end());

                sortpuzzlelocs.resize(setsize);
                for (int i=0; i<setsize; i++)
                    sortpuzzlelocs[i]=randpuzzlelocs[i];
                sort(sortpuzzlelocs.begin(),sortpuzzlelocs.end());

                for (int i=0; i<setsize; i++)
                {
                   switch (sortpuzzlelocs[i])
                   {
                   case 0:
                   {
                       ui->graphicsView_6->scene()->clear();
                       ui->graphicsView_6->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                       break;
                   }
                   case 1:
                   {
                       ui->graphicsView_7->scene()->clear();
                       ui->graphicsView_7->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                       break;
                   }
                   case 2:
                   {
                      ui->graphicsView_8->scene()->clear();
                      ui->graphicsView_8->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 3:
                   {
                      ui->graphicsView_9->scene()->clear();
                      ui->graphicsView_9->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 4:
                   {
                      ui->graphicsView_10->scene()->clear();
                      ui->graphicsView_10->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 5:
                   {
                      ui->graphicsView_3->scene()->clear();
                      ui->graphicsView_3->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 6:
                   {
                      ui->graphicsView_2->scene()->clear();
                      ui->graphicsView_2->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 7:
                   {
                      scene->clear();
                      scene->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 8:
                   {
                      ui->graphicsView_4->scene()->clear();
                      ui->graphicsView_4->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 9:
                   {
                      ui->graphicsView_5->scene()->clear();
                      ui->graphicsView_5->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 10:
                   {
                      ui->graphicsView_11->scene()->clear();
                      ui->graphicsView_11->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 11:
                   {
                      ui->graphicsView_12->scene()->clear();
                      ui->graphicsView_12->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 12:
                   {
                      ui->graphicsView_13->scene()->clear();
                      ui->graphicsView_13->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 13:
                   {
                      ui->graphicsView_14->scene()->clear();
                      ui->graphicsView_14->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   case 14:
                   {
                      ui->graphicsView_15->scene()->clear();
                      ui->graphicsView_15->scene()->addPixmap(onepicarr[randpuzzlelocs[i]]);
                      break;
                   }
                   }
                }
            }
        }
    }
    setloaded=true;
}

void paintform::matchpuzzle()
{
    ui->graphicsView_6->scene()->addPixmap(onepicarr[0]);
   // ui->graphicsView_6->repaint();
    ui->graphicsView_7->scene()->addPixmap(onepicarr[1]);
  //  ui->graphicsView_7->repaint();
    ui->graphicsView_8->scene()->addPixmap(onepicarr[2]);
   // ui->graphicsView_8->repaint();
    ui->graphicsView_9->scene()->addPixmap(onepicarr[3]);
   // ui->graphicsView_9->repaint();
    ui->graphicsView_10->scene()->addPixmap(onepicarr[4]);
  //  ui->graphicsView_10->repaint();

    ui->graphicsView_3->scene()->addPixmap(onepicarr[5]);
  //  ui->graphicsView_3->repaint();
    ui->graphicsView_2->scene()->addPixmap(onepicarr[6]);
   // ui->graphicsView_2->repaint();
    scene->addPixmap(onepicarr[7]);
  //  scene->paintf->repaint();
    ui->graphicsView_4->scene()->addPixmap(onepicarr[8]);
  //  ui->graphicsView_4->repaint();
    ui->graphicsView_5->scene()->addPixmap(onepicarr[9]);
  //  ui->graphicsView_5->repaint();

    ui->graphicsView_11->scene()->addPixmap(onepicarr[10]);
  //  ui->graphicsView_11->repaint();
    ui->graphicsView_12->scene()->addPixmap(onepicarr[11]);
  //  ui->graphicsView_12->repaint();
    ui->graphicsView_13->scene()->addPixmap(onepicarr[12]);
  //  ui->graphicsView_13->repaint();
    ui->graphicsView_14->scene()->addPixmap(onepicarr[13]);
  //  ui->graphicsView_14->repaint();
    ui->graphicsView_15->scene()->addPixmap(onepicarr[14]);
   // ui->graphicsView_15->repaint();
//    if (puzzlemode)
 //   {
  //      scene->addPixmap(onepicarr[14]);
}

void paintform::resizeEvent(QResizeEvent *)
{
    fitView();
}

void paintform::showEvent(QShowEvent *)
{
    fitView();
}

bool paintform::eventFilter(QObject *target, QEvent *event)
{
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

    if ((target == ui->graphicsView) && (event->type() == QEvent::KeyPress))
    {
        if (keyEvent->key() == Qt::Key_Z)
        {
            if (eegsize>1)
                eegsize--;
            ui->spinBox->setValue(eegsize);
        }
        if (keyEvent->key() == Qt::Key_X)
        {
            if (eegsize<12)
                eegsize++;
            ui->spinBox->setValue(eegsize);
        }
        if (keyEvent->key() == Qt::Key_A)
        {
            if (pensize>1)
                pensize--;
            ui->spinBox_2->setValue(pensize);
        }
        if (keyEvent->key() == Qt::Key_S)
        {
            if (pensize<50)
                pensize++;
            ui->spinBox_2->setValue(pensize);
        }

        if (keyEvent->key() == Qt::Key_L)
        {
            showestatt=!showestatt;
            if (showestatt)
            {
                ui->widget_2->yAxis->setRange(0,100);
                ui->widget_2->graph(6)->setVisible(true);
            } else
            {
                ui->widget_2->yAxis->setRange(0,50);
                ui->widget_2->graph(6)->setVisible(false);
            }
        }

        if (keyEvent->key() == Qt::Key_O)
            on_pushButton_clicked();

        if (keyEvent->key() == Qt::Key_P)
        {
            ui->checkBox_12->setChecked(!ui->checkBox_12->isChecked());
            ui->widget_2->setVisible(ui->checkBox_12->isChecked());
            ui->widget->setVisible(ui->checkBox_12->isChecked());
        }

        if (keyEvent->key() == Qt::Key_Tab)
        {
            this->hide();
            pw->show();
            pw->setFocus();
        }      

        pressedKeys += ((QKeyEvent*)event)->key();

        if ( (keyEvent->key() == Qt::Key_Q) && (!scene->horizline) && (!scene->vertline) )
        {
            scene->horizline=true;
            ui->checkBox_4->setChecked(true);
        }
        else
        if ( (keyEvent->key() == Qt::Key_Q) && (scene->horizline) )
        {
            scene->horizline=false;
            ui->checkBox_4->setChecked(false);
        }

        if ( (keyEvent->key() == Qt::Key_W) && (!scene->vertline) && (!scene->horizline) )
        {
            scene->vertline=true;
            ui->checkBox_5->setChecked(true);
        }
        else
        if ( (keyEvent->key() == Qt::Key_W) && (scene->vertline) )
        {
            scene->vertline=false;
            ui->checkBox_5->setChecked(false);
        }
    }

  //  if (((puzzlemode) && (!gamemode) && (target == ui->graphicsView) && (event->type() == QEvent::MouseButtonDblClick)))
  //  {
  //      on_checkBox_8_clicked();
   // }

    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseButtonDblClick))
    {
       if ((!gamemode) && (!flowmode) && (!scene->drawflow) && (minimode))
       {
            if (!puzzlemode)
                pmg = ui->graphicsView->grab();
            else
                pmg = mainpic;

            on_checkBox_8_clicked();
            ui->checkBox_8->setChecked(puzzlemode);
            scene->addPixmap(pmg.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->update();
            ui->graphicsView->repaint();
       }
    }

    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::RightButton)
        {
            if ((scene->randfixcolor))
                scene->randfxcl=QColor(qrand()%256,qrand()%256,qrand()%256,255-scene->attentt*2);
        }
        if (mouseEvent->button() == Qt::MiddleButton)
        {
            if (!erasepen)
                temppensize=pensize;
            else
            {
                pensize=temppensize;
                ui->spinBox_2->setValue(pensize);
            }
            erasepen=!erasepen;
            ui->checkBox_3->setChecked(erasepen);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[14]==prevpict) && (prevpuzzle!=1))
                startround();
            else
            {
                prevpict = currentindexes[14];
                prevpuzzle = 1;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_2) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode) && (!flowmode))
        {
            pmain.load(currimglist[0]);
            qim.load(currimglist[0]);
            mainpic.load(currimglist[0]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[0]=t;
            currimglist[0]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_2->scene()->clear();
            ui->graphicsView_2->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode) && (!flowmode))
        {
            int t = qrand() % imglist.length();
            currentindexes[0]=t;
            if (minimode)
                ui->graphicsView_2->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_2->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[0]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[0]==prevpict) && (prevpuzzle!=2))
                startround();
            else
            {
                prevpict = currentindexes[0];
                prevpuzzle = 2;
            }
        }
    }

  //  pmain.load(currimglist[1]);
 //   qim.load(currimglist[1]);
 //   qimload=true;
 //   scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
  //  scene->paintf->repaint();

    if ((setloaded) && (target == ui->graphicsView_3) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[1]);
            qim.load(currimglist[1]);
            mainpic.load(currimglist[1]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[1]=t;
            currimglist[1]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_3->scene()->clear();
            ui->graphicsView_3->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[1]=t;
            if (minimode)
                ui->graphicsView_3->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_3->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[1]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[1]==prevpict) && (prevpuzzle!=3))
                startround();
            else
            {
                prevpict = currentindexes[1];
                prevpuzzle = 3;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_4) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[2]);
            qim.load(currimglist[2]);
            mainpic.load(currimglist[2]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[2]=t;
            currimglist[2]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_4->scene()->clear();
            ui->graphicsView_4->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[2]=t;
            if (minimode)
                ui->graphicsView_4->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_4->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[2]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[2]==prevpict) && (prevpuzzle!=4))
                startround();
            else
            {
                prevpict = currentindexes[2];
                prevpuzzle = 4;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_5) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[3]);
            qim.load(currimglist[3]);
            mainpic.load(currimglist[3]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[3]=t;
            currimglist[3]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_5->scene()->clear();
            ui->graphicsView_5->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[3]=t;
            if (minimode)
                ui->graphicsView_5->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_5->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[3]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[3]==prevpict) && (prevpuzzle!=5))
                startround();
            else
            {
                prevpict = currentindexes[3];
                prevpuzzle = 5;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_6) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[4]);
            qim.load(currimglist[4]);
            mainpic.load(currimglist[4]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[4]=t;
            currimglist[4]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_6->scene()->clear();
            ui->graphicsView_6->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[4]=t;
            if (minimode)
                ui->graphicsView_6->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_6->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[4]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[4]==prevpict) && (prevpuzzle!=6))
                startround();
            else
            {
                prevpict = currentindexes[4];
                prevpuzzle = 6;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_7) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[5]);
            qim.load(currimglist[5]);
            mainpic.load(currimglist[5]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[5]=t;
            currimglist[5]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_7->scene()->clear();
            ui->graphicsView_7->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[5]=t;
            if (minimode)
                ui->graphicsView_7->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_7->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[5]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[5]==prevpict) && (prevpuzzle!=7))
                startround();
            else
            {
                prevpict = currentindexes[5];
                prevpuzzle = 7;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_8) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[6]);
            qim.load(currimglist[6]);
            mainpic.load(currimglist[6]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[6]=t;
            currimglist[6]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_8->scene()->clear();
            ui->graphicsView_8->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[6]=t;
            if (minimode)
                ui->graphicsView_8->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_8->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[6]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[6]==prevpict) && (prevpuzzle!=8))
                startround();
            else
            {
                prevpict = currentindexes[6];
                prevpuzzle = 8;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_9) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[7]);
            qim.load(currimglist[7]);
            mainpic.load(currimglist[7]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[7]=t;
            currimglist[7]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_9->scene()->clear();
            ui->graphicsView_9->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[7]=t;
            if (minimode)
                ui->graphicsView_9->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_9->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[7]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[7]==prevpict) && (prevpuzzle!=9))
                startround();
            else
            {
                prevpict = currentindexes[7];
                prevpuzzle = 9;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_10) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[8]);
            qim.load(currimglist[8]);
            mainpic.load(currimglist[8]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[8]=t;
            currimglist[8]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_10->scene()->clear();
            ui->graphicsView_10->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[8]=t;
            if (minimode)
                ui->graphicsView_10->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_10->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[8]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[8]==prevpict) && (prevpuzzle!=10))
                startround();
            else
            {
                prevpict = currentindexes[8];
                prevpuzzle = 10;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_11) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[9]);
            qim.load(currimglist[9]);
            mainpic.load(currimglist[9]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[9]=t;
            currimglist[9]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_11->scene()->clear();
            ui->graphicsView_11->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[9]=t;
            if (minimode)
                ui->graphicsView_11->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_11->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[9]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[9]==prevpict) && (prevpuzzle!=11))
                startround();
            else
            {
                prevpict = currentindexes[9];
                prevpuzzle = 11;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_12) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[10]);
            qim.load(currimglist[10]);
            mainpic.load(currimglist[10]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[10]=t;
            currimglist[10]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_12->scene()->clear();
            ui->graphicsView_12->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[10]=t;
            if (minimode)
                ui->graphicsView_12->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_12->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[10]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[10]==prevpict) && (prevpuzzle!=12))
                startround();
            else
            {
                prevpict = currentindexes[10];
                prevpuzzle = 12;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_13) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[11]);
            qim.load(currimglist[11]);
            mainpic.load(currimglist[11]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[11]=t;
            currimglist[11]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_13->scene()->clear();
            ui->graphicsView_13->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[11]=t;
            if (minimode)
                ui->graphicsView_13->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_13->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[11]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[11]==prevpict) && (prevpuzzle!=13))
                startround();
            else
            {
                prevpict = currentindexes[11];
                prevpuzzle = 13;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_14) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[12]);
            qim.load(currimglist[12]);
            mainpic.load(currimglist[12]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[12]=t;
            currimglist[12]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_14->scene()->clear();
            ui->graphicsView_14->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[12]=t;
            if (minimode)
                ui->graphicsView_14->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_14->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[12]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[12]==prevpict) && (prevpuzzle!=14))
                startround();
            else
            {
                prevpict = currentindexes[12];
                prevpuzzle = 14;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_15) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!gamemode))
        {
            pmain.load(currimglist[13]);
            qim.load(currimglist[13]);
            mainpic.load(currimglist[13]);
            qimload=true;
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->paintf->repaint();
            int t = qrand() % imglist.length();
            currentindexes[13]=t;
            currimglist[13]=folderpath+"/"+imglist.at(t);
            ui->graphicsView_15->scene()->clear();
            ui->graphicsView_15->scene()->addPixmap(pmarray[t]);
        }
        if ((mouseEvent->button() == Qt::RightButton) && (!gamemode))
        {
            int t = qrand() % imglist.length();
            currentindexes[13]=t;
            if (minimode)
                ui->graphicsView_15->scene()->addPixmap(pmarray[t]);
            else
            {
                pm.load(folderpath+"/"+imglist.at(t));
                ui->graphicsView_15->scene()->addPixmap(pm.scaled(puzzlew,puzzleh,rationmode,Qt::SmoothTransformation));
            }
            currimglist[13]=folderpath+"/"+imglist.at(t);
        }
        if ((mouseEvent->button() == Qt::LeftButton) && (gamemode))
        {
            if ((currentindexes[13]==prevpict) && (prevpuzzle!=15))
                startround();
            else
            {
                prevpict = currentindexes[13];
                prevpuzzle = 15;
            }
        }
    }

    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseMove))
    {
      //  qDebug()<<scene->pointnum;
      //  QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      //  if ((scene->drawbpoints) && (scene->startedline))
      //  {
      //      scene->linecoords[ scene->pointnum].setX(mouseEvent->pos().x());
      //      scene->linecoords[ scene->pointnum].setY(mouseEvent->pos().y());
     //       scene->pointnum++;
     //       qDebug()<<scene->pointnum;
      //  }
    }

    if ((!puzzlemode) && (qimload) && (target == ui->graphicsView) && (event->type() == QEvent::MouseButtonDblClick))
    {
      //  on_pushButton_9_clicked();
    }

    return false;
}

void paintform::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

     if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        if (erasepen)
        {
            if ((numSteps.ry()==1) && (pensize<29))
                pensize+=1;
            else if ((numSteps.ry()==-1) && (pensize>1))
                pensize+=-1;
        } else
        {
            if ((numSteps.ry()==1) && (eegsize<11))
                eegsize+=1;
            else if ((numSteps.ry()==-1) && (eegsize>1))
                eegsize+=-1;
        }
        ui->spinBox->setValue(eegsize);
        ui->spinBox_2->setValue(pensize);
    }
     event->accept();
}

void paintform::updateplots(bool fl)
{
    if (fl)
    {
        ui->widget_2->setVisible(true);
        ui->widget->setVisible(true);
    } else
    {
        ui->widget_2->setVisible(false);
        ui->widget->setVisible(false);
    }
}

QImage paintform::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent)
{
    if(src.isNull()) return QImage();   //No need to do anything else!
    if(!effect) return src;             //No need to do anything else!
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
    return res;
}

void paintform::filtering_puzzle(QGraphicsView* gv, QPixmap pm, int grade)
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    if (attentmodu)
        blur->setBlurRadius((100-pw->attent)/grade);
    else
        blur->setBlurRadius((100-pw->meditt)/grade);
    QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
    if (attentmodu)
        colorize->setStrength((double)pw->attent/70);
    else
        colorize->setStrength((double)pw->meditt/70);
    colorize->setColor(QColor(qrand()%255,qrand()%255,qrand()%256,qrand()%255));

    QImage qbim1 = applyEffectToImage(pm.toImage(), blur, 0);
    //QImage qbim2 = applyEffectToImage(qbim1, colorize, 0);
    gv->scene()->clear();
    gv->scene()->addPixmap(QPixmap::fromImage(qbim1));
    gv->repaint();
}

void paintform::filteringmain_ingame(int grade)
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    if (attentmodu)
        blur->setBlurRadius((100-pw->attent)/grade);
    else
        blur->setBlurRadius((100-pw->meditt)/grade);
    QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
    if (attentmodu)
        colorize->setStrength((double)pw->attent/70);
    else
        colorize->setStrength((double)pw->meditt/70);
    colorize->setColor(QColor(qrand()%255,qrand()%255,qrand()%256,qrand()%255));

    QImage qbim1;
    if (puzzlemode)
        qbim1 = applyEffectToImage(pmarray[mainindex].toImage(), blur, 0);
    else
        qbim1 = applyEffectToImage(qim, blur, 0);
   // QImage qbim2 = applyEffectToImage(qbim1, colorize, 0);

    pmain = QPixmap::fromImage(qbim1);

    scene->clear();
    if (puzzlemode)
        scene->addPixmap(pmain.scaled(300,200,rationmode,Qt::SmoothTransformation));
    else
        scene->addPixmap(pmain.scaled(1500,800,rationmode,Qt::SmoothTransformation));

}

void paintform::filtering_allpuzzles(int grade)
{
    if ((minimode) && (puzzlemode))
    {
        filtering_puzzle(ui->graphicsView_2,pmarray[currentindexes[0]],grade);
        filtering_puzzle(ui->graphicsView_3,pmarray[currentindexes[1]],grade);
        filtering_puzzle(ui->graphicsView_4,pmarray[currentindexes[2]],grade);
        filtering_puzzle(ui->graphicsView_5,pmarray[currentindexes[3]],grade);
        filtering_puzzle(ui->graphicsView_6,pmarray[currentindexes[4]],grade);
        filtering_puzzle(ui->graphicsView_7,pmarray[currentindexes[5]],grade);
        filtering_puzzle(ui->graphicsView_8,pmarray[currentindexes[6]],grade);
        filtering_puzzle(ui->graphicsView_9,pmarray[currentindexes[7]],grade);
        filtering_puzzle(ui->graphicsView_10,pmarray[currentindexes[8]],grade);
        filtering_puzzle(ui->graphicsView_11,pmarray[currentindexes[9]],grade);
        filtering_puzzle(ui->graphicsView_12,pmarray[currentindexes[10]],grade);
        filtering_puzzle(ui->graphicsView_13,pmarray[currentindexes[11]],grade);
        filtering_puzzle(ui->graphicsView_14,pmarray[currentindexes[12]],grade);
        filtering_puzzle(ui->graphicsView_15,pmarray[currentindexes[13]],grade);
    }
}

void paintform::startround()
{
    randompics();

    QString filename=folderpath+"/"+imglist.at(randnumb[14]);
    currimglist[14]=filename;
    mainindex = randnumb[14];
    pmain.load(filename);
    qim.load(filename);
    mainpic.load(filename);
    qimload = true;
    scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));

    vector<int> randnums = vector<int>(15);
    iota(randnums.begin(), randnums.end(), 0);
    random_shuffle(randnums.begin(), randnums.end());

    int picn1 = randnums[0];
    int picn2 = randnums[1];        

    int puzzlepic1 = randnumb[16];

    if ((picn1==14) || (picn2==14))
    {
        mainindex=randnumb[16];
        filename=folderpath+"/"+imglist.at(randnumb[16]);
        currimglist[14]=filename;
        pmain.load(filename);
        mainpic.load(filename);
        qim.load(filename);
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
    }


    switch (picn1)
    {
        case 0:
            ui->graphicsView_2->scene()->clear();
            ui->graphicsView_2->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[0]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[0]=puzzlepic1;
            break;
        case 1:
            ui->graphicsView_3->scene()->clear();
            ui->graphicsView_3->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[1]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[1]=puzzlepic1;
            break;
        case 2:
            ui->graphicsView_4->scene()->clear();
            ui->graphicsView_4->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[2]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[2]=puzzlepic1;
            break;
        case 3:
            ui->graphicsView_5->scene()->clear();
            ui->graphicsView_5->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[3]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[3]=puzzlepic1;
            break;
        case 4:
            ui->graphicsView_6->scene()->clear();
            ui->graphicsView_6->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[4]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[4]=puzzlepic1;
            break;
        case 5:
            ui->graphicsView_7->scene()->clear();
            ui->graphicsView_7->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[5]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[5]=puzzlepic1;
            break;
        case 6:
            ui->graphicsView_8->scene()->clear();
            ui->graphicsView_8->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[6]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[6]=puzzlepic1;
            break;
        case 7:
            ui->graphicsView_9->scene()->clear();
            ui->graphicsView_9->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[7]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[7]=puzzlepic1;
            break;
        case 8:
            ui->graphicsView_10->scene()->clear();
            ui->graphicsView_10->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[8]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[8]=puzzlepic1;
            break;
        case 9:
            ui->graphicsView_11->scene()->clear();
            ui->graphicsView_11->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[9]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[9]=puzzlepic1;
            break;
        case 10:
            ui->graphicsView_12->scene()->clear();
            ui->graphicsView_12->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[10]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[10]=puzzlepic1;
            break;
        case 11:
            ui->graphicsView_13->scene()->clear();
            ui->graphicsView_13->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[11]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[11]=puzzlepic1;
            break;
        case 12:
            ui->graphicsView_14->scene()->clear();
            ui->graphicsView_14->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[12]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[12]=puzzlepic1;
            break;
        case 13:
            ui->graphicsView_15->scene()->clear();
            ui->graphicsView_15->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[13]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[13]=puzzlepic1;
            break;
        case 14:
            scene->clear();
            scene->addPixmap(pmarray[puzzlepic1]);
            currimglist[14]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[14]=puzzlepic1;
            break;
    }

    switch (picn2)
    {
        case 0:
            ui->graphicsView_2->scene()->clear();
            ui->graphicsView_2->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[0]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[0]=puzzlepic1;
            break;
        case 1:
            ui->graphicsView_3->scene()->clear();
            ui->graphicsView_3->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[1]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[1]=puzzlepic1;
            break;
        case 2:
            ui->graphicsView_4->scene()->clear();
            ui->graphicsView_4->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[2]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[2]=puzzlepic1;
            break;
        case 3:
            ui->graphicsView_5->scene()->clear();
            ui->graphicsView_5->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[3]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[3]=puzzlepic1;
            break;
        case 4:
            ui->graphicsView_6->scene()->clear();
            ui->graphicsView_6->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[4]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[4]=puzzlepic1;
            break;
        case 5:
            ui->graphicsView_7->scene()->clear();
            ui->graphicsView_7->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[5]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[5]=puzzlepic1;
            break;
        case 6:
            ui->graphicsView_8->scene()->clear();
            ui->graphicsView_8->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[6]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[6]=puzzlepic1;
            break;
        case 7:
            ui->graphicsView_9->scene()->clear();
            ui->graphicsView_9->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[7]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[7]=puzzlepic1;
            break;
        case 8:
            ui->graphicsView_10->scene()->clear();
            ui->graphicsView_10->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[8]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[8]=puzzlepic1;
            break;
        case 9:
            ui->graphicsView_11->scene()->clear();
            ui->graphicsView_11->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[9]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[9]=puzzlepic1;
            break;
        case 10:
            ui->graphicsView_12->scene()->clear();
            ui->graphicsView_12->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[10]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[10]=puzzlepic1;
            break;
        case 11:
            ui->graphicsView_13->scene()->clear();
            ui->graphicsView_13->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[11]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[11]=puzzlepic1;
            break;
        case 12:
            ui->graphicsView_14->scene()->clear();
            ui->graphicsView_14->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[12]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[12]=puzzlepic1;
            break;
        case 13:
            ui->graphicsView_15->scene()->clear();
            ui->graphicsView_15->scene()->addPixmap(pmarray[puzzlepic1]);
            currimglist[13]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[13]=puzzlepic1;
            break;
        case 14:
            scene->clear();
            scene->addPixmap(pmarray[puzzlepic1]);
            currimglist[14]=folderpath+"/"+imglist.at(puzzlepic1);
            currentindexes[14]=puzzlepic1;
            break;
    }
    pw->tonenumbers=0;
    pw->tones=" ";
    pw->randomtone();
    pw->letsplay();
}

void paintform::setbackimageocv(QString filename)
{
    mainpic.load(filename);
    pmain.load(filename);
    qim.load(filename);
    currimglist[14]=filename;
    backloaded = true;
    if (scene->drawflow)
    {
        scene->bkgndimg.load(filename);
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
        scene->clear();
        this->repaint();
    } else
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));

}

void paintform::setbackimage(QPixmap pm)
{
    pmain=pm;
    qim=pm.toImage();    
    backloaded = true;
    if (!scene->drawflow)
    {
        scene->clear();
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
    }
    else
    {
        scene->bkgndimg=pm;
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
    }
}

void paintform::fitView()
{
  //  QRectF rect = QRectF(1,1, ui->graphicsView->width()-2, ui->graphicsView->height()-2); // 0,0,-2,-2
  //  ui->graphicsView->fitInView(rect, rationmode);
 //   ui->graphicsView->setSceneRect(rect);
   // qDebug()<<scene->itemsBoundingRect().width();
}


void paintform::on_checkBox_clicked()
{
    if (!ui->checkBox->isChecked())
        scene->randcolor=false;
    else
        scene->randcolor=true;
}

void paintform::grabpuzzles()
{
  //  if ((!puzzlemode) && (!pmain.isNull()))
    {
        onepicarr[0] = ui->graphicsView->grab(QRect(0,0,300,200));
        onepicarr[1] = ui->graphicsView->grab(QRect(300,0,300,200));
        onepicarr[2] = ui->graphicsView->grab(QRect(600,0,300,200));
        onepicarr[3] = ui->graphicsView->grab(QRect(900,0,300,200));
        onepicarr[4] = ui->graphicsView->grab(QRect(1200,0,300,200));
        onepicarr[5] = ui->graphicsView->grab(QRect(0,200,300,200));
        onepicarr[6] = ui->graphicsView->grab(QRect(300,200,300,200));
        onepicarr[7] = ui->graphicsView->grab(QRect(600,200,300,200));
        onepicarr[8] = ui->graphicsView->grab(QRect(900,200,300,200));
        onepicarr[9] = ui->graphicsView->grab(QRect(1200,200,300,200));
        onepicarr[10] = ui->graphicsView->grab(QRect(0,400,300,200));
        onepicarr[11] = ui->graphicsView->grab(QRect(300,400,300,200));
        onepicarr[12] = ui->graphicsView->grab(QRect(600,400,300,200));
        onepicarr[13] = ui->graphicsView->grab(QRect(900,400,300,200));
        onepicarr[14] = ui->graphicsView->grab(QRect(1200,400,300,200));
        puzzlegrabed=true;
    }
    //    if (puzzlemode)
     //   {
      //      scene->addPixmap(onepicarr[14]);
      //      scene->paintf->repaint();
     //   }
}

void paintform::on_pushButton_clicked()
{
    if (!pmain.isNull())
    {
     //   scene->addPixmap(pm.scaledToWidth(ui->graphicsView->width()));
      //  scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        if (!scene->drawflow)
        {            
            pmain = mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation);
            scene->addPixmap(mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->update();            
        }
        else
        {
            scene->clear();
            this->setPalette(qpr);
            this->repaint();
        }
    }
    else
        scene->clear();

  //  for (int i=0; i<50; i++)
  //  {
  //      centercoord[i][0]=qrand()%1500;
  //      centercoord[i][1]=qrand()%800;
  //      poltypearr[i]=3+qrand()%6;
  //  }
}

int paintform::getgraphicview_width()
{
    return ui->graphicsView->width();
}

int paintform::getgraphicview_height()
{
    return ui->graphicsView->height();
}

void paintform::on_pushButton_2_clicked()
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),"D://","All files (*.*)");
    if (filename!="")
    {
        mainpic.load(filename);
        pmain.load(filename);
        qim.load(filename);
        currimglist[14]=filename;
        backloaded = true;
        if (scene->drawflow)
        {
            scene->bkgndimg.load(filename);
            qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
            this->setPalette(qpr);
            scene->clear();
            this->repaint();
        } else
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));

       // if (puzzlemode)
       //     randompics();
    }
}

void paintform::on_spinBox_valueChanged(int arg1)
{
    eegsize=arg1;
}

void paintform::on_spinBox_2_valueChanged(int arg1)
{
    pensize=arg1;
}

void paintform::delay(int temp)
{
    QTime dieTime = QTime::currentTime().addMSecs(temp);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void paintform::on_pushButton_3_clicked()
{
    scene->fxcolor = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

    //scene->clearlast();
    //scene->drawline();
   // int t=0;
  //  while (t<200)
   // {
   //     QCursor::setPos(500, 100 + t);
    //    delay(10);
   //     t++;
   // }
}

void paintform::getimg1()
{
    prevpic=ui->graphicsView->grab();
}

void paintform::getimg2()
{
    curpic=ui->graphicsView->grab();
    if (!pmain.isNull())
        pmain=curpic;
}

void paintform::on_pushButton_4_clicked()
{
    QString fileName=QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "BMP Files (*.bmp);;JPEG (*.JPEG);;PNG (*.png)" );
    if (!fileName.isNull())
    {
        QPixmap pixMap = this->ui->graphicsView->grab();
        pixMap.save(fileName);
    }
}

void paintform::on_checkBox_2_clicked()
{
    if (!ui->checkBox_2->isChecked())
        scene->drawbpoints=false;
    else
        scene->drawbpoints=true;
}

void paintform::on_radioButton_clicked()
{
    scene->randcolor=true;
    scene->fixcolor=false;
    scene->freqcolor=false;
    scene->randfixcolor=false;
}

void paintform::on_radioButton_2_clicked()
{
    scene->randcolor=false;
    scene->fixcolor=true;
    scene->freqcolor=false;
    scene->randfixcolor=false;
}

void paintform::on_radioButton_3_clicked()
{
    scene->randcolor=false;
    scene->fixcolor=false;
    scene->freqcolor=true;
    scene->randfixcolor=false;
}

void paintform::on_pushButton_5_clicked()
{
    if (!prevpic.isNull())
        scene->addPixmap(prevpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
}

void paintform::on_checkBox_3_clicked()
{
    if (!ui->checkBox_3->isChecked())
        erasepen=false;
    else
        erasepen=true;
}

void paintform::on_checkBox_4_clicked()
{
    if (!ui->checkBox_5->isChecked())
    {
        if (!ui->checkBox_4->isChecked())
            scene->horizline=false;
        else
            scene->horizline=true;
    } else
    {
        ui->checkBox_5->setChecked(false);
        scene->vertline=false;
        if (!ui->checkBox_4->isChecked())
            scene->horizline=false;
        else
            scene->horizline=true;
    }
}

void paintform::on_checkBox_5_clicked()
{
    if (!ui->checkBox_4->isChecked())
    {
        if (!ui->checkBox_5->isChecked())
            scene->vertline=false;
        else
            scene->vertline=true;
    } else
    {
        ui->checkBox_4->setChecked(false);
        scene->horizline=false;
        if (!ui->checkBox_5->isChecked())
            scene->vertline=false;
        else
            scene->vertline=true;
    }
}

void paintform::on_checkBox_6_clicked()
{
    bfiltmode=!bfiltmode;
    pw->bfiltmode=bfiltmode;
}

void paintform::on_pushButton_6_clicked()
{
   /* if (ui->checkBox_12->isChecked())
    {
        ui->widget_2->setVisible(false);
        ui->widget->setVisible(false);
    } */
    int rimg = qrand() % imglist.length();
    QString filename=folderpath+"/"+imglist.at(rimg);
    mainpic.load(filename);
    currimglist[14]=filename;
    mainindex = rimg;
    scene->bkgndimg.load(filename);
    pmain.load(filename);
    qim.load(filename);
    backloaded = true;

    if (scene->drawflow)
    {
       // scene->clear();
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
    } else
    {
        qim=qim.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation);
      //  this->repaint();
      //  scene->clear();
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
    }
        //qDebug()<<"image loaded";
     //   scene->paintf->repaint();

  /*  if (ui->checkBox_12->isChecked())
    {
        ui->widget_2->setVisible(true);
        ui->widget->setVisible(true);
    } */
}

void paintform::on_checkBox_7_clicked()
{
    scene->attmodul=!scene->attmodul;

}

void paintform::on_checkBox_8_clicked()
{
    //if (flowmode)
        puzzlemode=!puzzlemode;
   // if ((puzzlemode) && (!flowmode))
   //     ui->checkBox_16->setEnabled(false);
   // else
   //     ui->checkBox_16->setEnabled(true);

    ui->checkBox_10->setEnabled(puzzlemode);
    ui->checkBox_20->setEnabled(!puzzlemode);
    ui->pushButton_7->setEnabled(puzzlemode);

    if (puzzlemode)
    {
        if (flowmode)
        {
            scene->clear();
            scene->addPixmap(onepicarr[7]);
        }            
        ui->graphicsView->setGeometry(650,250,300,200);
    }
    else
        ui->graphicsView->setGeometry(50,50,1500,800);
    if ((qimload) && (!flowmode))
        scene->addPixmap(mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
    ui->graphicsView->repaint();
    ui->graphicsView_2->setVisible(puzzlemode);
    ui->graphicsView_3->setVisible(puzzlemode);
    ui->graphicsView_4->setVisible(puzzlemode);
    ui->graphicsView_5->setVisible(puzzlemode);
    ui->graphicsView_6->setVisible(puzzlemode);
    ui->graphicsView_7->setVisible(puzzlemode);
    ui->graphicsView_8->setVisible(puzzlemode);
    ui->graphicsView_9->setVisible(puzzlemode);
    ui->graphicsView_10->setVisible(puzzlemode);
    ui->graphicsView_11->setVisible(puzzlemode);
    ui->graphicsView_12->setVisible(puzzlemode);
    ui->graphicsView_13->setVisible(puzzlemode);
    ui->graphicsView_14->setVisible(puzzlemode);
    ui->graphicsView_15->setVisible(puzzlemode);

    if (minimode)
        ui->checkBox_13->setEnabled(puzzlemode);

    if (firstpuzzle)
    {
        randompics();
        if (!backloaded)
            on_pushButton_6_clicked();
        firstpuzzle=false;
    }
}

void paintform::on_pushButton_7_clicked()
{
    randompics();
}

void paintform::on_checkBox_9_clicked()
{
    fixedmain=!fixedmain;
}

void paintform::tpicschangeUpdate()
{
    randompics();
}

void paintform::on_spinBox_3_valueChanged(int arg1)
{
    if (puzzlemode)
    {
        timepics=arg1;
        tpicschange->setInterval(timepics);
    }
}

void paintform::on_checkBox_10_clicked()
{
    changingpics=!changingpics;
    ui->checkBox_13->setEnabled(!changingpics);
    if (minimode)
        ui->checkBox_11->setEnabled(changingpics);
    if (changingpics)
        tpicschange->start();
    else
        tpicschange->stop();
}

void paintform::on_pushButton_8_clicked()
{
    fd.setPath(folderpath);
    fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    pmarray.clear();
    pmarray.resize(imglist.length());
    randnumb = vector<int>(imglist.length());
    iota(randnumb.begin(), randnumb.end(), 0);
    random_shuffle(randnumb.begin(), randnumb.end());
    initpics();
//    ui->pushButton_8->setEnabled(false);
    ui->spinBox_3->setMinimum(100);
    if (puzzlemode)
        ui->checkBox_13->setEnabled(true);
    ui->checkBox_16->setEnabled(true);
    ui->checkBox_17->setEnabled(true);
    ui->checkBox_8->setEnabled(true);
}

void paintform::on_radioButton_4_clicked()
{
    ui->radioButton_5->setStyleSheet("QRadioButton { color : black; }");
    ui->label_24->setVisible(false);
    ui->label_23->setVisible(true);
    pw->attentmodul=true;
    attentmodu=true;
 }

void paintform::on_radioButton_5_clicked()
{
    ui->radioButton_4->setStyleSheet("QRadioButton { color : black; }");
    ui->label_23->setVisible(false);
    ui->label_24->setVisible(true);
    pw->attentmodul=false;
    attentmodu=false;
}

void paintform::on_checkBox_11_clicked()
{
    limitpicschange=!limitpicschange;
}

void paintform::on_spinBox_4_valueChanged(int arg1)
{
    picsforchange=arg1;
}

void paintform::on_comboBox_currentIndexChanged(int index)
{
    if (index==0)
        rationmode=Qt::IgnoreAspectRatio;
    else if (index==1)
        rationmode=Qt::KeepAspectRatio;
    else if (index==2)
        rationmode=Qt::KeepAspectRatioByExpanding;
}

void paintform::on_checkBox_12_clicked()
{
     ui->widget_2->setVisible(ui->checkBox_12->isChecked());
     ui->widget->setVisible(ui->checkBox_12->isChecked());
}

void paintform::on_checkBox_13_clicked()
{
    gamemode=!gamemode;
    ui->checkBox_10->setEnabled(!gamemode);
    ui->checkBox_16->setEnabled(!gamemode);
    ui->checkBox_17->setEnabled(!gamemode);
    if (gamemode)
        startround();
}

void paintform::on_checkBox_14_clicked()
{
    scene->drawcontours=!scene->drawcontours;
}

void paintform::on_radioButton_6_clicked()
{
    scene->randcolor=false;
    scene->fixcolor=false;
    scene->freqcolor=false;
    scene->randfixcolor=true;
}

void paintform::on_checkBox_15_clicked()
{
    if (musicactiv)
    {
        musicactiv=false;
        pw->addmodeon=false;
        pw->mindplay=false;
        mypen.setWidth(1);
        ui->widget->graph(2)->setPen(mypen);
        ui->widget->graph(2)->pen().setWidth(1);
    }
    else
        musicactiv=true;
    ui->spinBox_5->setEnabled(musicactiv);
    ui->verticalSlider_2->setEnabled(musicactiv);
}

void paintform::on_spinBox_5_valueChanged(int arg1)
{
    borderlevel=arg1;
    ui->verticalSlider_2->setValue(arg1);
}

void paintform::on_verticalSlider_2_sliderMoved(int position)
{
    ui->spinBox_5->setValue(position);
    borderlevel=position;
}

void paintform::setsoundtype(int index)
{
    if (index==0)
        ui->comboBox_2->setCurrentIndex(0);
    else if (index==1)
        ui->comboBox_2->setCurrentIndex(1);
    else
        ui->comboBox_2->setCurrentIndex(2);
}

void paintform::on_comboBox_2_currentIndexChanged(int index)
{
    if (pw->start)
    {
        if (index==0)
            pw->radiobut1();
        else if (index==1)
            pw->radiobut2();
        else
            pw->radiobut3();
    }
}

void paintform::on_pushButton_9_clicked()
{
    QPixmap pixMap;
    if (puzzlemode)
    {
        pixMap = mainpic;
        pw->setbackimage(pixMap);
    }
    else
    if (!scene->drawflow)
    {
        pixMap = ui->graphicsView->grab();
        pw->setbackimage(pixMap);
    }
    else
    {
        pixMap = scene->bkgndimg;
        pw->setbackimage(pixMap);
    }
    this->hide();
    pw->show();
    pw->setFocus();
}

void paintform::on_spinBox_6_valueChanged(int arg1)
{
    borderpicchange=arg1;
}

void paintform::on_checkBox_16_clicked()
{
    if (!puzzlemode)
    {
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        grabpuzzles();
        randompics();
    }

    if (puzzlegrabed)
    {
        if (flowmode)                    
            flowmode=false;                    
        else
        {
            flowmode=true;            
            random_shuffle(puzzlelocs.begin(), puzzlelocs.end());
        }

        ui->checkBox_8->setChecked(flowmode);
        on_checkBox_8_clicked();
        changingpics=flowmode;
        ui->checkBox_10->setChecked(flowmode);                
        ui->checkBox_13->setEnabled(!flowmode);
        ui->checkBox_11->setEnabled(flowmode);        
        ui->checkBox_11->setChecked(flowmode);
        if (flowmode)
            tpicschange->start();
        else
            tpicschange->stop();
        limitpicschange=flowmode;
        bfiltmode=flowmode;
        ui->checkBox_6->setChecked(flowmode);

        if (flowmode)
            ui->graphicsView->setGeometry(650,250,300,200);
        else
            ui->graphicsView->setGeometry(50,50,1500,800);
       // if (qimload)
       //     scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        ui->graphicsView->repaint();
    }
//    pw->bfiltmode=bfiltmode;
//    puzzlemode=true;
}

void paintform::on_checkBox_17_clicked()
{
    collectiveflow=!collectiveflow;
}

void paintform::on_pushButton_10_clicked()
{
    QString filePath=QFileDialog::getExistingDirectory(this, "Get Any Folder", "D://");    
    if (filePath!="")
    {
        fd.setPath(filePath);
        imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
        folderpath=filePath;
      //  pw->setpicfolder(filePath);
    }
}

void paintform::on_checkBox_19_clicked()
{
    adaptivebord=!adaptivebord;
    ui->verticalSlider_2->setEnabled(!adaptivebord);
    ui->spinBox_5->setEnabled(!adaptivebord);
}

void paintform::on_checkBox_20_clicked()
{    
    scene->drawflow=!scene->drawflow;
    if (minimode)
    {
        ui->checkBox_8->setEnabled(!scene->drawflow);
        ui->checkBox_16->setEnabled(!scene->drawflow);
        ui->checkBox_17->setEnabled(!scene->drawflow);
    }
    ui->pushButton_5->setEnabled(!scene->drawflow);
    if ((scene->drawflow) && (!pmain.isNull()))
    {
       // scene->tim->start();
        scene->bkgndimg=pmain;
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
        scene->clear();
        this->repaint();
    }
    else
    {
        // scene->tim->stop();
        pmain=scene->bkgndimg;
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
    }
}

void paintform::on_checkBox_21_clicked()
{
    spacedflow=!spacedflow;
    if (spacedflow)
        scene->tim->start();
    else
        scene->tim->stop();
}

void paintform::on_checkBox_18_clicked()
{
    updateback=!updateback;
    if (updateback)
    {
        ui->checkBox_8->setEnabled(false);
        ui->checkBox_16->setEnabled(false);
        ui->checkBox_17->setEnabled(false);
    } else
    {
        if (minimode)
        {
            ui->checkBox_8->setEnabled(true);
            ui->checkBox_16->setEnabled(true);
            ui->checkBox_17->setEnabled(true);
        }
    }
}

void paintform::on_pushButton_11_clicked()
{
    pmain.load(":/pics/pics/empty.jpg");
    mainpic.load(":/pics/pics/empty.jpg");
    qim.load(":/pics/pics/empty.jpg");
    scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
}
