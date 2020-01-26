#include "mainwindow.h"
#include "plotwindow.h"
#include "ui_mainwindow.h"
#include "ui_plotwindow.h"
#include <QtGui>
#include <iostream>
#include <QMainWindow>
#include <QScrollArea>
#include <qdebug.h>
#include <hilbert.h>
#include "appconnect.h"
#include <QFileDialog>
#include <settings.h>
#include "paintform.h"
#include "thinkgear.h"
#include <bits/stdc++.h>
#include <windows.h>
#include <random>
#include <chrono>
#include "QThread"
#include "QTimer"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/types.hpp>
#include <thread>


QStringList strList1;
QStringListModel *strListM1;

using namespace cv;
using std::cout; using std::cerr; using std::endl;

void Processing();
void ProcessingMix();
void Hue( int, void* );
void Saturation( int, void* );
void Value( int, void* );
void Attent( int, void* );
void Overlay( int, void* );
void Border( int, void* );
void onMouse( int event, int x, int y, int, void* );

void defineiconsarr();
void define_riconsarr();
void shuffleicons(bool left);

Mat src,srccopy,dst,dstcopy,img,image,tempimg,dstemp;

int currmainpic, curroverpic, prevmainpic = -1, prevoverpic = -1;
vector <int> iconsarr, riconsarr;
bool firstrun = true;
bool estattention = false;

leftpanel *leftpw;
rightpanel *rightpw;
bool rchanged, lchanged;
QStringList imglist;
QString folderpath;

int elem1 = 255;
int elem2 = 255;
int elem3 = 255;
int elem4 = 50;
int elem5 = 50;
int elem6 = 90;

bool canchangepic = true;
double alphaval = 0.5;

