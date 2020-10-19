/* source file for MindDraw window class -
   resposible for MindDraw parameters and actions */

#include "paintform.h"
#include "ui_paintform.h"
#include "paintscene.h"
#include <QColorDialog>
#include <QApplication>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include "random"
#include <QtConcurrent>
#include <myitem.h>

// TO DO:
// saving history of waves

paintform::paintform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::paintform)
{
    ui->setupUi(this);
    scene = new paintScene(); scene->paintf=this; // create paintscene for drawing

    puzzlemode = false; // puzzle mode - representation of pics as 15 small puzzles
    fixedmain = true; // flag for fixing/changing background picture by attention/meditation value > border
    changingpics = false; // flag for changing pics in puzzle representation
    grabmindplayflow = false; // grab flow from MindPlay
    canpuzzlechange = true;   // to prevent constant change of gathered puzzles, if attention/meditation > border
    firstpuzzle = true;     // flag for checking if first puzzle (loading new set of pics)
    showestatt = false;     // show estimated attention on plot with brain waves expression values
    limitpicschange = false;// if only limited number of puzzles are changing around main pic
    bfiltmode = false;      // if main pic / puzzles in game mode are changing colors / blurring by attention
    erasepen = false;       // if pen is in erase mode
    setloaded = false;      // if puzzle set is loaded
    game_findsame = false;  // flag for game mode (finding 2 same pics among 15)
    musicactiv = false;     // flag for music playing from MindPlay by attention / meditation > border
    puzzlegrabed = false;   // create and save puzzles from single pic
    flowmode = false;       // flowmode - gathering puzzle, collectiveflow - behind overlays of different pics
    collectiveflow = true;  // gathering puzzle behind overlays of different pics
    spacedflow = false;     // experimental mode for drawing structures
    attent_modulaion = true;// attention / meditation modulation
    music_adaptive_bord = false;  // adaptive border for music activation mode
    backloaded = false;           // flag if background image is loaded
    game_gothrough = false;       // game through mode (moving item by attention / meditation)
    canchangeback = true;         // prevent constant change of back image on condition: attention / meditation > border

    prevpuzzle = -1;             // previous puzzle number with puzzle clicks
    prevpict = -1;               // previous pictures index with puzzle clicks
    picsforchange = 4;           // default value for number of puzzles changing in flow mode or around main pic
    eegsize = 1; pensize = 2;    // parameters for brain waves drawing
    soundborderlevel = 80;       // border for playing tones from MindPlay by attention / meditation > border
    borderpicchange = 75;        // border for changing back image by attention / meditation > border
    numsamples = 0;              // number of samples in attention / meditation arrays / plot
    numfrsamples = 0;            // number of samples in brain waves expression arrays / plot
    puzzlew = 300;               // puzzle width
    puzzleh = 200;               // puzzle height
    laststop = 0;                // last point for adaptive border estimation for sound activation mode
    lenofinterval = 3;           // number of points for adaptive border estimation for sound activation mode
    pointsfor_estattention = 5;  // number of points for attention estimation
    gamethroughborder = 42;      // border for game through (when item moves)
    y0border = 600;              // y0 for all borders

    rationmode = Qt::IgnoreAspectRatio; // ration mode for filling images in main pic and puzzles
    graphicsviews_init(); // initialize puzzles graphicsview geometry and properties

    currimglist = QVector<QString>(15); // list of image paths for current puzzle representation
    currentindexes = new int[14];   // initialize indexes for current pics in puzzle
    for (int i=0; i<14; i++)
        currentindexes[i]=0;

    // initialize vectors for attention / meditation / border / brain waves expression / estimated attention
    init_brainwaves_arrays();

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec()); // random generator seed    

    // initialize set for puzzle gathering mode
    for (int i=0; i<15; i++)
        stpic1.insert(i);    
    // initialize vectors for puzzle gathering mode
    onepicarr.resize(15);
    puzzlelocs = vector<int>(15);
    sortpuzzlelocs = vector<int>(15);
    iota(puzzlelocs.begin(), puzzlelocs.end(), 0);
    random_shuffle(puzzlelocs.begin(), puzzlelocs.end());
    randpuzzlelocs = vector<int>(15);
    iota(randpuzzlelocs.begin(), randpuzzlelocs.end(), 0);
    random_shuffle(randpuzzlelocs.begin(), randpuzzlelocs.end());

    timepics = 2000;
    tpicschange = new QTimer(this); // timer for changing puzzles
    tpicschange->connect(tpicschange, SIGNAL(timeout()), this, SLOT(tpicschangeUpdate()));
    tpicschange->setInterval(timepics);

    for (int i=0; i<108; i++)   // arrays of polygons (for experimental drawing mode)
    {
        centercoord[i][0]=qrand()%1500;
        centercoord[i][1]=qrand()%800;
        poltypearr[i]=3+qrand()%6;
    }       

    configure_ui(); // configure ui elements

    pmain.load(":/pics/pics/empty.jpg");
    mainpic.load(":/pics/pics/empty.jpg");
    qim.load(":/pics/pics/empty.jpg");    
}

paintform::~paintform()
{
    delete ui;
}

void paintform::init_brainwaves_arrays()
{
    // initialize vectors for attention / meditation / border plot
    attent_arr = QVector<double>(TPMAX);
    medit_arr = QVector<double>(TPMAX);
    border_arr = QVector<double>(TPMAX);
    for (int j=0; j<TPMAX; j++)
    {
        attent_arr[j]=0;
        medit_arr[j]=0;
        border_arr[j]=0;
    }
    xc = QVector<double>(TPMAX);
    for (int j=0; j<TPMAX; j++)
        xc[j]=0;

    // initialize vectors for brain waves expression / estimated attention plot
    delta_arr = QVector<double>(TFMAX);
    theta_arr = QVector<double>(TFMAX);
    alpha_arr = QVector<double>(TFMAX);
    beta_arr = QVector<double>(TFMAX);
    gamma_arr = QVector<double>(TFMAX);
    hgamma_arr = QVector<double>(TFMAX);
    estatt_arr = QVector<double>(TFMAX);
    fxc = QVector<double>(TFMAX);
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
    fxc = QVector<double>(TFMAX);
    for (int j=0; j<TFMAX; j++)
        fxc[j]=0;
}