int const max_elem = 500;
int const max_elem2 = 100;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{     
    setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);
    plotw = new plotwindow();
    plotw->setWindowTitle("MindDrawPlay beta 2.24 | MindPlay");
    paintw = new paintform();
    paintw->pw=plotw;
    paintw->mww=this;
    paintw->setFixedSize(1600,970);

    plotw->setFixedSize(1600,978);
    plotw->move(QApplication::desktop()->screen()->rect().center() - plotw->rect().center()-QPoint(10,30));
    plotw->start=false;  

    leftpw = new leftpanel();
    leftpw->mww = this;
    leftpw->setFixedSize(148,1030);
    leftpw->move(QPoint(0,0));

    rightpw = new rightpanel();
    rightpw->mww = this;
    rightpw->setFixedSize(148,1030);
    rightpw->move(QPoint(1769,0));

    folderpath="D:/PICS";
    QDir fd(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    leftpw->imgnumber = imglist.length()-2;
    rightpw->imgnumber = imglist.length()-2;

    picsarr = vector<int>(imglist.length());

    ui->lineEdit->setText(folderpath);

    pwstart=false;
    bciconnect=false;
    opencvstart = false; 
    connectWin = new appconnect();
    connectWin->wd=plotw;
    connectWin->mw=this;
    psstart=false;
    packetsRead=0;
    ui->label_2->setVisible(false);
    strListM1 = new QStringListModel();
    strListM1->setStringList(strList1);
    ui->listView->setModel(strListM1);
    ui->listView->show();
    ui->listView->setAutoScroll(true);
    //ui->listView->scrollBarWidgets(Qt::AlignRight);
    mindwt = new QTimer(this);
    mindwt->connect(mindwt, SIGNAL(timeout()), this, SLOT(mindwtUpdate()));
    mindwt->setInterval(0);

    srfr = 500;
    deltaphs = 4; thetaphs = 8; alphaphs = 0; betaphs = 7; gammaphs = 8;
    deltafr = 2; thetafr = 5; alphafr = 9; betafr = 21; gammafr=33; hgammafr=64;
    zdeltaamp = 7; zthetaamp = 7; zalphaamp = 7; zbetaamp = 7; zgammaamp = 5; zhgammaamp = 3;

    canchangehue=true;
    canchangeoverlay=true;
    curhue = prevhue = 255;
    curoverl = prevoverl = 50;

    currentel = 0; currentsimdata = 0;
    simulateEEG = new QTimer(this);
    simulateEEG->connect(simulateEEG,SIGNAL(timeout()), this, SLOT(simulateEEGUpdate()));
    simulateEEG->setInterval(2);
  //  simulateEEG->start();

    fullscr=false;
    opencvinterval=20;
    picfilt = new QTimer(this);
    picfilt->connect(picfilt,SIGNAL(timeout()), this, SLOT(picfiltUpdate()));
    picfilt->setInterval(opencvinterval);    

  //  QThread* tr = new QThread();
  //  picfilt->moveToThread(tr);
  //  tr->start();
  //  tr->setPriority(QThread::HighestPriority);

    simeeg = true; plotw->simeeg = true;

    connect(ui->actionOpen_Data_File, SIGNAL(triggered()), this, SLOT(OpenDataFile()));

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    rs = new rawsignal();
    rs->setGeometry(0,0,1600,80);
    rs->move(158,0);

    plotw->rws=rs;    

    ui->label->setVisible(false); 

    makeicons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setopencvt(int i)
{
    opencvinterval=i;
    picfilt->setInterval(opencvinterval);
}

void Hue(int, void *)
{
    Processing();
}

void Saturation(int, void *)
{
    Processing();
}

void Value(int, void *)
{
    Processing();
}

void Border(int, void *)
{
  //  Processing();
}

void Overlay( int, void* )
{
    ProcessingMix();
}

void Attent( int, void* )
{
   // Processing();
}

QImage Mat2QImage(cv::Mat const& srct)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(srct, temp,COLOR_HSV2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     return dest;
}

QImage Mat2QImageRGB(cv::Mat const& srct)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(srct,temp,COLOR_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     return dest;
}

void Processing()
{
    cvtColor(src,img,COLOR_RGB2HSV);

    int hue = elem1 - 255;
    int saturation = elem2 - 255;
    int value = elem3 - 255;

    for(int y=0; y<img.cols; y++)
    {
        for(int x=0; x<img.rows; x++)
        {
            int cur1 = img.at<Vec3b>(Point(y,x))[0];
            int cur2 = img.at<Vec3b>(Point(y,x))[1];
            int cur3 = img.at<Vec3b>(Point(y,x))[2];
            cur1 += hue;
            cur2 += saturation;
            cur3 += value;

            if(cur1 < 0) cur1= 0; else if(cur1 > 255) cur1 = 255;
            if(cur2 < 0) cur2= 0; else if(cur2 > 255) cur2 = 255;
            if(cur3 < 0) cur3= 0; else if(cur3 > 255) cur3 = 255;

            img.at<Vec3b>(Point(y,x))[0] = cur1;
            img.at<Vec3b>(Point(y,x))[1] = cur2;
            img.at<Vec3b>(Point(y,x))[2] = cur3;
        }
    }

    cvtColor(img,image,COLOR_HSV2RGB);

    addWeighted(image, alphaval, srccopy, 1 - alphaval, 0, dst);
    imshow("image", dst);

    // imshow( "image", image );

}

void ProcessingMix()
{
    alphaval = (double) elem5 / 100;
    if (estattention)
        addWeighted(image, alphaval, srccopy, 1 - alphaval, 0, dst);
    else
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
    imshow("image", dst);
}

void onMouse( int event, int x, int y, int, void* )
{
    if (event == EVENT_MBUTTONDOWN)
    {
        prevmainpic = currmainpic;
        currmainpic = iconsarr[qrand() % (imglist.length()-2)];
        lchanged=true;
        rchanged=false;
        defineiconsarr();
        define_riconsarr();
        leftpw->fillpics();    
        rightpw->fillpics();
        QString ocvpic=folderpath+"/"+imglist.at(currmainpic);
        src = imread(ocvpic.toStdString());
        cv::resize(srccopy, srccopy, cv::Size(src.cols,src.rows), 0, 0, cv::INTER_LINEAR);
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("image", dst);
    } else
    if (event == EVENT_RBUTTONDOWN)
    {

        prevoverpic = curroverpic;
        curroverpic = riconsarr[qrand() % (imglist.length()-2)];;
        lchanged=false;
        rchanged=true;
        define_riconsarr();
        defineiconsarr();
        rightpw->fillpics();
        leftpw->fillpics();
        QString ocvpic=folderpath+"/"+imglist.at(curroverpic);
        srccopy = imread(ocvpic.toStdString());
        cv::resize(srccopy, srccopy, cv::Size(src.cols,src.rows), 0, 0, cv::INTER_LINEAR);
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("image", dst);
    }
}

void delay(int temp)
{
    QTime dieTime = QTime::currentTime().addMSecs(temp);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void defineiconsarr()
{
    vector <int> temparr;
    int tp;
    if (firstrun)
        tp = imglist.length();
    else
        tp = imglist.length()-2;
    if ((!firstrun) && (prevmainpic==-1))
        tp = imglist.length()-1;
    for (int i=0; i<tp; i++)
    {
        if ((firstrun) && (i!=currmainpic))
            iconsarr.push_back(i);
        else if ((!firstrun) && (iconsarr[i]!=currmainpic) && ((iconsarr[i]!=curroverpic)))
            temparr.push_back(iconsarr[i]);
    }    
    if (prevmainpic>-1)
    {
        if (lchanged)
            temparr.push_back(prevmainpic);
    }
    if ((!lchanged) && (prevoverpic>-1))
        temparr.push_back(prevoverpic);
    if (!firstrun)
        iconsarr=temparr;
}

void define_riconsarr()
{
    vector <int> rtemparr;
    int tp;
    if (firstrun)
        tp = imglist.length();
    else
        tp = imglist.length()-2;
    for (int i=0; i<tp; i++)
    {
        if ((firstrun) && (i!=currmainpic) && (i!=curroverpic))
            riconsarr.push_back(i);
        else if ((!firstrun) && (riconsarr[i]!=currmainpic) && ((riconsarr[i]!=curroverpic)))
            rtemparr.push_back(riconsarr[i]);
    }
    if (prevoverpic>-1)
    {
        if ((rchanged) && (!lchanged))
            rtemparr.push_back(prevoverpic);
        if ((rchanged) && (lchanged))
            rtemparr.push_back(prevmainpic);
    }
    if ((!rchanged) && (prevmainpic>-1))
        rtemparr.push_back(prevmainpic);
    if (!firstrun)
        riconsarr=rtemparr;
}

void shuffleicons(bool left)
{
    if (left)
        random_shuffle(iconsarr.begin(), iconsarr.end());
    else
        random_shuffle(riconsarr.begin(), riconsarr.end());
}

int MainWindow::geticon(int t, bool left)
{
    if (left)
        return iconsarr[t];
    else
        return riconsarr[t];
}

int MainWindow::getmainpic()
{
    return currmainpic;
}

int MainWindow::getoverpic()
{
    return curroverpic;
}

void MainWindow::updatemainpic(int num)
{           
    prevmainpic = currmainpic;
    currmainpic = num;
    lchanged=true;
    rchanged=false;
    defineiconsarr();
    define_riconsarr();
    leftpw->fillpics();
    rightpw->fillpics();
    QString ocvpic=folderpath+"/"+imglist.at(currmainpic);
    src = imread(ocvpic.toStdString());
    cv::resize(srccopy, srccopy, cv::Size(src.cols,src.rows), 0, 0, cv::INTER_LINEAR);
    addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
    imshow("image", dst);
    if (plotw->start)
        plotw->grabopencv(ocvpic);
}

void MainWindow::updateoverpic(int num)
{
    prevoverpic = curroverpic;
    curroverpic = num;
    lchanged=false;
    rchanged=true;
    define_riconsarr();
    defineiconsarr();
    rightpw->fillpics();
    leftpw->fillpics();
    QString ocvpic=folderpath+"/"+imglist.at(curroverpic);
    srccopy = imread(ocvpic.toStdString());
    cv::resize(srccopy, srccopy, cv::Size(src.cols,src.rows), 0, 0, cv::INTER_LINEAR);
    addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
    imshow("image", dst);
}

void MainWindow::adddata(string s, QString spath)
{
    ui->label->setText(QString::fromStdString(s));
    plotw->hnt=ht;
    for (int i=0; i<ht->npt; i++)
    {
        plotw->arrc.xc[i] = i;
     //   plotw->arrc.amp1[i]=ht->x[i];
    }
    ui->label_2->setText("Data File:  " + spath);  
}

void MainWindow::printdata(QString str)
{   
    strList1.push_front(str);
    strListM1->setStringList(strList1);
}

void MainWindow::on_pushButton_clicked()
{
    TG_FreeConnection( connectionId );
  //  plotw->cleanmem();
    this->close();    
    QApplication::quit();
}

void MainWindow::on_pushButton_3_clicked()
{
  //  if (plotw->start)
 //      plotw->move(QApplication::desktop()->screen()->rect().center() - plotw->rect().center()+QPoint(0,10));

    plotw->show();
    plotw->doplot();    
    plotw->appcn=connectWin;
    paintw->scene->init(plotw, this);
    connectWin->ps=paintw->scene;

    paintw->scene->init(plotw, this);
    paintw->move(QApplication::desktop()->screen()->rect().center() - paintw->rect().center()+QPoint(-10,-35));
    plotw->pssstart=true;
    paintw->scene->clear();
    paintw->show();
    paintw->loadempty();
   // paintw->startpolyt();
    connectWin->ps=paintw->scene;
    if (plotw->start)
        plotw->pss=paintw->scene;

    paintw->hide();
    plotw->setFocus();
}

void MainWindow::on_pushButton_2_clicked()
{          
    bool ok1,ok2;
    stringstream str1, str2;
    str1 << "Enter sampling rate: ";
    int srate = QInputDialog::getInt(this,"Sampling rate",str1.str().c_str(), 512, 1, 1024, 1, &ok1);
    if (ok1)
        plotw->hnt->srfr=srate;
    str2 << "Enter EEG source channel number: ";
    int channel = QInputDialog::getInt(this,"Channel number",str2.str().c_str(), 1, 1, 64, 1, &ok2);
    if (ok2)
    {        
        connectWin->chnum=channel-1;
        plotw->setWindowTitle(plotw->windowTitle()+"    Online EEG source channel: "+QString::number(channel));
        plotw->sourcech=channel-1;
        connectWin->clearstates();
        connectWin->show();
        connectWin->connectButCallback();
        if (connectWin->ready)
        {
            printdata("Connection with BCI2000 established! :)");
            plotw->hnt->srfr=512;
            ui->pushButton_5->setEnabled(false);
            ui->label->setText("Channel number: "+QString::number(channel));
            ui->label->setVisible(true);
            bciconnect = true;
            ui->pushButton_4->setEnabled(false);            
        } else
            printdata("Connection with BCI2000 not established.. ( ");
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    on_pushButton_3_clicked();
    //plotw->hide();
    paintw->scene->init(plotw, this);
  //  paintw->move(QApplication::desktop()->screen()->rect().center() - paintw->rect().center()+QPoint(0,-55));
    plotw->pssstart=true;
    paintw->scene->clear();
    paintw->show();
    paintw->loadempty();
   // paintw->startpolyt();
    if (!bciconnect)
    {
        rs->starting();
        rs->show();
    }
    connectWin->ps=paintw->scene;
    if (plotw->start)    
        plotw->pss=paintw->scene;    
}

void MainWindow::OpenDataFile()
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),"F://","Data file (*.dat);;All files (*.*)");
    if (filename!="")
    {
        if (plotw->start)
            plotw->close();
        ht->firstinit(filename,NMAX);
        string s = "EEG channel: " + ht->channel;
        strList1.clear();
        strListM1->setStringList(strList1);
        plotw = new plotwindow();
        plotw->setFixedSize(1450,785);
        plotw->start=false;        
        connectWin->wd=plotw;
        plotw->appcn=connectWin;           
        adddata(s,filename);
        plotw->show();
        plotw->doplot();
    }
}

void MainWindow::mindwaveconnect()
{
    char *comPortName = NULL;
    char *portNumber = (char*)malloc(sizeof(char) * (2 + 1));
    const char *comPortBase = NULL;
    int   errCode = 0;
    int comPortFound = 0;
    const int MAX_PORT = 16;
    size_t length = 0;
    int i = 0;
    int j = 0;

    int dllVersion = TG_GetDriverVersion();
    printdata("ThinkGear DLL version: "+QString::number(dllVersion));
    connectionId = TG_GetNewConnectionId();
    if( connectionId < 0 )
        printdata("ERROR: TG_GetNewConnectionId() returned: "+QString::number(connectionId));
    else
        printdata("ThinkGear Connection ID is: "+QString::number(connectionId));

    printdata("Scanning COM ports 0 to "+QString::number(MAX_PORT));
    comPortBase = "\\\\.\\COM";
    length = strlen(comPortBase);
    comPortName = (char *)realloc (comPortName, (length + 5)*sizeof(char));

    for(i=0; i <= MAX_PORT && comPortFound == 0; i++)
    {

        portNumber = itoa(i, portNumber, 10);
        strcpy(comPortName,comPortBase);

        for(j=0; j<(int)strlen(portNumber); j++)
            comPortName[length+j] = portNumber[j];

        comPortName[length+strlen(portNumber)] = '\0';

        printdata("trying to connect on "+QString::fromUtf8(comPortName));
        errCode = TG_Connect( connectionId,comPortName,TG_BAUD_9600,TG_STREAM_PACKETS );
        if( errCode < 0 )
            printdata("ERROR: TG_Connect() returned");
        else
        {
            // Trying to read one packet to check the connection.
            printdata("Connection available... wait...");
            Sleep(3000); // sometimes we need to wait a little...
            errCode = TG_ReadPackets(connectionId, 1);
            if(errCode >= 0)
            {
                printdata("OK");
                comPortFound = 1;
                break;
            }
        }
    } 	/* end: "Attempt to connect the connection ID handle to serial ports between COM0 and "COM16"" */

    if (comPortFound == 1)
    {
        printdata("Connection with MindWave established! :)");
        ui->pushButton_2->setEnabled(false);
        simulateEEG->stop();
        simeeg=false;
        plotw->hnt->srfr=512;
        plotw->hnt->imlength=256;
        mindwt->start();
        plotw->mindwstart=true;
        plotw->simeeg=false;
        rs->starting();
        ui->pushButton_5->setEnabled(false);
        if (!estattention)
            estattention=true;
        rs->show();
    }
    else
        printdata("Connection with MindWave could not be established! :(");
    packetsRead = 0;
}