void paintform::configure_ui() // configure ui elements
{
    ui->radioButton->setChecked(scene->randcolor);
    ui->radioButton_2->setChecked(scene->fixcolor);
    ui->radioButton_3->setChecked(scene->freqcolor);
    ui->radioButton_6->setChecked(scene->randfixcolor);
    ui->checkBox_16->setEnabled(false);
    ui->checkBox_9->setChecked(true);
    ui->checkBox_17->setEnabled(false);
    ui->pushButton_3->setGeometry(660,865,100,30);
    ui->radioButton_4->setGeometry(740,905,150,20);
    ui->radioButton_5->setGeometry(740,930,150,20);
    ui->pushButton->setGeometry(780,865,70,20);
    ui->pushButton_11->setGeometry(780,885,70,20);
    ui->label_4->setGeometry(855,920,120,20);
    ui->label_5->setGeometry(950,910,70,50);
    ui->pushButton_4->setGeometry(860,865,50,24);
    ui->label_8->setGeometry(920,865,50,20);
    ui->spinBox_7->setGeometry(970,865,50,20);
    ui->pushButton_12->setGeometry(940,891,80,24);
    ui->pushButton_9->setGeometry(860,891,80,24);
    ui->checkBox_8->setEnabled(false);
    ui->verticalSlider->setGeometry(1038,820,20,150);        
    ui->verticalSlider->setVisible(false);

    ui->checkBox_10->setGeometry(1070,900,130,20);
    ui->checkBox_6->setGeometry(1070,920,221,20);
    ui->checkBox_10->setEnabled(false);        
    ui->spinBox_3->setGeometry(1200,900,50,20);
    ui->label_3->setGeometry(1255,900,30,20);
    ui->checkBox_11->setGeometry(1290,900,120,20);
    ui->spinBox_4->setGeometry(1385,900,35,20);

    ui->pushButton_5->setGeometry(1070,865,80,30);
    ui->pushButton_2->setGeometry(1160,865,100,30);
    ui->pushButton_6->setGeometry(1270,865,100,30);
    ui->pushButton_7->setGeometry(1380,865,100,30);
    ui->checkBox_16->setGeometry(1490,855,85,30);
    ui->checkBox_16->setVisible(false);
    ui->pushButton_10->setGeometry(1430,900,116,22);    
    ui->comboBox_2->setGeometry(1430,924,100,20);
    ui->checkBox_22->setGeometry(1325,924,95,20);
    ui->pushButton_7->setEnabled(false);
    ui->comboBox->setVisible(false);
    ui->comboBox->setGeometry(1420,900,130,25);    
    ui->checkBox->setGeometry(550,860,110,20);
    ui->checkBox_2->setGeometry(550,880,110,20);
    ui->radioButton->setGeometry(554,860,120,20);
    ui->radioButton_2->setGeometry(554,883,120,20);
    ui->radioButton_3->setGeometry(554,905,145,20);
    ui->radioButton_6->setGeometry(554,927,100,20);
    ui->label->setGeometry(380,853,100,20);
    ui->spinBox->setGeometry(475,853,40,20);
    ui->label_2->setGeometry(260,853,60,20);
    ui->checkBox_14->setGeometry(260,878,120,20);
    ui->checkBox_3->setGeometry(390,878,120,20);
    ui->label_7->setGeometry(260,878,120,20);    
    ui->label_7->setVisible(false);
    ui->spinBox_6->setGeometry(370,878,40,20);    
    ui->spinBox_6->setVisible(false);
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
    ui->verticalSlider_2->setVisible(false);
    ui->checkBox_5->setGeometry(400,905,120,20);
    ui->checkBox_5->setVisible(false);    
    ui->spinBox_5->setValue(soundborderlevel);

    ui->widget->setGeometry(50,656,740,195);
    ui->widget->setVisible(true);
    ui->widget_2->setGeometry(805,656,745,195);
    ui->widget_2->setVisible(true);
    ui->widget_2->yAxis->setLabel("%");
    ui->widget_2->rescaleAxes();
    ui->widget->yAxis->setLabel("%");
    ui->widget->rescaleAxes();

    sp1.setColor(QPalette::Window, Qt::white);
    sp1.setColor(QPalette::WindowText, Qt::red);
    sp2.setColor(QPalette::Window, Qt::white);
    sp2.setColor(QPalette::WindowText, Qt::darkGreen);

    ui->comboBox_3->setGeometry(585,26,105,22);
    ui->progressBar->setGeometry(700,26,236,20);
    ui->progressBar->setPalette(sp1);
    ui->checkBox_8->setGeometry(60,26,120,21);
    ui->label_6->setGeometry(1080,25,61,21);
    ui->label_6->setVisible(false);
    ui->checkBox_13->setGeometry(197,27,111,20);
    ui->checkBox_23->setGeometry(312,27,111,20);
    ui->checkBox_17->setGeometry(433,27,141,20);

    ui->checkBox_9->setGeometry(950,26,81,20);
    ui->checkBox_20->setGeometry(1046,26,81,20);
    ui->checkBox_18->setGeometry(1142,26,171,20);

    ui->checkBox_7->setGeometry(80,858,151,20);
    ui->checkBox_12->setGeometry(80,878,99,20);
    ui->checkBox_12->setChecked(true);
    ui->spinBox_2->setGeometry(320,853,40,20);
    ui->checkBox->setChecked(scene->randcolor);
    ui->checkBox->setVisible(false);
    ui->checkBox_2->setChecked(scene->drawbpoints);
    ui->checkBox_2->setVisible(false);
    ui->spinBox->setValue(eegsize);
    ui->spinBox_2->setValue(pensize);
    ui->checkBox_8->setChecked(puzzlemode);
    ui->checkBox_13->setEnabled(false);
    ui->checkBox_11->setEnabled(false);

    ui->horizontalSlider_2->setGeometry(700,26,160,20);
    ui->horizontalSlider_2->setValue(80);
    ui->horizontalSlider_2->setVisible(false);

    ui->widget->xAxis->setRange(0,32);
    ui->widget->yAxis->setRange(0,101);
    ui->widget->legend->setVisible(true);    
    QFont legendFont = font();
    legendFont.setPointSize(6);
    legendFont.setBold(false);
    ui->widget->legend->setFont(legendFont);
    ui->widget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->widget->legend->setBrush(QBrush(QColor(255,255,255,190)));

    ui->widget->addGraph();        
    QPen qpt_att(Qt::darkRed);
    qpt_att.setWidth(3);
    ui->widget->graph(0)->setPen(qpt_att);
    ui->widget->graph(0)->setName("Attention");
    ui->widget->graph(0)->setData(xc, attent_arr);
    QPen qpts_att = ui->widget->graph(0)->pen();
    qpts_att.setColor(Qt::red);
    ui->widget->graph(0)->selectionDecorator()->setPen(qpts_att);

    ui->widget->addGraph();
    ui->widget->setInteraction(QCP::iSelectPlottables, true);
    QPen qpt_medit(Qt::darkGreen);
    qpt_medit.setWidth(1);
    ui->widget->graph(1)->setPen(qpt_medit);
    ui->widget->graph(1)->setName("Meditation");
    ui->widget->graph(1)->setData(xc, medit_arr);    
    QPen qpts_medit =  ui->widget->graph(1)->pen();
    qpts_medit.setColor(Qt::green);
    ui->widget->graph(1)->selectionDecorator()->setPen(qpts_medit);
    ui->widget->replot();
    ui->widget->setAutoFillBackground(false);

    ui->widget->addGraph();    
    QPen qpt_bord("orange");
    mypen = qpt_bord;
    mypen.setWidth(1);
    ui->widget->graph(2)->setPen(mypen);
    ui->widget->graph(2)->setName("Border");
    ui->widget->graph(2)->setData(xc, border_arr);
    ui->widget->graph(2)->selectionDecorator()->setPen(mypen);

    ui->widget_2->xAxis->setRange(0,32);
    ui->widget_2->legend->setVisible(true);
    ui->widget_2->legend->setBrush(QBrush(QColor(255,255,255,190)));
    ui->widget_2->setInteraction(QCP::iSelectPlottables, true);
    ui->widget_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->widget_2->legend->setFont(legendFont);

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
    QPen qpt_estatt(Qt::red);
    qpt_estatt.setWidth(2);
    ui->widget_2->graph(6)->setPen(qpt_estatt);
    ui->widget_2->graph(6)->setName("Est-Attention");
    ui->widget_2->graph(6)->setData(fxc, estatt_arr);

    ui->widget_2->yAxis->setRange(0,50);
    ui->widget_2->graph(6)->setVisible(false);

    ui->widget->setAttribute(Qt::WA_TranslucentBackground,true);
    ui->widget->setWindowOpacity(0.5);
    ui->widget_2->replot();  

    ui->label_23->setGeometry(50,25,181,26);
    ui->label_23->setPalette(sp1);
    ui->label_24->setGeometry(250,25,181,26);
    ui->label_24->setPalette(sp2);
    ui->label_23->setVisible(false);
    ui->label_24->setVisible(false);

    ui->spinBox_3->setValue(timepics);
    ui->radioButton_4->setChecked(attent_modulaion);
    ui->spinBox_6->setValue(borderpicchange);
}

void paintform::graphicsviews_init() // initialize puzzles graphicsview geometry and properties
{
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
    ui->graphicsView_16->setGeometry(50,50,1500,800);
    ui->graphicsView_16->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_16->setAlignment(Qt::AlignCenter);
    ui->graphicsView_16->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsView->setFrameStyle(0);
    ui->graphicsView->setStyleSheet("background: transparent");
    ui->graphicsView_16->setFrameStyle(0);
    ui->graphicsView_16->setStyleSheet("background: transparent");
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

    scene->setSceneRect(ui->graphicsView->rect());
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    scene2 = new QGraphicsScene();
    scene2->setSceneRect(ui->graphicsView_2->rect());
    ui->graphicsView_2->setScene(scene2);
    scene3 = new QGraphicsScene();
    scene3->setSceneRect(ui->graphicsView_3->rect());
    ui->graphicsView_3->setScene(scene3);
    scene4 = new QGraphicsScene();
    scene4->setSceneRect(ui->graphicsView_4->rect());
    ui->graphicsView_4->setScene(scene4);
    scene5 = new QGraphicsScene();
    scene5->setSceneRect(ui->graphicsView_5->rect());
    ui->graphicsView_5->setScene(scene5);
    scene6 = new QGraphicsScene();
    scene6->setSceneRect(ui->graphicsView_6->rect());
    ui->graphicsView_6->setScene(scene6);
    scene7 = new QGraphicsScene();
    scene7->setSceneRect(ui->graphicsView_7->rect());
    ui->graphicsView_7->setScene(scene7);
    scene8 = new QGraphicsScene();
    scene8->setSceneRect(ui->graphicsView_8->rect());
    ui->graphicsView_8->setScene(scene8);
    scene9 = new QGraphicsScene();
    scene9->setSceneRect(ui->graphicsView_9->rect());
    ui->graphicsView_9->setScene(scene9);
    scene10 = new QGraphicsScene();
    scene10->setSceneRect(ui->graphicsView_10->rect());
    ui->graphicsView_10->setScene(scene10);
    scene11 = new QGraphicsScene();
    scene11->setSceneRect(ui->graphicsView_11->rect());
    ui->graphicsView_11->setScene(scene11);
    scene12 = new QGraphicsScene();
    scene12->setSceneRect(ui->graphicsView_12->rect());
    ui->graphicsView_12->setScene(scene12);
    scene13 = new QGraphicsScene();
    scene13->setSceneRect(ui->graphicsView_13->rect());
    ui->graphicsView_13->setScene(scene13);
    scene14 = new QGraphicsScene();
    scene14->setSceneRect(ui->graphicsView_14->rect());
    ui->graphicsView_14->setScene(scene14);
    scene15 = new QGraphicsScene();
    scene15->setSceneRect(ui->graphicsView_15->rect());
    ui->graphicsView_15->setScene(scene15);   

    scenegame = new QGraphicsScene();
    scenegame->setSceneRect(ui->graphicsView_16->rect());
    ui->graphicsView_16->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView_16->setScene(scenegame);

    // object for game for moving item by attention / meditation
    movingItem = new MyItem();
    movingItem->pfw = this;
    scenegame->addItem(movingItem);
    moveItemInterval = 100;
    moveItemTimer = new QTimer(this);
    moveItemTimer->connect(moveItemTimer, SIGNAL(timeout()),scenegame,SLOT(advance()));
    moveItemTimer->setInterval(moveItemInterval);
    ui->graphicsView_16->setVisible(false);    
    initborderlines();

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
    ui->graphicsView_16->installEventFilter(this);
}

void paintform::initborderlines()
{
    QPen mpen;
    mpen.setWidth(8);
    qsrand(QDateTime::currentMSecsSinceEpoch()%UINT_MAX);
    for (size_t t = 0; t<8; t++)
    {
        QPoint pt;
        pt.setX(50+180*t+qrand()%80);
        pt.setY(500-qrand()%200);
        itemborders.push_back(pt);
        mpen.setColor(QColor(qrand()%256,qrand()%256,qrand()%256));
        borderlines.push_back(scenegame->addLine(pt.x(),y0border,pt.x(),pt.y(),mpen));
    }
}

void paintform::updateborderlines(double scale)
{
    QPen mpen;
    mpen.setWidth(8);
    for (size_t t = 0; t<8; t++)
    {
        scenegame->removeItem(borderlines[t]);
        QPoint pt;
        pt.setX(itemborders[t].x());
        pt.setY(y0border-(y0border-itemborders[t].y())*scale);
        mpen.setColor(QColor(qrand()%256,qrand()%256,qrand()%256));
        borderlinesnew.push_back(scenegame->addLine(pt.x(),y0border,pt.x(),pt.y(),mpen));
    }
    borderlines.clear();
    for (size_t t = 0; t<8; t++)
        borderlines.push_back(borderlinesnew[t]);
    borderlinesnew.clear();
}

void paintform::playsometone() // play random tone from MindPlay
{
    pw->tonenumbers=0;
    pw->tones=" ";
    pw->randomtone();
    pw->letsplay();
}

void paintform::newroundmovegame() // new round for moving item game
{
    if (!grabmindplayflow)
        on_pushButton_6_clicked();
    itemborders.clear();
    scenegame->removeItem(movingItem);
    scenegame->clear();
    scenegame->addItem(movingItem);
    borderlines.clear();
    initborderlines();
}

void paintform::setpicfolder(QString fpath) // set folder for pictures
{
    folderpath = fpath;
    fd.setPath(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    randnumb = vector<int>(imglist.length());
    randnumb.resize(imglist.length());
    iota(randnumb.begin(), randnumb.end(), 0);
    random_shuffle(randnumb.begin(), randnumb.end());
    iconsready=false;
    pmarray.resize(imglist.length());
    if (imglist.length()>14)
        initpics();
}

void paintform::setdflowtime(int t) // set flow time - delay for chaning puzzles
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

void paintform::setflowspace(int t) // set flow space - number of puzzles for change
{
    if ((t<80) && (!canpuzzlechange))
    {
        // when puzzle is completed and attention/meditation goes below border
        delay(100);
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
           picsforchange=10;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>30) && (t<40))
       {
           if (collectiveflow)
               picsforchange=8;
           else
               picsforchange=9;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>40) && (t<50))
       {
           if (collectiveflow)
               picsforchange=6;
           else
               picsforchange=7;
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
       else if ((t>70) && (t<75))
       {
           if (collectiveflow)
               picsforchange=2;
           else
               picsforchange=3;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if ((t>75) && (t<80))
       {
           if (collectiveflow)
               picsforchange=1;
           else
               picsforchange=2;
           ui->spinBox_4->setValue(picsforchange);
       }
       else if (t>80)
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

bool paintform::getattentmode()  // return attention modulation (for paintScene color modulation)
{
    return attent_modulaion;
}

double paintform::getestattval()  // return estimated attention value (for MindOCV controls)
{
    if (numfrsamples>0)
        return estattn;
    else
        return 0;
}

void paintform::update_estrate(int t)
// update brain waves estimation rate spinBox (interval length), invoked from MindPlay
{
    ui->spinBox_7->setValue(t);
}

void paintform::updatefreqarrs(double deltat, double thetat, double alphat, double betat, double gammat, double hgammat)
// update brain waves expression arrays and plot
{  
    if (numfrsamples>2000)
    {
        numfrsamples-=2001;
        ui->widget_2->xAxis->moveRange(-1968);
    } else
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

    if (pw->imlength>=256)
        pointsfor_estattention = 5;     // estimation based on last [ >= 2.5] sec
    else
        pointsfor_estattention = 10;    // estimation based on last [ >= 1.4] sec

    if (numfrsamples<pointsfor_estattention)
        estattn=(1-thetat/betat)*100;
    else
    {
        for (int i=0; i<pointsfor_estattention; i++)
        {
            thet+=theta_arr[numfrsamples-i];
            bet+=beta_arr[numfrsamples-i];
        }
        thet/=pointsfor_estattention;
        bet/=pointsfor_estattention;
        estattn=((1-thet/bet)-0.4)/0.4*100;
    }
    if (estattn<5) estattn=5;
    if (estattn>100) estattn=100;
    estatt_arr[numfrsamples]=estattn;       
    fxc[numfrsamples]=numfrsamples;

    // pw->update_attention(estattn); // if use estimated attention for MindPlay

    if (game_findsame)
    {
        ui->verticalSlider->setValue((int)estattn);
        ui->label_5->setText(QString::number((int)estattn)+"%");
        ui->label_23->setText("ATTENTION: "+QString::number((int)estattn)+"%");
    }

  //  if (gamethrough)
  //  {
  //      moveItemInterval=100-estattn;
  //      moveItemTimer->setInterval(moveItemInterval);
  //  }

  /*  if ((bfiltmode) || (!canpuzzlechange)) // if estimated attention is used for flow parameters
    {
        setdflowtime(estattn);
        setflowspace(estattn);
    } */

    ui->widget_2->graph(0)->setData(fxc, delta_arr);
    ui->widget_2->graph(1)->setData(fxc, theta_arr);
    ui->widget_2->graph(2)->setData(fxc, alpha_arr);
    ui->widget_2->graph(3)->setData(fxc, beta_arr);
    ui->widget_2->graph(4)->setData(fxc, gamma_arr);
    ui->widget_2->graph(5)->setData(fxc, hgamma_arr);
    ui->widget_2->graph(6)->setData(fxc, estatt_arr);

    ui->widget_2->replot(); 

    numfrsamples++;
}

void paintform::loadempty() // load empty white main image
{
    pmain.load(":/pics/pics/empty.jpg");
    mainpic.load(":/pics/pics/empty.jpg");
    qim.load(":/pics/pics/empty.jpg");
    scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
}

void paintform::addScaledImage(int num) // add rescaled image
{
    pmarray[num]=QPixmap::fromImage(imageScaling->resultAt(num));
}

void paintform::scalingFinished()  // when all rescaling through QtConcurrent is finished
{
    iconsready = true;
    firstpuzzle = true;
    ui->spinBox_3->setMinimum(100);
    if (!puzzlemode)
    {
        ui->checkBox_16->setEnabled(true);
        ui->checkBox_17->setEnabled(true);
        ui->checkBox_8->setEnabled(true);
    }
}

void paintform::initpics() // make puzzles from images
{                    
    imageScaling = new QFutureWatcher<QImage>(this);    // rescaling images, making icons
    connect(imageScaling, &QFutureWatcher<QImage>::resultReadyAt, this, &paintform::addScaledImage);
    connect(imageScaling, &QFutureWatcher<QImage>::finished, this, &paintform::scalingFinished);

    QString fpath = folderpath;
    std::function<QImage(const QString&)> rescale = [fpath](const QString &imageFileName)
    {
        QString picst = fpath+"/"+imageFileName;
        Mat tempimg = imread(picst.toStdString());
        Mat dstr, temp;
        Size size(300,200);
        cv::resize(tempimg,dstr,size);
        cvtColor(dstr, temp, COLOR_BGR2RGB);
        QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        dest.bits();
        return dest;
        //  QImage image(picst);  // ~ 2 times slower
        //  return image.scaled(QSize(imageSize, imageSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    };
    imageScaling->setFuture(QtConcurrent::mapped(imglist, rescale));
}

void paintform::updateattentionplot(int t) // update attention (values from EEG device) array and plot
{
    if (numsamples>7200) // shift of the plot after 2 hours to zero point (avoid limit on array length)
    {
        numsamples-=7201;
        ui->widget->xAxis->moveRange(-7168);
    } else
    if (numsamples>32)
        ui->widget->xAxis->moveRange(1);

    numsamples++;

    attent_arr[numsamples]=t;
    border_arr[numsamples]=soundborderlevel;
    xc[numsamples]=numsamples;

    ui->widget->graph(0)->setData(xc, attent_arr);
    ui->widget->graph(2)->setData(xc, border_arr);

    ui->widget->replot();  

    updateattention(t);
}

void paintform::updateattention(int t) // update attention dependent variables
{
    scene->attentt=t;

    if (attent_modulaion)
    {
        scene->drawrate=80-t;       // contour mode drawing rate
        if (scene->drawrate<5)
            scene->drawrate=5;
        scene->tim->setInterval(scene->drawrate);        

        if ((bfiltmode) || (!canpuzzlechange))
        {
            setdflowtime(t);
            setflowspace(t);
        }

        if ((numsamples-laststop>lenofinterval) && (music_adaptive_bord))
        // adaptive border for music activation
        {
            avgv = 0;
            for (int i = numsamples-lenofinterval; i<numsamples; i++)
                avgv+=attent_arr[i];
            avgv/=lenofinterval;
            if (avgv<95)
                soundborderlevel=avgv;
            else
                soundborderlevel=99;
            laststop=numsamples;
            ui->verticalSlider_2->setValue(soundborderlevel);
            ui->spinBox_5->setValue(soundborderlevel);
        }

        if (musicactiv)
        {
            if (t>soundborderlevel)
            {
                pw->brainflow_on=true;
                pw->musicmode_on=true;
                mypen = ui->widget->graph(2)->pen();
                mypen.setWidth(3);
                ui->widget->graph(2)->setPen(mypen);
            } else
            {
                mypen.setWidth(1);
                ui->widget->graph(2)->setPen(mypen);
                ui->widget->graph(2)->pen().setWidth(1);
                pw->brainflow_on=false;
                pw->musicmode_on=false;
            }
        }

        ui->label_23->setStyleSheet("QLabel { color : red; }");
        ui->radioButton_4->setStyleSheet("QRadioButton { color : red; }");

        if (!game_findsame)
        {
            ui->verticalSlider->setValue(t);
            ui->label_5->setText(QString::number(t)+"%");
            ui->label_23->setText("ATTENTION: "+QString::number(t)+"%");
        }

        if (game_gothrough)
        {
            if (t<gamethroughborder)
            {
                if (!movingItem->hitborder)
                {
                    movingItem->cantgo=true;
                    movingItem->update();
                }
                else
                    movingItem->godown=true;
            }
            else
            {
                movingItem->cantgo=false;
                movingItem->godown=false;
            }
            moveItemInterval=110-t;
            moveItemTimer->setInterval(moveItemInterval);
            updateborderlines((double)(130-t)/100);
        }

        // canchangeback - flag to prevent constant change of back image when attention / meditation > border:
        // change only when it becomes > border after it was less
        if ((!fixedmain) && (!flowmode) && (!game_findsame) && (t<borderpicchange))
            if (!canchangeback)
                canchangeback = true;

        // change of back image randomly by attention / meditation > border
        if ((!fixedmain) && (!flowmode) && (!game_findsame) && (t>borderpicchange) && (canchangeback))
        {
            on_pushButton_6_clicked();
            canchangeback = false;
        }

     }

  //  scene->clear();
 //   scene->drawpolygon(scene->attentt/10,700,400,scene->meditt,0.5);
}

void paintform::updatemeditation(int t) // update meditation array, plot and dependent variables
{
    scene->meditt=t;

    medit_arr[numsamples]=t;
    ui->widget->graph(1)->setData(xc, medit_arr);

    if (!attent_modulaion)
    {  
       ui->verticalSlider->setValue(t);

       if ((bfiltmode) || (!canpuzzlechange))
       {
           setdflowtime(t);
           setflowspace(t);
       }

       if ((numsamples-laststop>lenofinterval) && (music_adaptive_bord))
       {
           avgv=0;
           for (int i=numsamples-lenofinterval; i<numsamples; i++)
               avgv+=medit_arr[i];
           avgv/=lenofinterval;
           if (avgv<95)
               soundborderlevel=avgv;
           else
               soundborderlevel=99;
           laststop=numsamples;
           ui->verticalSlider_2->setValue(soundborderlevel);
           ui->spinBox_5->setValue(soundborderlevel);
       }

       if (musicactiv)
       {
           if (t>soundborderlevel)
           {
               pw->brainflow_on=true;
               pw->musicmode_on=true;
               mypen = ui->widget->graph(2)->pen();
               mypen.setWidth(3);
               ui->widget->graph(2)->setPen(mypen);
           } else
           {
               mypen.setWidth(1);
               ui->widget->graph(2)->setPen(mypen);
               ui->widget->graph(2)->pen().setWidth(1);
               pw->brainflow_on=false;
               pw->musicmode_on=false;
           }
       }

       if (game_gothrough)
       {
           if (t<gamethroughborder)
           {
               if (!movingItem->hitborder)
               {
                   movingItem->cantgo=true;
                   movingItem->update();
               }
               else
                   movingItem->godown=true;
           }
           else
           {
               movingItem->cantgo=false;
               movingItem->godown=false;
           }
           moveItemInterval=110-t;
           moveItemTimer->setInterval(moveItemInterval);
           updateborderlines((double)(130-t)/100);
       }

       ui->label_24->setStyleSheet("QLabel { color : darkGreen; }");
       ui->radioButton_5->setStyleSheet("QRadioButton { color : darkGreen; }");

       ui->label_5->setText(QString::number(t)+"%");
       ui->label_24->setText("MEDITATION: "+QString::number(t)+"%");


       // canchangeback - flag to prevent constant change of back image when attention / meditation > border:
       // change only when it becomes > border after it was less
       if ((!fixedmain) && (!flowmode) && (!game_findsame) && (t<borderpicchange))
           if (!canchangeback)
               canchangeback = true;

       if ((!fixedmain) && (!flowmode) && (!game_findsame) && (t>borderpicchange) && (canchangeback))
       {
           on_pushButton_6_clicked();
           canchangeback = false;
       }
    }
}

int paintform::getaverage_mentallevel(int interval, bool attention)
// get average mental level for last [interval] seconds
{
    double t = 0;
    if (attention)
    {
        for (int i=0; i<interval; i++)
            t+=attent_arr[numsamples-i];
    }
    else
    {
        for (int i=0; i<interval; i++)
            t+=medit_arr[numsamples-i];
    }
    return (int)(t / interval);
}

void paintform::updateset_withlimitpics()
// update limit number of puzzles: some currentin indexes are changed, some stays the same
{
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

void paintform::updateset_allpics() // update all puzzles
{
    ui->graphicsView_2->scene()->clear();
    ui->graphicsView_2->scene()->addPixmap(pmarray[randnumb[0]]);
    currimglist[0]=folderpath+"/"+imglist.at(randnumb[0]);
    currentindexes[0]=randnumb[0];

    ui->graphicsView_3->scene()->clear();
    ui->graphicsView_3->scene()->addPixmap(pmarray[randnumb[1]]);
    currimglist[1]=folderpath+"/"+imglist.at(randnumb[1]);
    currentindexes[1]=randnumb[1];

    ui->graphicsView_4->scene()->clear();
    ui->graphicsView_4->scene()->addPixmap(pmarray[randnumb[2]]);
    currimglist[2]=folderpath+"/"+imglist.at(randnumb[2]);
    currentindexes[2]=randnumb[2];

    ui->graphicsView_5->scene()->clear();
    ui->graphicsView_5->scene()->addPixmap(pmarray[randnumb[3]]);
    currimglist[3]=folderpath+"/"+imglist.at(randnumb[3]);
    currentindexes[3]=randnumb[3];

    ui->graphicsView_6->scene()->clear();
    ui->graphicsView_6->scene()->addPixmap(pmarray[randnumb[4]]);
    currimglist[4]=folderpath+"/"+imglist.at(randnumb[4]);
    currentindexes[4]=randnumb[4];

    ui->graphicsView_7->scene()->clear();
    ui->graphicsView_7->scene()->addPixmap(pmarray[randnumb[5]]);
    currimglist[5]=folderpath+"/"+imglist.at(randnumb[5]);
    currentindexes[5]=randnumb[5];

    ui->graphicsView_8->scene()->clear();
    ui->graphicsView_8->scene()->addPixmap(pmarray[randnumb[6]]);
    currimglist[6]=folderpath+"/"+imglist.at(randnumb[6]);
    currentindexes[6]=randnumb[6];

    ui->graphicsView_9->scene()->clear();
    ui->graphicsView_9->scene()->addPixmap(pmarray[randnumb[7]]);
    currimglist[7]=folderpath+"/"+imglist.at(randnumb[7]);
    currentindexes[7]=randnumb[7];

    ui->graphicsView_10->scene()->clear();
    ui->graphicsView_10->scene()->addPixmap(pmarray[randnumb[8]]);
    currimglist[8]=folderpath+"/"+imglist.at(randnumb[8]);
    currentindexes[8]=randnumb[8];

    ui->graphicsView_11->scene()->clear();
    ui->graphicsView_11->scene()->addPixmap(pmarray[randnumb[9]]);
    currimglist[9]=folderpath+"/"+imglist.at(randnumb[9]);
    currentindexes[9]=randnumb[9];

    ui->graphicsView_12->scene()->clear();
    ui->graphicsView_12->scene()->addPixmap(pmarray[randnumb[10]]);
    currimglist[10]=folderpath+"/"+imglist.at(randnumb[10]);
    currentindexes[10]=randnumb[10];

    ui->graphicsView_13->scene()->clear();
    ui->graphicsView_13->scene()->addPixmap(pmarray[randnumb[11]]);
    currimglist[11]=folderpath+"/"+imglist.at(randnumb[11]);
    currentindexes[11]=randnumb[11];

    ui->graphicsView_14->scene()->clear();
    ui->graphicsView_14->scene()->addPixmap(pmarray[randnumb[12]]);
    currimglist[12]=folderpath+"/"+imglist.at(randnumb[12]);
    currentindexes[12]=randnumb[12];

    ui->graphicsView_15->scene()->clear();
    ui->graphicsView_15->scene()->addPixmap(pmarray[randnumb[13]]);
    currimglist[13]=folderpath+"/"+imglist.at(randnumb[13]);        
    currentindexes[13]=randnumb[13];
}

void paintform::updateset_allpics_similarly(int* picnums)
// update puzzles based on histogramm similarity:
// picnums contains indexes of nearest / farest pics to the main (central) one
{
    ui->graphicsView_2->scene()->clear();
    ui->graphicsView_2->scene()->addPixmap(pmarray[picnums[0]]);
    currimglist[0]=folderpath+"/"+imglist.at(picnums[0]);
    currentindexes[0]=picnums[0];

    ui->graphicsView_3->scene()->clear();
    ui->graphicsView_3->scene()->addPixmap(pmarray[picnums[1]]);
    currimglist[1]=folderpath+"/"+imglist.at(picnums[1]);
    currentindexes[1]=picnums[1];

    ui->graphicsView_4->scene()->clear();
    ui->graphicsView_4->scene()->addPixmap(pmarray[picnums[2]]);
    currimglist[2]=folderpath+"/"+imglist.at(picnums[2]);
    currentindexes[2]=picnums[2];

    ui->graphicsView_5->scene()->clear();
    ui->graphicsView_5->scene()->addPixmap(pmarray[picnums[3]]);
    currimglist[3]=folderpath+"/"+imglist.at(picnums[3]);
    currentindexes[3]=picnums[3];

    ui->graphicsView_6->scene()->clear();
    ui->graphicsView_6->scene()->addPixmap(pmarray[picnums[4]]);
    currimglist[4]=folderpath+"/"+imglist.at(picnums[4]);
    currentindexes[4]=picnums[4];

    ui->graphicsView_7->scene()->clear();
    ui->graphicsView_7->scene()->addPixmap(pmarray[picnums[5]]);
    currimglist[5]=folderpath+"/"+imglist.at(picnums[5]);
    currentindexes[5]=picnums[5];

    ui->graphicsView_8->scene()->clear();
    ui->graphicsView_8->scene()->addPixmap(pmarray[picnums[6]]);
    currimglist[6]=folderpath+"/"+imglist.at(picnums[6]);
    currentindexes[6]=picnums[6];

    ui->graphicsView_9->scene()->clear();
    ui->graphicsView_9->scene()->addPixmap(pmarray[picnums[7]]);
    currimglist[7]=folderpath+"/"+imglist.at(picnums[7]);
    currentindexes[7]=picnums[7];

    ui->graphicsView_10->scene()->clear();
    ui->graphicsView_10->scene()->addPixmap(pmarray[picnums[8]]);
    currimglist[8]=folderpath+"/"+imglist.at(picnums[8]);
    currentindexes[8]=picnums[8];

    ui->graphicsView_11->scene()->clear();
    ui->graphicsView_11->scene()->addPixmap(pmarray[picnums[9]]);
    currimglist[9]=folderpath+"/"+imglist.at(picnums[9]);
    currentindexes[9]=picnums[9];

    ui->graphicsView_12->scene()->clear();
    ui->graphicsView_12->scene()->addPixmap(pmarray[picnums[10]]);
    currimglist[10]=folderpath+"/"+imglist.at(picnums[10]);
    currentindexes[10]=picnums[10];

    ui->graphicsView_13->scene()->clear();
    ui->graphicsView_13->scene()->addPixmap(pmarray[picnums[11]]);
    currimglist[11]=folderpath+"/"+imglist.at(picnums[11]);
    currentindexes[11]=picnums[11];

    ui->graphicsView_14->scene()->clear();
    ui->graphicsView_14->scene()->addPixmap(pmarray[picnums[12]]);
    currimglist[12]=folderpath+"/"+imglist.at(picnums[12]);
    currentindexes[12]=picnums[12];

    ui->graphicsView_15->scene()->clear();
    ui->graphicsView_15->scene()->addPixmap(pmarray[picnums[13]]);
    currimglist[13]=folderpath+"/"+imglist.at(picnums[13]);
    currentindexes[13]=picnums[13];
}

void paintform::updateset_singlepuzzle() // update puzzles with random shuffling of one pic fragments
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

void paintform::updateset_fillcorrectpuzzles()
// fill correct puzzle fragments (positions) of a single image
{
    stpic0.clear();
    for (int i=0; i<picsrestored; i++) // picsrestored - number of correctly placed puzzles
    {
        // stpic0 - set used further for determining remaining puzzles indexes (not correctly placed)
        // (by difference with set of all puzzle indexes) and random shuffling of their position
        stpic0.insert(puzzlelocs[i]);
        switch (puzzlelocs[i])
        {
        case 0:
        {            
            if (collectiveflow)
            // in collective flow - all correct fragments (free of overlay) are colorized in the same way
            {
                pmtemp = scene->filterpuzzle(onepicarr[0],scene->attentt);
                ui->graphicsView_6->scene()->addPixmap(pmtemp.scaled(300,200));
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
                pmtemp = scene->filterpuzzle(onepicarr[1],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[2],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[3],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[4],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[5],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[6],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[7],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[8],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[9],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[10],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[11],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[12],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[13],scene->attentt);
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
                pmtemp = scene->filterpuzzle(onepicarr[14],scene->attentt);
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
}

void paintform::updateset_fillcorrectpuzzles_single()
// fill correct puzzles in case of only single image parts (not collective flow, no overlays)
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

void paintform::randompics() // filling puzzles with random pics according to different modes
{
    random_shuffle(randnumb.begin(), randnumb.end());
    if (!limitpicschange)
        for (int i=0; i<14; i++)
            currentindexes[i]=randnumb[i];

    // flowmode - gathering puzzle, collectiveflow - behind overlays of different pics
    if (!flowmode)
    {
        if (limitpicschange)
        {
            random_shuffle(randnumb.begin(), randnumb.end());
            for (int i=0; i<picsforchange; i++)
            {
                int t = qrand() % 14;
                currentindexes[t]=randnumb[i];
            }
            updateset_withlimitpics();
        }
        else if (collectiveflow)
            updateset_allpics();
        else
            updateset_singlepuzzle();
    }
    else
    {
        // firstly fill all fragments with different pics randomly, then recover N parts from puzzle pic
        if (collectiveflow)
        {
            updateset_allpics();
            scene->clear();
            scene->addPixmap(pmarray[randnumb[18]]);
        }

        //  matchpuzzle();
        scene->randr=qrand()%256; // for colorizing parts
        scene->randg=qrand()%256;
        scene->randb=qrand()%256;

        picsrestored = 15-picsforchange;            // picsforchange determined by attention
        updateset_fillcorrectpuzzles();             // filling correct fragments in collective flow
        if (!collectiveflow)
            updateset_fillcorrectpuzzles_single();      // filling correct fragments in one pic puzzle
    }
    setloaded=true;
}

void paintform::matchpuzzle()  // filling correctly placed puzzles
{
    ui->graphicsView_6->scene()->addPixmap(onepicarr[0]);   
    ui->graphicsView_7->scene()->addPixmap(onepicarr[1]);  
    ui->graphicsView_8->scene()->addPixmap(onepicarr[2]);   
    ui->graphicsView_9->scene()->addPixmap(onepicarr[3]);   
    ui->graphicsView_10->scene()->addPixmap(onepicarr[4]);  
    ui->graphicsView_3->scene()->addPixmap(onepicarr[5]);  
    ui->graphicsView_2->scene()->addPixmap(onepicarr[6]);   
    scene->addPixmap(onepicarr[7]);  
    ui->graphicsView_4->scene()->addPixmap(onepicarr[8]);  
    ui->graphicsView_5->scene()->addPixmap(onepicarr[9]);  
    ui->graphicsView_11->scene()->addPixmap(onepicarr[10]);  
    ui->graphicsView_12->scene()->addPixmap(onepicarr[11]);  
    ui->graphicsView_13->scene()->addPixmap(onepicarr[12]);  
    ui->graphicsView_14->scene()->addPixmap(onepicarr[13]);  
    ui->graphicsView_15->scene()->addPixmap(onepicarr[14]);   
}

void paintform::updatepuzzles() // updating puzzles with current pics indexes
{
    ui->graphicsView_2->scene()->clear();
    ui->graphicsView_2->scene()->addPixmap(pmarray[currentindexes[0]]);
    ui->graphicsView_3->scene()->clear();
    ui->graphicsView_3->scene()->addPixmap(pmarray[currentindexes[1]]);
    ui->graphicsView_4->scene()->clear();
    ui->graphicsView_4->scene()->addPixmap(pmarray[currentindexes[2]]);
    ui->graphicsView_5->scene()->clear();
    ui->graphicsView_5->scene()->addPixmap(pmarray[currentindexes[3]]);
    ui->graphicsView_6->scene()->clear();
    ui->graphicsView_6->scene()->addPixmap(pmarray[currentindexes[4]]);
    ui->graphicsView_7->scene()->clear();
    ui->graphicsView_7->scene()->addPixmap(pmarray[currentindexes[5]]);
    ui->graphicsView_8->scene()->clear();
    ui->graphicsView_8->scene()->addPixmap(pmarray[currentindexes[6]]);
    ui->graphicsView_9->scene()->clear();
    ui->graphicsView_9->scene()->addPixmap(pmarray[currentindexes[7]]);
    ui->graphicsView_10->scene()->clear();
    ui->graphicsView_10->scene()->addPixmap(pmarray[currentindexes[8]]);
    ui->graphicsView_11->scene()->clear();
    ui->graphicsView_11->scene()->addPixmap(pmarray[currentindexes[9]]);
    ui->graphicsView_12->scene()->clear();
    ui->graphicsView_12->scene()->addPixmap(pmarray[currentindexes[10]]);
    ui->graphicsView_13->scene()->clear();
    ui->graphicsView_13->scene()->addPixmap(pmarray[currentindexes[11]]);
    ui->graphicsView_14->scene()->clear();
    ui->graphicsView_14->scene()->addPixmap(pmarray[currentindexes[12]]);
    ui->graphicsView_15->scene()->clear();
    ui->graphicsView_15->scene()->addPixmap(pmarray[currentindexes[13]]);
}

void paintform::swappuzzles(int t1, int t2)     // swapping puzzle pics
{
    if (t1==14) // main puzzle case
    {
        pmain.load(currimglist[t2]);
        qim.load(currimglist[t2]);
        mainpic.load(currimglist[t2]);
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        scene->paintf->repaint();
    }
    if (t2==14) // main puzzle case
    {
        pmain.load(currimglist[t1]);
        qim.load(currimglist[t1]);
        mainpic.load(currimglist[t1]);
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        scene->paintf->repaint();
    }

    int tempind = currentindexes[t1];
    currentindexes[t1]=currentindexes[t2];
    currentindexes[t2]=tempind;
    currimglist[t1]=folderpath+"/"+imglist.at(currentindexes[t1]);
    currimglist[t2]=folderpath+"/"+imglist.at(currentindexes[t2]);
    updatepuzzles();
}

void paintform::mainpuzzle_update(int t)    // updating main puzzle
{
    pmain.load(currimglist[t]);
    qim.load(currimglist[t]);
    mainpic.load(currimglist[t]);
    scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
    scene->paintf->repaint();
    int tr = qrand() % imglist.length();
    currentindexes[14]=currentindexes[t];
    currentindexes[t]=tr;
    currimglist[t]=folderpath+"/"+imglist.at(tr);

    switch(t) {
    case 0:
        ui->graphicsView_2->scene()->clear();
        ui->graphicsView_2->scene()->addPixmap(pmarray[tr]);
        break;
    case 1:
        ui->graphicsView_3->scene()->clear();
        ui->graphicsView_3->scene()->addPixmap(pmarray[tr]);
        break;
    case 2:
        ui->graphicsView_4->scene()->clear();
        ui->graphicsView_4->scene()->addPixmap(pmarray[tr]);
        break;
    case 3:
        ui->graphicsView_5->scene()->clear();
        ui->graphicsView_5->scene()->addPixmap(pmarray[tr]);
        break;
    case 4:
        ui->graphicsView_6->scene()->clear();
        ui->graphicsView_6->scene()->addPixmap(pmarray[tr]);
        break;
    case 5:
        ui->graphicsView_7->scene()->clear();
        ui->graphicsView_7->scene()->addPixmap(pmarray[tr]);
        break;
    case 6:
        ui->graphicsView_8->scene()->clear();
        ui->graphicsView_8->scene()->addPixmap(pmarray[tr]);
        break;
    case 7:
        ui->graphicsView_9->scene()->clear();
        ui->graphicsView_9->scene()->addPixmap(pmarray[tr]);
        break;
    case 8:
        ui->graphicsView_10->scene()->clear();
        ui->graphicsView_10->scene()->addPixmap(pmarray[tr]);
        break;
    case 9:
        ui->graphicsView_11->scene()->clear();
        ui->graphicsView_11->scene()->addPixmap(pmarray[tr]);
        break;
    case 10:
        ui->graphicsView_12->scene()->clear();
        ui->graphicsView_12->scene()->addPixmap(pmarray[tr]);
        break;
    case 11:
        ui->graphicsView_13->scene()->clear();
        ui->graphicsView_13->scene()->addPixmap(pmarray[tr]);
        break;
    case 12:
        ui->graphicsView_14->scene()->clear();
        ui->graphicsView_14->scene()->addPixmap(pmarray[tr]);
        break;
    case 13:
        ui->graphicsView_15->scene()->clear();
        ui->graphicsView_15->scene()->addPixmap(pmarray[tr]);
        break;
    }
}

bool paintform::eventFilter(QObject *target, QEvent *event)  // processing key / mouse events
{
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

    if (event->type() == QEvent::KeyPress)      // switch to MindPlay window
    {
        if (keyEvent->key() == Qt::Key_Tab)
        {
            this->hide();
            pw->show();
            pw->setFocus();
        }
    }

    if (((target == ui->graphicsView) || (target == ui->graphicsView_16)) && (event->type() == QEvent::KeyPress))
    {
        if (keyEvent->key() == Qt::Key_Z)
        {
            if (eegsize>1)
                eegsize--;
            ui->spinBox->setValue(eegsize);
        }
        if (keyEvent->key() == Qt::Key_X)
        {
            if (eegsize<40)
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
            if (pensize<20)
                pensize++;
            ui->spinBox_2->setValue(pensize);
        }

        if (keyEvent->key() == Qt::Key_L)   // show / hide estimated attention
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

        if ((keyEvent->key() == Qt::Key_N) && (pw->opencvstart)) // show / hide MindOCV controls form
        {
            if (ocvfm->formshown)
                ocvfm->hide();
            else
                ocvfm->show();
            ocvfm->formshown = !ocvfm->formshown;
        }

        if (keyEvent->key()==Qt::Key_B)     // hide / show progress bar
        {
            ui->progressBar->setVisible(!ui->progressBar->isVisible());
            if (!fixedmain)
                ui->horizontalSlider_2->setVisible(ui->progressBar->isVisible());
        }

        if ((keyEvent->key()==Qt::Key_K) && (!game_findsame) && (!flowmode) && (!pw->filteringback))    // start / stop camera input
        {
            pw->camerainp_on_off();
            if (!grabmindplayflow)
                on_checkBox_18_clicked();
            if (grabmindplayflow)
                ui->checkBox_18->setChecked(true);
            else
                ui->checkBox_18->setChecked(false);
        }

        if (keyEvent->key() == Qt::Key_O)  // clear all drawings
            on_pushButton_clicked();

        if (keyEvent->key() == Qt::Key_P) // hide / show plots
        {
            ui->checkBox_12->setChecked(!ui->checkBox_12->isChecked());
            ui->widget_2->setVisible(ui->checkBox_12->isChecked());
            ui->widget->setVisible(ui->checkBox_12->isChecked());
        }      

        pressedKeys += ((QKeyEvent*)event)->key();

        // drawing brain waves on horizontal line
        if ( (keyEvent->key() == Qt::Key_H) && (!scene->horizline) && (!scene->vertline) )
        {
            scene->horizline=true;
            ui->checkBox_4->setChecked(true);
        }
        else            
        if ( (keyEvent->key() == Qt::Key_H) && (scene->horizline) )
        {
            scene->horizline=false;
            ui->checkBox_4->setChecked(false);
        }
        // drawing brain waves on vertical line
        if ( (keyEvent->key() == Qt::Key_V) && (!scene->vertline) && (!scene->horizline) )
        {
            scene->vertline=true;
            ui->checkBox_5->setChecked(true);
        }
        else
        if ( (keyEvent->key() == Qt::Key_V) && (scene->vertline) )
        {
            scene->vertline=false;
            ui->checkBox_5->setChecked(false);
        }
    }

    // switch to puzzle mode and back to one pic
    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseButtonDblClick))
    {
       if ((!game_findsame) && (!flowmode) && (!scene->spacedview) && (iconsready))
       {
            pmg = mainpic;
            on_checkBox_8_clicked();
            ui->checkBox_8->setChecked(puzzlemode);
            scene->addPixmap(pmg.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            scene->update();
            ui->graphicsView->repaint();
       }
    }

    // processing on mouse events on main pic / central puzzle
    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::RightButton)    // change of pen color
        {
            if ((scene->randfixcolor))
                scene->randfxcl=QColor(qrand()%256,qrand()%256,qrand()%256,255-scene->attentt*2);
        }
        if (mouseEvent->button() == Qt::MiddleButton)   // switch to erase pen
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
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)           // processing of click in game mode
            {
                if ((currentindexes[14]==prevpict) && (prevpuzzle!=1))
                    startround();
                else
                {
                    prevpict = currentindexes[14];
                    prevpuzzle = 1;
                }
            }
            else if (!flowmode)     // processing of click for puzze swapping
            {
                if ((prevpict!=-1) && (prevpict!=14))
                {
                    swappuzzles(prevpict,14);
                    prevpict = -1;
                } else
                    prevpict = 14;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_2) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame) && (!flowmode))
        // middle click - make current puzzle pic as main pic / central puzzle, update current puzzle with random pic
            mainpuzzle_update(0);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame) && (!flowmode))
        // right click - change pic of current puzzle on random pic
        {
            int t = qrand() % imglist.length();
            currentindexes[0]=t;
            ui->graphicsView_2->scene()->addPixmap(pmarray[t]);
            currimglist[0]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        // left click - game mode (check for identity of pic with previously clicked puzzle)
        // not game mode: swapping of puzzle pics
        {
            if (game_findsame)
            {
                if ((currentindexes[0]==prevpict) && (prevpuzzle!=2))
                    startround();
                else
                {
                    prevpict = currentindexes[0];
                    prevpuzzle = 2;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=0))
                {
                    swappuzzles(prevpict,0);
                    prevpict = -1;
                } else
                    prevpict = 0;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_3) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(1);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[1]=t;
            ui->graphicsView_3->scene()->addPixmap(pmarray[t]);
            currimglist[1]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[1]==prevpict) && (prevpuzzle!=3))
                    startround();
                else
                {
                    prevpict = currentindexes[1];
                    prevpuzzle = 3;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=1))
                {
                    swappuzzles(prevpict,1);
                    prevpict = -1;
                } else
                    prevpict = 1;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_4) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(2);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[2]=t;
            ui->graphicsView_4->scene()->addPixmap(pmarray[t]);
            currimglist[2]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[2]==prevpict) && (prevpuzzle!=4))
                    startround();
                else
                {
                    prevpict = currentindexes[2];
                    prevpuzzle = 4;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=2))
                {
                    swappuzzles(prevpict,2);
                    prevpict = -1;
                } else
                    prevpict = 2;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_5) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(3);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[3]=t;
            ui->graphicsView_5->scene()->addPixmap(pmarray[t]);
            currimglist[3]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[3]==prevpict) && (prevpuzzle!=5))
                    startround();
                else
                {
                    prevpict = currentindexes[3];
                    prevpuzzle = 5;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=3))
                {
                    swappuzzles(prevpict,3);
                    prevpict = -1;
                } else
                    prevpict = 3;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_6) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(4);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[4]=t;
            ui->graphicsView_6->scene()->addPixmap(pmarray[t]);
            currimglist[4]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[4]==prevpict) && (prevpuzzle!=6))
                    startround();
                else
                {
                    prevpict = currentindexes[4];
                    prevpuzzle = 6;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=4))
                {
                    swappuzzles(prevpict,4);
                    prevpict = -1;
                } else
                    prevpict = 4;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_7) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(5);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[5]=t;
            ui->graphicsView_7->scene()->addPixmap(pmarray[t]);
            currimglist[5]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[5]==prevpict) && (prevpuzzle!=7))
                    startround();
                else
                {
                    prevpict = currentindexes[5];
                    prevpuzzle = 7;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=5))
                {
                    swappuzzles(prevpict,5);
                    prevpict = -1;
                } else
                    prevpict = 5;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_8) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(6);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[6]=t;
            ui->graphicsView_8->scene()->addPixmap(pmarray[t]);
            currimglist[6]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[6]==prevpict) && (prevpuzzle!=8))
                    startround();
                else
                {
                    prevpict = currentindexes[6];
                    prevpuzzle = 8;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=6))
                {
                    swappuzzles(prevpict,6);
                    prevpict = -1;
                } else
                    prevpict = 6;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_9) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(7);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[7]=t;
            ui->graphicsView_9->scene()->addPixmap(pmarray[t]);
            currimglist[7]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[7]==prevpict) && (prevpuzzle!=9))
                    startround();
                else
                {
                    prevpict = currentindexes[7];
                    prevpuzzle = 9;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=7))
                {
                    swappuzzles(prevpict,7);
                    prevpict = -1;
                } else
                    prevpict = 7;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_10) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(8);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[8]=t;
            ui->graphicsView_10->scene()->addPixmap(pmarray[t]);
            currimglist[8]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[8]==prevpict) && (prevpuzzle!=10))
                    startround();
                else
                {
                    prevpict = currentindexes[8];
                    prevpuzzle = 10;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=8))
                {
                    swappuzzles(prevpict,8);
                    prevpict = -1;
                } else
                    prevpict = 8;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_11) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(9);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[9]=t;
            ui->graphicsView_11->scene()->addPixmap(pmarray[t]);
            currimglist[9]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[9]==prevpict) && (prevpuzzle!=11))
                    startround();
                else
                {
                    prevpict = currentindexes[9];
                    prevpuzzle = 11;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=9))
                {
                    swappuzzles(prevpict,9);
                    prevpict = -1;
                } else
                    prevpict = 9;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_12) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(10);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[10]=t;
            ui->graphicsView_12->scene()->addPixmap(pmarray[t]);
            currimglist[10]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[10]==prevpict) && (prevpuzzle!=12))
                    startround();
                else
                {
                    prevpict = currentindexes[10];
                    prevpuzzle = 12;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=10))
                {
                    swappuzzles(prevpict,10);
                    prevpict = -1;
                } else
                    prevpict = 10;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_13) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(11);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[11]=t;
            ui->graphicsView_13->scene()->addPixmap(pmarray[t]);
            currimglist[11]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[11]==prevpict) && (prevpuzzle!=13))
                    startround();
                else
                {
                    prevpict = currentindexes[11];
                    prevpuzzle = 13;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=11))
                {
                    swappuzzles(prevpict,11);
                    prevpict = -1;
                } else
                    prevpict = 11;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_14) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(12);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[12]=t;
            ui->graphicsView_14->scene()->addPixmap(pmarray[t]);
            currimglist[12]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[12]==prevpict) && (prevpuzzle!=14))
                    startround();
                else
                {
                    prevpict = currentindexes[12];
                    prevpuzzle = 14;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=12))
                {
                    swappuzzles(prevpict,12);
                    prevpict = -1;
                } else
                    prevpict = 12;
            }
        }
    }

    if ((setloaded) && (target == ui->graphicsView_15) && (event->type() == QEvent::MouseButtonPress) && (!flowmode))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if ((mouseEvent->button() == Qt::MiddleButton) && (!game_findsame))
            mainpuzzle_update(13);
        if ((mouseEvent->button() == Qt::RightButton) && (!game_findsame))
        {
            int t = qrand() % imglist.length();
            currentindexes[13]=t;
            ui->graphicsView_15->scene()->addPixmap(pmarray[t]);
            currimglist[13]=folderpath+"/"+imglist.at(t);
        }
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (game_findsame)
            {
                if ((currentindexes[13]==prevpict) && (prevpuzzle!=15))
                    startround();
                else
                {
                    prevpict = currentindexes[13];
                    prevpuzzle = 15;
                }
            }
            else if (!flowmode)
            {
                if ((prevpict!=-1) && (prevpict!=13))
                {
                    swappuzzles(prevpict,13);
                    prevpict = -1;
                } else
                    prevpict = 13;
            }
        }
    }  

    return false;
}