void MainWindow::setsourceimg(QString fpath)
{
    src = imread(fpath.toStdString());
    imshow("image", src );
}

void MainWindow::setsourceimgd(QImage qp)
{
    cv::Mat matp(qp.height(),qp.width(),CV_8UC3, qp.bits());
    //src = matp;
    imshow("image", src );
}

void MainWindow::sethue(int i)
{
    elem1=i*3;
    setTrackbarPos("Hue", "image", elem1);
}

void MainWindow::setattent(int i)
{
    elem4=i;
    setTrackbarPos("Attention","image",elem4);
}

void MainWindow::setoverlay(int i)
{
    elem5=i;
    setattent(elem5);
    setTrackbarPos("Overlay","image",elem5);
    checkoverlay();
}

QImage MainWindow::grabopcvpic()
{
     return Mat2QImageRGB(dst);
   // cvtColor(dst,dstcopy,COLOR_RGB2HSV);
  //  return Mat2QImage(dstcopy);
}

void MainWindow::startopencv()
{
    if (!opencvstart)
    {
        namedWindow("image",WINDOW_NORMAL + WINDOW_OPENGL);

        createTrackbar("Hue","image",&elem1, max_elem,Hue);
        // createTrackbar("Saturation","image",&elem2, max_elem,Saturation);
        // createTrackbar("Value","image",&elem3, max_elem,Value);
        createTrackbar("Attention","image",&elem4,max_elem2,Attent);
        createTrackbar("Overlay","image",&elem5,max_elem2,Overlay);
        createTrackbar("Border","image",&elem6,max_elem2,Border);

        setMouseCallback( "image", onMouse, 0 );

        currmainpic = qrand() % imglist.length();        
        lchanged=false;
        rchanged=false;
        defineiconsarr();
        opencvpic = folderpath+"/"+imglist.at(currmainpic);
        src = imread(opencvpic.toStdString());
        image = imread(opencvpic.toStdString());


        curroverpic = iconsarr[qrand() % (imglist.length()-1)];
        define_riconsarr();
        QString stp = folderpath+"/"+imglist.at(curroverpic);
        srccopy =  imread(stp.toStdString());

        firstrun=false;
        defineiconsarr();
        resizeWindow("image",1600,900);
        resize(1600,900);
        moveWindow("image", 150,39);
        setTrackbarPos("Overlay", "image", elem5);
        imshow("image", src);

        opencvstart=true;
        plotw->opencvstart=true;
        if (plotw->start)
            plotw->enablehue();
        picfilt->start();

    } else
        imshow("image", src);
}

void MainWindow::simulateEEGUpdate()
{
    const double mean = 0.0;
    const double stddev = 0.5;
   // std::default_random_engine generator;
   // std::normal_distribution<double> dist(mean, stddev);
  //  double nois = 5*dist(generator);
    int nois = -25 + rand() % 50;
    deltaamp = zdeltaamp - 10 + rand() % 20;
    thetaamp = zthetaamp - 12 + rand() % 25;
    alphaamp = zalphaamp - 5 + rand() % 10;
    betaamp = zbetaamp - 7 + rand() % 15;
    gammaamp = zgammaamp - 3 + rand() % 6;
    hgammaamp = zhgammaamp - 2 + rand() % 4;
    if (currentel>50)
    {
        deltaphs = rand() % 20;
        thetaphs = rand() % 20;
        alphaphs = rand() % 20;
        betaphs = rand() % 20;
        gammaphs = rand() % 20;
    }
    currentel++;
    if (currentel>100)
        currentel=0;
    currentsimdata = deltaamp*sin(deltafr*2*PI/srfr*(currentel+deltaphs)) + thetaamp*sin(thetafr*2*PI/srfr*(currentel+thetaphs)) + alphaamp*sin(alphafr*2*PI/srfr*(currentel+alphaphs)) + betaamp*sin(betafr*2*PI/srfr*(currentel+betaphs)) + gammaamp*sin(gammafr*2*PI/srfr*(currentel+gammaphs)) + hgammaamp*sin(hgammafr*2*PI/srfr*(currentel+gammaphs)) + nois;
    currentsimdata *= 3;
    if (plotw->start)
        plotw->bcidata(currentsimdata);
    if (psstart)
        paintw->scene->getdata(currentsimdata/4);
    //  qDebug()<<currentsimdata;       
}