QImage paintform::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent)
// applying QGraphicsEffect to image
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
// filtering (blurring) puzzles in game mode
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    if (attent_modulaion)
        blur->setBlurRadius((100-estattn)/grade);
    else
        blur->setBlurRadius((100-pw->meditt)/grade);
   // QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
   /* if (attentmodu)
        colorize->setStrength((double)estattn/70);
    else
        colorize->setStrength((double)pw->meditt/70);
    colorize->setColor(QColor(qrand()%255,qrand()%255,qrand()%256,qrand()%255));*/

    QImage qbim1 = applyEffectToImage(pm.toImage(), blur, 0);
   // QImage qbim2 = applyEffectToImage(qbim1, colorize, 0);
    gv->scene()->clear();
    gv->scene()->addPixmap(QPixmap::fromImage(qbim1));
    gv->repaint();
}

void paintform::filteringmain_ingame(int grade)
// // filtering (blurring) main puzzle in game mode
{
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    if (attent_modulaion)
        blur->setBlurRadius((100-estattn)/grade);
    else
        blur->setBlurRadius((100-pw->meditt)/grade);
   // QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
   /* if (attentmodu)
        colorize->setStrength((double)estattn/70);
    else
        colorize->setStrength((double)pw->meditt/70);
    colorize->setColor(QColor(qrand()%255,qrand()%255,qrand()%256,qrand()%255)); */

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
    if ((iconsready) && (puzzlemode))
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

void paintform::startround()  // start new round of game
{
    randompics();

    QString filename=folderpath+"/"+imglist.at(randnumb[14]);
    currimglist[14]=filename;
    mainindex = randnumb[14];
    pmain.load(filename);
    qim.load(filename);
    mainpic.load(filename);
    scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));

    vector<int> randnums = vector<int>(15);
    iota(randnums.begin(), randnums.end(), 0);
    random_shuffle(randnums.begin(), randnums.end());

    // define indexes of puzzles with the same pic
    int picn1 = randnums[0];
    int picn2 = randnums[1];        

    // define index of pic for these puzzles
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

void paintform::setbackimage(QPixmap pm) // set back image from Pixmap (MindPlay / MindOCV image)
{
    pmain=pm;
    qim=pm.toImage();
    backloaded = true;
    if (!scene->spacedview)
    {        
        scene->clear();
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        QBrush qbr(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        ui->graphicsView_16->setBackgroundBrush(qbr);
    }
    else
    {
        scene->bkgndimg=pm;
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
        QBrush qbr(Qt::NoBrush);
        ui->graphicsView_16->setBackgroundBrush(qbr);
    }
}

void paintform::adaptivespinrate(bool fl)
// set spinbox back yellow for adaptive intervals (invoked from MindPlay)
{
    if (fl)
        ui->spinBox_7->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox_7->setStyleSheet("QSpinBox { background-color: white; }");
}

void paintform::setbackimageocv(QString filename) // set back image from left panel click
{
    mainpic.load(filename);
    pmain.load(filename);
    qim.load(filename);
    currimglist[14]=filename;        
    for (int i=0; i<imglist.length(); i++) 
        if (folderpath+"/"+imglist.at(i)==filename)
        {
            currentindexes[14]=i;
            break;
        }
    backloaded = true;
    if (scene->spacedview)
    {
        scene->bkgndimg.load(filename);
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
        scene->clear();
        this->repaint();
        QBrush qbr(Qt::NoBrush);
        ui->graphicsView_16->setBackgroundBrush(qbr);
    } else
    {
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        QBrush qbr(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        ui->graphicsView_16->setBackgroundBrush(qbr);
    }

}

void paintform::grabpuzzles() // make puzzles from single image
{
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
}

void paintform::puzzle_onepic_switch()
// switch of central puzzle / main pic and hide/show all other puzzles
{
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

    if ((!flowmode) && (!grabmindplayflow) && (!pw->camerainp))
    {
        scene->addPixmap(mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        QBrush qbr(mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        ui->graphicsView_16->setBackgroundBrush(qbr);
    }

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
}

void paintform::updatemusicmode(bool fl) // update music mode checkBox (invoked from MindPlay)
{
    ui->checkBox_22->setChecked(fl);
}

void paintform::delay(int temp)
{
    QTime dieTime = QTime::currentTime().addMSecs(temp);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// processing ui

void paintform::on_pushButton_clicked() // load clean main pic (not filtered and without drawing)
{    
    if (!pmain.isNull())
    {     
        if (!scene->spacedview)
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

void paintform::on_pushButton_2_clicked() // load main image from file
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),folderpath,"Images (*.png *.bmp *.jpg)");
    if (filename!="")
    {
        mainpic.load(filename);
        pmain.load(filename);
        qim.load(filename);
        currimglist[14]=filename;
        backloaded = true;
        if (scene->spacedview)
        {
            scene->bkgndimg.load(filename);
            qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
            this->setPalette(qpr);
            scene->clear();
            this->repaint();
            QBrush qbr(Qt::NoBrush);
            ui->graphicsView_16->setBackgroundBrush(qbr);
        } else
        {
            scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));            
            QBrush qbr(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
            ui->graphicsView_16->setBackgroundBrush(qbr);
        }
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

void paintform::on_pushButton_3_clicked()  // choice of pen color
{
    scene->fxcolor = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);   
}

// save previous image for cancel last drawing action
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

void paintform::on_pushButton_4_clicked() // save image to file
{
    QString fileName=QFileDialog::getSaveFileName(this, "Save image", folderpath, "BMP Files (*.bmp);;JPEG (*.JPEG);;PNG (*.png)" );
    if (!fileName.isNull())
    {
        QPixmap pixMap = this->ui->graphicsView->grab();
        pixMap.save(fileName);
    }
}

void paintform::on_checkBox_clicked()
{
    if (!ui->checkBox->isChecked())
        scene->randcolor=false;
    else
        scene->randcolor=true;
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

void paintform::on_pushButton_5_clicked() // restore last image (cancel last drawing)
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

void paintform::on_checkBox_4_clicked() // drawing on horizontal line
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

void paintform::on_checkBox_5_clicked()  // drawing on vertical line
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
// brain filtering mode: attention determines blurring effect
// and flow parameters (number of puzzles for change and delay)
{
    bfiltmode=!bfiltmode;
    if (bfiltmode)
    {
        ui->spinBox_3->setStyleSheet("QSpinBox { background-color: yellow; }");
        ui->spinBox_4->setStyleSheet("QSpinBox { background-color: yellow; }");
    } else
    {
        ui->spinBox_3->setStyleSheet("QSpinBox { background-color: white; }");
        ui->spinBox_4->setStyleSheet("QSpinBox { background-color: white; }");
    }
}

void paintform::on_pushButton_6_clicked() // set main pic as random from folder
{    
    int rimg = qrand() % imglist.length();    
    QString filename=folderpath+"/"+imglist.at(rimg);
    mainpic.load(filename);    
    currimglist[14]=filename;
    currentindexes[14]=rimg;
    mainindex = rimg;
    scene->bkgndimg.load(filename);
    pmain.load(filename);    
    qim.load(filename);
    backloaded = true;    
    if (scene->spacedview)
    {
        qpr.setBrush(QPalette::Background, scene->bkgndimg.scaled(this->size(),rationmode,Qt::SmoothTransformation));
        this->setPalette(qpr);
        QBrush qbr(Qt::NoBrush);
        ui->graphicsView_16->setBackgroundBrush(qbr);
    } else
    {
        qim=qim.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation);
        scene->clear();        
        scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        QBrush qbr(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        ui->graphicsView_16->setBackgroundBrush(qbr);        
    }     
}

void paintform::on_checkBox_7_clicked()
// attention modulation for paintScene (if on - modulates amplitude of brain waves with drawing)
{
    scene->attmodul=!scene->attmodul;  
    if (scene->attmodul)
        ui->spinBox->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox->setStyleSheet("QSpinBox { background-color: white; }");
}

void paintform::on_checkBox_8_clicked()  // puzzle mode on / off
{
    puzzlemode=!puzzlemode;

    prevpict = -1;
    prevpuzzle = -1;

    ui->checkBox_10->setEnabled(puzzlemode);
    ui->checkBox_16->setEnabled(!puzzlemode);
    ui->checkBox_17->setEnabled(!puzzlemode);
    ui->checkBox_20->setEnabled(!puzzlemode);
    ui->checkBox_23->setEnabled(!puzzlemode);
    ui->pushButton_7->setEnabled(puzzlemode);

    puzzle_onepic_switch();

    if (!grabmindplayflow)
        ui->checkBox_13->setEnabled(puzzlemode);

    if ((!puzzlemode) && (grabmindplayflow))
        ui->checkBox_17->setEnabled(false);

    if (firstpuzzle)
    {
        randompics();
        if (!backloaded)
            on_pushButton_6_clicked();
        firstpuzzle=false;
    }
}

void paintform::on_pushButton_7_clicked()   // load random pics in puzzles
{
    randompics();
}

void paintform::on_checkBox_9_clicked()     // fix/unfix main pic for change by attention > border
{
    fixedmain=!fixedmain;
    ui->horizontalSlider_2->setVisible(!fixedmain);
}

void paintform::tpicschangeUpdate()
{
    randompics();
}

void paintform::on_spinBox_3_valueChanged(int arg1)   // rate / delay of puzzle change flow
{
    if (puzzlemode)
    {
        timepics=arg1;
        tpicschange->setInterval(timepics);
    }
}

void paintform::on_checkBox_10_clicked()    // changing pics in puzzles
{
    changingpics=!changingpics;
    ui->checkBox_13->setEnabled(!changingpics);
    if (iconsready)
        ui->checkBox_11->setEnabled(changingpics);
    if (changingpics)
        tpicschange->start();
    else
        tpicschange->stop();
}

void paintform::on_radioButton_4_clicked()  // attention modulation
{
    ui->radioButton_5->setStyleSheet("QRadioButton { color : black; }");
   // ui->label_24->setVisible(false);
   // ui->label_23->setVisible(true);
    ui->progressBar->setPalette(sp1);
    pw->attention_modulation=true;
    attent_modulaion=true;

    if (!game_findsame)
    {
        QPen qpt_att = ui->widget->graph(0)->pen();
        qpt_att.setWidth(3);
        ui->widget->graph(0)->setPen(qpt_att);
        QPen qpt_med = ui->widget->graph(1)->pen();
        qpt_med.setWidth(1);
        ui->widget->graph(1)->setPen(qpt_med);
    }

    ui->comboBox_3->setCurrentIndex(0);
 }

void paintform::on_radioButton_5_clicked()  // meditation modulation
{
    ui->radioButton_4->setStyleSheet("QRadioButton { color : black; }");
   // ui->label_23->setVisible(false);
   // ui->label_24->setVisible(true);
    ui->progressBar->setPalette(sp2);
    pw->attention_modulation=false;
    attent_modulaion=false;

    if (!game_findsame)
    {
        QPen qpt_att = ui->widget->graph(0)->pen();
        qpt_att.setWidth(1);
        ui->widget->graph(0)->setPen(qpt_att);
        QPen qpt_med = ui->widget->graph(1)->pen();
        qpt_med.setWidth(3);
        ui->widget->graph(1)->setPen(qpt_med);
    }

    ui->comboBox_3->setCurrentIndex(1);    
}

void paintform::on_checkBox_11_clicked()    // changing only limit number of puzzles
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

void paintform::on_checkBox_12_clicked()    // hide / show plots
{
     ui->widget_2->setVisible(ui->checkBox_12->isChecked());
     ui->widget->setVisible(ui->checkBox_12->isChecked());
}

void paintform::on_checkBox_13_clicked()    // start / stop game mode
{
    game_findsame = !game_findsame;
    prevpict = -1;
    prevpuzzle = -1;
    ui->checkBox_8->setEnabled(!game_findsame);
    ui->checkBox_10->setEnabled(!game_findsame);
    ui->checkBox_18->setEnabled(!game_findsame);
    showestatt = game_findsame;
    if (showestatt)
    {
        ui->widget_2->yAxis->setRange(0,100);
        ui->widget_2->graph(6)->setVisible(true);
    } else
    {
        ui->widget_2->yAxis->setRange(0,50);
        ui->widget_2->graph(6)->setVisible(false);
    }
    if (game_findsame)
    {
        QPen qpt_att = ui->widget->graph(0)->pen();
        qpt_att.setWidth(1);
        ui->widget->graph(0)->setPen(qpt_att);
        QPen qpt_med = ui->widget->graph(1)->pen();
        qpt_med.setWidth(1);
        ui->widget->graph(1)->setPen(qpt_med);
        startround();
        on_checkBox_6_clicked();
        ui->checkBox_6->setChecked(true);
    }
    else
    {
        if (attent_modulaion)
            on_radioButton_4_clicked();
        else
            on_radioButton_5_clicked();
        on_checkBox_6_clicked();
        ui->checkBox_6->setChecked(false);
    }
}

void paintform::on_checkBox_14_clicked()
// mode for drawing by contours: firstly line is drawn by mouse, then it fills by brain waves amplitude
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

void paintform::turn_music_checkbox(bool fl)
{
    ui->checkBox_15->setEnabled(fl);
}

void paintform::on_checkBox_15_clicked()    // music activation (from MindPlay) by border
{
    if (musicactiv)
    {
        musicactiv=false;       
        pw->musicmode_on=false;
        mypen.setWidth(1);
        ui->widget->graph(2)->setPen(mypen);
    }
    else
        musicactiv=true;
    ui->checkBox_22->setChecked(musicactiv);
    pw->turn_music_checkbox(musicactiv);  
    ui->verticalSlider_2->setEnabled(musicactiv);
}

void paintform::on_spinBox_5_valueChanged(int arg1)
{
    soundborderlevel=arg1;
    ui->verticalSlider_2->setValue(arg1);
    if (game_gothrough)
        gamethroughborder=arg1;
}

void paintform::on_verticalSlider_2_sliderMoved(int position)
{
    ui->spinBox_5->setValue(position);
    soundborderlevel=position;
}

void paintform::setsoundtype(int index)
// update sound samples set label in combobox (invoked from MindPlay)
{
    if (index==0)
        ui->comboBox_2->setCurrentIndex(0);
    else if (index==1)
        ui->comboBox_2->setCurrentIndex(1);
    else
        ui->comboBox_2->setCurrentIndex(2);
}

void paintform::on_comboBox_2_currentIndexChanged(int index)
// change sound samples set (tankdrum1 / tankdrum2 / spacedrum)
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

void paintform::on_pushButton_9_clicked()   // transfer main pic to MindPlay and switch to MindPlay
{
    if (puzzlemode)    
        pixMap = mainpic;        
    else
    if (!scene->spacedview)
        pixMap = ui->graphicsView->grab();            
    else    
        pixMap = scene->bkgndimg; 
    pw->setbackimage(pixMap,true);
    this->hide();
    pw->show();
    pw->setFocus();
}

void paintform::on_spinBox_6_valueChanged(int arg1)
{
    borderpicchange=arg1;
}

void paintform::on_checkBox_16_clicked() // activate puzzle flow (puzzle gathering)
{
    if (!puzzlemode)
    {
        scene->addPixmap(mainpic.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
        grabpuzzles();
        randompics();
    }

    if (puzzlegrabed)
    {
        if (flowmode)
        {
            flowmode=false;                    
            if (fixedmain)
                ui->horizontalSlider_2->setVisible(false);
        }
        else
        {
            flowmode=true;
            ui->horizontalSlider_2->setValue(80);
            ui->horizontalSlider_2->setVisible(true);
            random_shuffle(puzzlelocs.begin(), puzzlelocs.end());
        }

        ui->checkBox_8->setEnabled(!flowmode);
        ui->checkBox_8->setChecked(flowmode);
        ui->checkBox_20->setEnabled(!flowmode);
        puzzlemode=flowmode;

        puzzle_onepic_switch();

        changingpics=flowmode;
        ui->checkBox_10->setChecked(flowmode);                       
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
        ui->graphicsView->repaint();
    }         

    // make attention modulated interval in MindPlay
    // to have sound duration synchronized with delay for puzzle change
    pw->attention_interval = flowmode;
    adaptivespinrate(flowmode);
}

void paintform::on_checkBox_17_clicked() // puzzle gathering game
{
    on_checkBox_16_clicked();
    collectiveflow=!collectiveflow;    
    ui->checkBox_23->setEnabled(collectiveflow);
}

void paintform::on_pushButton_10_clicked() // set new pictures folder
{
    QString filePath=QFileDialog::getExistingDirectory(this, "Get Any Folder", folderpath);
    if (filePath!="")
    {
      //  pw->setpicfolder(filePath);
        setpicfolder(filePath);
    }
}

void paintform::on_checkBox_19_clicked() // adaptive border for music activation
{
    music_adaptive_bord=!music_adaptive_bord;
    ui->verticalSlider_2->setEnabled(!music_adaptive_bord);
   // ui->spinBox_5->setEnabled(!music_adaptive_bord);
    if (music_adaptive_bord)
        ui->spinBox_5->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox_5->setStyleSheet("QSpinBox { background-color: white; }");
}

void paintform::on_checkBox_20_clicked()
// spaced mode: main pic is filling as the whole form background image
{    
    scene->spacedview=!scene->spacedview;
    if (iconsready)
    {        
        if (!game_gothrough)
            ui->checkBox_8->setEnabled(!scene->spacedview);
        ui->checkBox_16->setEnabled(!scene->spacedview);
        ui->checkBox_17->setEnabled(!scene->spacedview);
    }
    ui->pushButton_5->setEnabled(!scene->spacedview);
    if (scene->spacedview)
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

void paintform::on_checkBox_21_clicked()  // mode for autonomous drawing (currently removed)
{
    spacedflow=!spacedflow;
    if (spacedflow)
        scene->tim->start();
    else
        scene->tim->stop();
}

void paintform::on_checkBox_18_clicked()    // grab flow from MindPlay
{
    grabmindplayflow=!grabmindplayflow;
    if (grabmindplayflow)
    {
      //  ui->checkBox_8->setEnabled(false);
        ui->checkBox_13->setEnabled(false);
        ui->checkBox_16->setEnabled(false);
        ui->checkBox_17->setEnabled(false);
    } else
    {
        if (iconsready)
        {
          //  ui->checkBox_8->setEnabled(true);
            if (puzzlemode)
                ui->checkBox_13->setEnabled(true);
            ui->checkBox_16->setEnabled(true);
            ui->checkBox_17->setEnabled(true);
        }
    }
}

void paintform::on_pushButton_11_clicked() // load empty white main image
{
    pmain.load(":/pics/pics/empty.jpg");
    mainpic.load(":/pics/pics/empty.jpg");
    qim.load(":/pics/pics/empty.jpg");
    scene->addPixmap(pmain.scaled(ui->graphicsView->width(),ui->graphicsView->height(),rationmode,Qt::SmoothTransformation));
}

void paintform::on_pushButton_12_clicked()  // transfer current main pic to MindOCV
{
    if (mww->opencvstart)
    {
        if (puzzlemode)
            pixMap = mainpic;
        else if (!scene->spacedview)
            pixMap = ui->graphicsView->grab();
        else
            pixMap = scene->bkgndimg;
        mww->setdstfromplay(pixMap.toImage());
    }
}

void paintform::on_spinBox_7_valueChanged(int arg1) // change length of interval in MindPlay
{
    pw->updateimlength(arg1);
}

void paintform::on_checkBox_22_clicked()
{
    pw->setmusicmode(ui->checkBox_22->isChecked());
}

void paintform::on_checkBox_23_clicked()    // start/stop "go through" game mode
{
    game_gothrough = !game_gothrough;
    if (!scene->spacedview)
        ui->checkBox_8->setEnabled(!game_gothrough);
    ui->checkBox_17->setEnabled(!game_gothrough);
    if (game_gothrough)
    {
        movingItem->setX(movingItem->startX);
        movingItem->setY(movingItem->startY);
        itemborders.clear();
        scenegame->removeItem(movingItem);
        scenegame->clear();
        scenegame->addItem(movingItem);
        borderlines.clear();
        initborderlines();
        soundborderlevel = gamethroughborder;
        ui->spinBox_5->setValue(soundborderlevel);
        ui->checkBox_15->setText("go through border");
        ui->graphicsView->setVisible(false);
        ui->graphicsView_16->setVisible(true);
        moveItemTimer->start();
    }
    else
    {
        ui->graphicsView->setVisible(true);
        ui->graphicsView_16->setVisible(false);
        ui->checkBox_15->setText("music activation border");
        moveItemTimer->stop();
    }
}

void paintform::on_comboBox_3_currentIndexChanged(int index)  // attention / meditation switch
{
    if (index==0)
    {
        on_radioButton_4_clicked();
        ui->radioButton_4->setChecked(true);
    }
    else if (index==1)
    {
        on_radioButton_5_clicked();
        ui->radioButton_5->setChecked(true);
    }
}

void paintform::update_penamp(int p)
{
    ui->spinBox->setValue(p);
}