void MainWindow::checkoverlay()
{
    if ((elem5>elem6) && (canchangepic))
    {
        prevmainpic = currmainpic;
        currmainpic = curroverpic;
        prevoverpic = curroverpic;
        curroverpic = iconsarr[qrand() % (imglist.length()-2)];
        lchanged=true;
        rchanged=true;
        defineiconsarr();
        define_riconsarr();
        leftpw->fillpics();
        rightpw->fillpics();
       // leftpw->updateplaypic();
        QString ocvpic=folderpath+"/"+imglist.at(curroverpic);
        src=srccopy.clone();
        srccopy = imread(ocvpic.toStdString());
        cv::resize(srccopy, srccopy, cv::Size(src.cols,src.rows), 0, 0, cv::INTER_LINEAR);
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("image", dst);
        canchangepic=false;
    }
    if ((elem5<elem6) && (!canchangepic))
        canchangepic=true;
}

void MainWindow::makeicons()
{
    QString picst;
    imgarray.resize(imglist.length());
    for (int i=0; i<imglist.length(); i++)
    {
        picst = folderpath+"/"+imglist.at(i);
        tempimg = imread(picst.toStdString());
        Size size(138,138);
        cv::resize(tempimg,dstemp,size);
        QImage qm = Mat2QImageRGB(dstemp);
        imgarray[i]=QPixmap::fromImage(qm);
    }

}

void MainWindow::shuffleiconss(bool left)
{
    shuffleicons(left);
}

void MainWindow::setfolderpath(QString fp)
{
    folderpath=fp;
    QDir fd(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    ui->lineEdit->setText(folderpath);
}

void MainWindow::mindwtUpdate()
{
    int c=0;
    int errCode = TG_ReadPackets( connectionId,1);
    if( errCode == 1 )
    {
       /* packetsRead++; // if get frequencies from device, only once per second
        if (packetsRead==512)
        {
           // allpower = mw_delta + mw_theta + mw_alpha1 + mw_alpha2 + mw_beta1 + mw_beta2 + mw_gamma1 + mw_gamma2;
            allpower = mw_delta + mw_theta + mw_alpha + mw_beta + mw_gamma1;// + mw_gamma2;
            p_delta = (double)mw_delta/allpower;
            p_theta = (double)mw_theta/allpower;
            p_alpha = (double)mw_alpha/allpower;
            p_beta = (double)mw_beta/allpower;
            p_gamma1 = (double)mw_gamma1/allpower;
          //  p_gamma2 = (double)mw_gamma2/allpower;
          //  if (psstart)
          //      paintw->updatefreqarrs(100*p_delta,100*p_theta,100*p_alpha,100*p_alpha,100*p_gamma1);
          //      paintw->updatefreqarrs(p_delta,p_theta,p_alpha,p_beta,p_gamma1);
          /*  plotw->delta = 100*p_delta;
            plotw->theta = 100*p_theta;
            plotw->alpha = 100*p_alpha;
            plotw->beta = 100*p_beta;
            plotw->gamma = 100*p_gamma1;
            plotw->hgamma = 100*p_gamma2;
            if (psstart)
                paintw->updatefreqarrs(100*p_delta,100*p_theta,100*p_alpha,100*p_beta,100*p_gamma1);
            cout<<fixed<<setprecision(2)<<"Raw: "<<mw_raw<<"  Delta: "<<(double)mw_delta/allpower<<"  Theta: "<<(double)mw_theta/allpower<<"  Alpha1: "<<(double)mw_alpha1/allpower<<"  Alpha2: "<<(double)mw_alpha2/allpower<<"  Beta1: "<<(double)mw_beta1/allpower<<"  Beta2: "<<(double)mw_beta2/allpower<<"  Gamma1: "<<(double)mw_gamma1/allpower<<"  Gamma2: "<<(double)mw_gamma2/allpower<<endl;
           // cout<<fixed<<setprecision(2)<<"Raw: "<<mw_raw<<"  Theta: "<<p_theta<<"  Alpha: "<<p_alpha<<"  Beta: "<<p_beta<<"  Gamma: "<<p_gamma1<<"  HGamma: "<<p_gamma2<<endl;
            packetsRead=0;
        } */
        plotw->chnums=1;
        if ( TG_GetValueStatus(connectionId, TG_DATA_RAW) != 0 )
        {
            mw_raw = TG_GetValue(connectionId, TG_DATA_RAW);
            if (abs(mw_raw)<180)
            if (plotw->start)
                plotw->bcidata(mw_raw/2);
            if (rs->start)
                rs->updatesignal(mw_raw/2);
            if (abs(mw_raw)<100) // anti blink artifacts for drawing
            if (psstart)
                paintw->scene->getdata(mw_raw/2);
                //cout<<"RAW value: "<<mw_raw<<endl;
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_ATTENTION) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_ATTENTION)>0)
            {
                mw_atten=TG_GetValue(connectionId, TG_DATA_ATTENTION);
                if (plotw->start)
                    plotw->update_attention(mw_atten);
                if (opencvstart)
                {                   
                   // setoverlay(mw_atten);
                   //   setattent(mw_atten);
                   // elem2 = 210 + mw_atten/2;
                   // setTrackbarPos("Saturation", "image", elem2);
                    if (canchangehue)
                    {
                        curhue=100+mw_atten*4;
                        canchangehue=false;
                    //  setopencvt(50+mw_atten*2);
                    }
               //     if (canchangeoverlay)
               //     {
               //         curoverl=mw_atten;
               //         canchangeoverlay=false;
                    //  setopencvt(50+mw_atten*2);
                  //  }
                }
                if (psstart)
                {
                    paintw->updateattentionplot(mw_atten);
                    if (opencvstart)
                    {
                        setattent(paintw->getestattval());                       
                    //    if (canchangeoverlay)
                        {
                            curoverl=elem4;
                      //      canchangeoverlay=false;
                        }
                    }
                    //paintw->updateplots(false);
                    //paintw->bfiltmode;
                  //      paintw->randompics();
                }
                // cout<<"Attention value: "<<mw_atten<<endl;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_MEDITATION) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_MEDITATION)>0)
            {
                mw_medit=TG_GetValue(connectionId, TG_DATA_MEDITATION);                
                if (plotw->start)
                    plotw->update_meditation(mw_medit);
                if (psstart)
                {
                    paintw->updatemeditation(mw_medit);
                //    paintw->updateplots(false);
                }
                // cout<<"Meditation value: "<<mw_medit<<endl;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_ALPHA1) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_ALPHA1)>0)
            {
                mw_alpha1 = TG_GetValue(connectionId, TG_DATA_ALPHA1);
            //    qDebug()<<mw_alpha1;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_ALPHA2) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_ALPHA2)>0)
            {
                mw_alpha2 = TG_GetValue(connectionId, TG_DATA_ALPHA2);
            //    qDebug()<<alpha2;
            }
        }
        mw_beta = (mw_beta1 + mw_beta2) / 2;
        if ( TG_GetValueStatus(connectionId, TG_DATA_BETA1) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_BETA1)>0)
            {
                mw_beta1 = TG_GetValue(connectionId, TG_DATA_BETA1);
            //    qDebug()<<mw_beta1;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_BETA2) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_BETA2)>0)
            {
                mw_beta2 = TG_GetValue(connectionId, TG_DATA_BETA2);
            //    qDebug()<<alpha1;
            }
        }
        mw_alpha = (mw_alpha1 + mw_alpha2) / 2;
        if ( TG_GetValueStatus(connectionId, TG_DATA_DELTA) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_DELTA)>0)
            {
                mw_delta = TG_GetValue(connectionId, TG_DATA_DELTA);
            //    qDebug()<<alpha1;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_THETA) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_THETA)>0)
            {
                mw_theta = TG_GetValue(connectionId, TG_DATA_THETA);
            //    qDebug()<<alpha1;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_GAMMA1) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_GAMMA1)>0)
            {
                mw_gamma1 = TG_GetValue(connectionId, TG_DATA_GAMMA1);
            //    qDebug()<<alpha1;
            }
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_GAMMA2) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_GAMMA2)>0)
            {
                mw_gamma2 = TG_GetValue(connectionId, TG_DATA_GAMMA2);
            //    qDebug()<<alpha1;
            }
        }       
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    mindwaveconnect();
}

void MainWindow::picfiltUpdate()
{    
    char key = cv::waitKey(10);
    if (key == ' ')
    {
        if (!fullscr)
        {            
            rs->setGeometry(0,0,1940,80);
            rs->move(0,0);
            cv::setWindowProperty("image",cv::WND_PROP_FULLSCREEN,1);
            fullscr=true;
        }
        else
        {
            rs->setGeometry(0,0,1600,80);
            rs->move(158,0);
            cv::setWindowProperty("image",cv::WND_PROP_FULLSCREEN,0);
            fullscr=false;
        }
        rs->changefsize(fullscr);
    }
    if (key == 27)
    {
        if (plotw->start)
            plotw->pauseflow();
    }
    if ((abs(curhue-prevhue)==0) || ((abs(curhue-prevhue)==1)))
        canchangehue=true;
    else
    {
        if (curhue<prevhue)
            prevhue-=2;
        else
            prevhue+=2;
        elem1=prevhue;
        setTrackbarPos("Hue", "image", elem1);
    }


    if ((abs(curoverl-prevoverl)==0) || ((abs(curoverl-prevoverl)==1)))
        canchangeoverlay=true;
    else
    {
        if (curoverl<prevoverl)
            prevoverl-=2;
        else
            prevoverl+=2;
        elem5=prevoverl;
        setTrackbarPos("Overlay", "image", elem5);
    }
    checkoverlay();
}

void MainWindow::on_pushButton_6_clicked()
{
    if (imglist.length()>0)
    {

        startopencv();        
        shuffleicons(true);
        leftpw->show();
        leftpw->fillpics();        
        shuffleicons(false);
        rightpw->show();
        rightpw->fillpics();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Choose a folder with .jpg files!");
        msgBox.exec();
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    QString fPath=QFileDialog::getExistingDirectory(this, "Get Any Folder", "D://");
    if (fPath!="")
    {
        setfolderpath(fPath);
        makeicons();
    }
}
