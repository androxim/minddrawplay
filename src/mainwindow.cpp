/* source file for MainWindow class -
   resposible for connecting to EEG device, starting EEG generator,
   starting MindPlay / MindDraw / MindOCV windows, processing most MindOCV actions */

#include "mainwindow.h"
#include "plotwindow.h"
#include "ui_mainwindow.h"
#include "ui_plotwindow.h"
#include <QtGui>
#include <iostream>
#include <QMainWindow>
#include <QScrollArea>
#include <qdebug.h>
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
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <thread>
#include "qmath.h"
#include <QDate>
#include "algorithm"
#include <QtConcurrent>

// TO DO:
// ...

using namespace cv;
using namespace cv::dnn;
typedef std::pair<int,float> pairt;

// ==== openCV functions ====
void Processing();
void ProcessingMix();
void onMouse(int event, int x, int y, int flags, void* );

void dosvdtransform();
void getsvdimage(int r);
vector<float> gethistogram(Mat image, Mat maskx);
void applyfilt(int type, Rect rt);
void fillpolygon(Mat im);
Mat adaptivemask(Rect rt);

Mat dilate(Rect srcRect);               // dilation filter
Mat waves(Rect srcRect);                // waves filter
Mat cartoon(Rect srcRect);              // cartoonize filter
Mat orbdetect(Rect srcRect);            // ORB features detector
Mat mixfilt(Rect srcRect);              // mixer of pics with transparency
Mat ripples(Rect srcRect);              // ripples effect filter
// ==== openCV functions end ====

// ==== openCV variables ====
// different Mat variables, main of them:
// src - main pic on which HUE change applying, chosen from left panel, more clear when attention high
// srccopy - overlay pic, chosen from right panel, more clear when attention low
// dst - resulting overlay pic for area filtering
Mat src, srccopy, dst, dstcopy, prev_dst, clear_dst, img, image, dstg;
Mat edges, mask, trp, randpic, stinp, blob, tempmat, stpic;
Mat tempimg, dstemp, dst0, dst1, srg, srct, srwt, dstt, svd_img, gray_element;
Mat element, dream0, imghist, pichist, label_area, menu_area;
vector<Mat> channels;
vector<Mat> curr_img_set; std::set<int> img_num_set;
int currmainpic, curroverpic, prevmainpic = -1, prevoverpic = -1;
Rect df_srcDstRect;

bool firstrun = true;
bool estattention = false;  // if attention is streaming from MindWave device
bool fullscr = false;
bool mwconnected = false;
bool rawplotshort = true;
bool simulated_eeg = false;

int hue_elem = 255;  // HUE variable
int satur_elem = 255;  // Saturation
int value_elem = 255;  // Value
int atten_elem = 50;   // Attention
int overlay_elem = 50;   // Overlay
int border_elem = 80;   // Border for change of pictures

VideoCapture cam;
vector<vector<float>> hist_features; // vector of histogramm features for all pics
vector<pairt> chi2distances;         // dict of chi2 distances between selected and other pics
bool canchangepic = true;            // to prevent constant change of pics when attention > border => can change pics only when new attention became lower than border    
bool keepfiltering = false;          // additional mode to "!activeflow" with continous filtering with mouse move, activated by "space" press

double alphaval = 0.5; // transparency variable for overlay
int curr_iter = 0; // variable defines how often filter is applied during mousemove
// ==== openCV variables end ====

// ==== SVD variables ====
cv::SVD svdtr;
Mat trimg[3], resimg[3], temptrio[3];
Mat svd_w[3], svd_u[3], svd_vt[3], svd_W[3];
Mat t_W,t_u,t_vt;
int svdt = 80; // number of eigen values for picture compression
// ==== SVD variables end ====

leftpanel *leftpw;          // left panel form for pictures choice
rightpanel *rightpw;        // right panel form for pictures choice
rawsignal *rs;              // raw signal form
ocvcontrols *ocvform;       // openCV filters control form

vector <int> iconsarr, riconsarr;       // vectors of right and left panels pictures
void defineiconsarr();                  // updationg vector of left panel pictures
void define_riconsarr();                // updationg vector of right panel pictures
void shuffleicons(bool left);           // random shuffling of panels pictures
void drawcontrolmenu(Mat imgt, int active_item);  // drawing menu item labels for switching modes
bool rchanged, lchanged;

QString folderpath;     // path to pictures folder
QStringList imglist;    // list of pictures paths
QStringList strList1;
QStringListModel *strListM1;

std::thread thr; // separate thread for making color histogram for images

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{     
    setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);  
    QPalette pbackimg;
    QPixmap pm;
    pm.load(":/pics/pics/mainback.jpg");
    pbackimg.setBrush(QPalette::Background,pm.scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    this->setPalette(pbackimg);
    ui->setupUi(this);

    plotw = new plotwindow();
    plotw->setWindowTitle("MindDrawPlay | MindPlay");
    plotw->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    plotw->setFixedSize(1560,978);
    plotw->move(QApplication::desktop()->screen()->rect().center() - plotw->rect().center()-QPoint(10,30));
    plotw->start=false;

    paintw = new paintform();
    paintw->setWindowTitle("MindDrawPlay | MindDraw");
    paintw->setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    paintw->pw=plotw;
    paintw->mww=this;
    paintw->setFixedSize(1560,970);    
    plotw->paintf = paintw;

    rs = new rawsignal();                // raw signal plot form
    rs->setGeometry(0,0,1600,70);
    rs->move(158,0);
    // rs->starting();
    plotw->rws=rs;

    leftpw = new leftpanel();            // lefp panel for pics choice (main pic)
    leftpw->mww = this;
    leftpw->setFixedSize(148,1030);
    leftpw->move(QPoint(0,0));

    rightpw = new rightpanel();          // right panel for pics choice (overlay pic)
    rightpw->mww = this;
    rightpw->setFixedSize(148,1030);
    rightpw->move(QPoint(1769,0));

    ocvform = new ocvcontrols();         // openCV controls form
    ocvform->mww = this;
    ocvform->setFixedSize(1171,130);
    ocvform->move(QPoint(373,900));
    ocvform->leftpan = leftpw;
    plotw->ocvf = ocvform;
    paintw->ocvfm = ocvform;

    br_levels = new brainlevels();      // mental activity levels form
    br_levels->setFixedSize(701,40);
    br_levels->mww = this;
    br_levels->plw = plotw;
    plotw->brl = br_levels;
    br_levels->move(QPoint(1060,0));

    statsWin = new statistics();
    statsWin->setWindowTitle("MindDrawPlay | Statistics");
    statsWin->setFixedSize(1113,685);
    statsWin->recordrate = (double)plotw->recordwaves_rate/1000;
    statsWin->mww = this;
    stsecond = new statistics();
    stsecond->setWindowTitle("MindDrawPlay | Statistics");
    stsecond->setFixedSize(1113,685);
    stsecond->recordrate = (double)plotw->recordwaves_rate/1000;

    openglw = new openglwin();
    openglw->mww = this;
    openglw->setWindowTitle("MindDrawPlay | MindOGL");
    plotw->oglw = openglw;

    loadFolderpath();
    plotw->folderpath = folderpath;
    QDir fd(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    if (imglist.length()<1)
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a folder with .jpg files!");
        msgBox.exec();
        on_pushButton_7_clicked();
    } else
    {        
        leftpw->imgnumber = imglist.length()-2;     // -2 because excluding current main and overlay pics
        rightpw->imgnumber = imglist.length()-2;
        picsarr = vector<int>(imglist.length());
        ocvform->randpicn = qrand()%imglist.length();
        ui->lineEdit->setText(folderpath);
        for (int i=0; i<imglist.length(); i++)
            imgpaths.push_back(imglist[i]);
    }

    //ocvcontrshow = true;             // if openCV filters control form shown

    pwstart = false;                 // MindPlay window run detector
    paintw_started = false;          // MindDraw window run detector
    opencvstart = false;             // MindOCV window run detector
    storymode = false;               // mode when main pic is fixed, but not overlay
    canchangepuzzle = true;          // when puzzle is completed, to prevent constant pic changes with high attention

    bciconnect = false;              // for data from BCI2000
    connectWin = new appconnect();
    connectWin->wd = plotw;
    connectWin->mw = this;

    packetsRead=0;  // number of packets form MindWave device

    strListM1 = new QStringListModel();
    strListM1->setStringList(strList1);
    ui->listView->setModel(strListM1);
    ui->listView->show();
    ui->listView->setAutoScroll(true);
    ui->label->setVisible(false);
    ui->label_2->setVisible(false);
    ui->pushButton_2->setVisible(false);

    mindwt = new QTimer(this);  // timer for processing data from MindWave device
    mindwt->connect(mindwt, SIGNAL(timeout()), this, SLOT(mindwtUpdate()));
    mindwt->setInterval(1);

    // ==== set of variables for simulated EEG data mode (in development) ====
    srfr = 500; // sampling rate
    deltaphs = 4; thetaphs = 8; alphaphs = 0; betaphs = 7; gammaphs = 8;
    deltafr = 3; thetafr = 5; alphafr = 9; betafr = 21; gammafr=33; hgammafr=44;
    zdeltaamp = 5; zthetaamp = 18; zalphaamp = 21; zbetaamp = 20; zgammaamp = 17; zhgammaamp = 12;
    currentel = 0; currentsimdata = 0;
    simulateEEG = new QTimer(this);
    simulateEEG->connect(simulateEEG,SIGNAL(timeout()), this, SLOT(simulateEEGUpdate()));
    simulateEEG->setInterval(1);    
    simeeg = false; plotw->simeeg = false;
    // ==== set of variables for simulated EEG data mode end ====

    canchangehue = true;  // HUE and Overlay values are changing from previous to new values in cycle
    canchangeoverlay = true;
    curhue = prevhue = 255;
    curoverl = prevoverl = 50;   

    keyprocess = new QTimer(this); // timer for keys press processing in MindOCV
    keyprocess->connect(keyprocess, SIGNAL(timeout()), this, SLOT(keys_processing()));
    keyprocess->setInterval(10);

    opencvinterval = 40;      // timer for color-overlay flow
    picfilt = new QTimer(this);
    picfilt->connect(picfilt, SIGNAL(timeout()), this, SLOT(picfiltUpdate()));
    picfilt->setInterval(opencvinterval);    

    dreamflow_timer = new QTimer(this);     // timer for dreamflow mode, when new picture appears by fragments
    dreamflow_timer->connect(dreamflow_timer, SIGNAL(timeout()), this, SLOT(dreamflow_Update()));
    dreamflow_timer->setInterval(ocvform->dreamflowrate);

    focuseddflow_timer = new QTimer(this);     // timer for focused dreamflow mode, when overlay pic is changing
    focuseddflow_timer->connect(focuseddflow_timer, SIGNAL(timeout()), this, SLOT(focuseddflow_Update()));
    focuseddflow_timer->setInterval(ocvform->focuseddreamflowrate*1000);

    puzzleflow = new QTimer(this); // timer for puzzle gathering flow
    puzzleflow->connect(puzzleflow, SIGNAL(timeout()), this, SLOT(puzzleflow_Update()));
    puzzleflow->setInterval(ocvform->puzzleflowrate);

    int streamflowrate = 100;
    streamflows = new QTimer(this); // timer for streaming flows to MindPlay
    streamflows->connect(streamflows, SIGNAL(timeout()), this, SLOT(streamflows_Update()));
    streamflows->setInterval(streamflowrate);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());          

    // making icons of pics (via QtConcurrent), computing color histogram features (in separate thread)
    makeIconsAndHists();

    // for histogram analysis and flow direction mode
    // connect(this,&MainWindow::histFinished,this,&MainWindow::enablenewfolder);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAboutFile()));
}

// open file with application description
void MainWindow::openAboutFile()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/MDP_about.pdf"));
}


// making icons of pics (via QtConcurrent), computing color histogram features (in separate thread)
void MainWindow::makeIconsAndHists()
{
    int imageSize = 138;

    imgarray.resize(imglist.length());
    imageScaling = new QFutureWatcher<QImage>(this);    // rescaling images, making icons
    connect(imageScaling, &QFutureWatcher<QImage>::resultReadyAt, this, &MainWindow::addScaledImage);
    connect(imageScaling, &QFutureWatcher<QImage>::finished, this, &MainWindow::scalingFinished);

    std::function<QImage(const QString&)> rescale = [imageSize](const QString &imageFileName)
    {
        QString picst = folderpath+"/"+imageFileName;
        Mat tempimg = imread(picst.toStdString());
        Mat dstr, temp;
        Size size(imageSize,imageSize);
        cv::resize(tempimg,dstr,size);
        cvtColor(dstr, temp, COLOR_BGR2RGB);
        QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        dest.bits();
        return dest;
        //  QImage image(picst);  // ~ 2 times slower
        //  return image.scaled(QSize(imageSize, imageSize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    };
   // time_take.start();
    ui->pushButton_7->setEnabled(false);
    imageScaling->setFuture(QtConcurrent::mapped(imglist, rescale));

   // histfinished = false; // computing color histogram features in a separate thread
   // enablenewfolder();
   // thr = std::thread(&MainWindow::makeHistFeatures,this);
   // thr.detach();
}

// load folder path from settings, D:/PICS - default folder
void MainWindow::loadFolderpath()
{
    QSettings setting("MindDrawPlay","app");
    setting.beginGroup("MainSettings");
    // QString defaultpath = QCoreApplication::applicationDirPath()+"/PICS";
    folderpath = setting.value("picfolderpath").toString();
    if (folderpath=="")
        folderpath = QCoreApplication::applicationDirPath()+"/PICS";
    setting.endGroup();
}

// save folder path to settings
void MainWindow::saveFolderpath()
{
    QSettings setting("MindDrawPlay","app");
    setting.beginGroup("MainSettings");
    setting.setValue("picfolderpath",folderpath);
    setting.endGroup();
}

// enable choice of new folder (if color histogram features are computed for current folder)
void MainWindow::enablenewfolder()
{
    ui->pushButton_7->setEnabled(histfinished);
}

void delay(int temp)
{
    QTime dieTime = QTime::currentTime().addMSecs(temp);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// ==== openCV related functions implementation ====

void applyfilt(int type, Rect rt)   // choice of filter type on area around mouse position
{
    switch (type)
    {
        case 1:
        {
            dstt = dilate(rt);
            break;
        }
        case 2:
        {
            dstt = waves(rt);
            break;
        }
        case 3:
        {
            dstt = cartoon(rt);
            break;
        }
        case 4:
        {
            dstt = orbdetect(rt);
            break;
        }
        case 5:
        {
            dstt = mixfilt(rt);
            break;
        }
        // case 6:
        case 7:
        {
            dstt = ripples(rt);
            break;
        }
    }
}

QImage Mat2QImage(cv::Mat const& srct)
{
     cv::Mat temp;
     cvtColor(srct, temp, COLOR_HSV2RGB);
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy
     return dest;
}

QImage Mat2QImageRGB(cv::Mat const& srct)
{
     cv::Mat temp;
     cvtColor(srct, temp, COLOR_BGR2RGB);
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy
     return dest;
}

void drawcontrolmenu(Mat imgt, int active_item) // drawing menu item labels for switching modes
{
    if (active_item == 0)
        putText(imgt, ocvform->l_menu_item1, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(0,255,0,255), ocvform->l_menu_fontsize);
    else
        putText(imgt, ocvform->l_menu_item1, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(255,255,255,255), ocvform->l_menu_fontsize);

    if (active_item == 1)
        putText(imgt, ocvform->l_menu_item2, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy+40), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(0,255,0,255), ocvform->l_menu_fontsize);
    else
        putText(imgt, ocvform->l_menu_item2, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy+40), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(255,255,255,255), ocvform->l_menu_fontsize);

    if (active_item == 2)
        putText(imgt, ocvform->l_menu_item3, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy+80), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(0,255,0,255), ocvform->l_menu_fontsize);
    else
        putText(imgt, ocvform->l_menu_item3, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy+80), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(255,255,255,255), ocvform->l_menu_fontsize);

    if (active_item == 3)
        putText(imgt, ocvform->l_menu_item4, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy+120), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(0,255,0,255), ocvform->l_menu_fontsize);
    else
        putText(imgt, ocvform->l_menu_item4, Point2f(ocvform->l_menu_posx,ocvform->l_menu_posy+120), FONT_HERSHEY_PLAIN, ocvform->l_menu_fontscale, Scalar(255,255,255,255), ocvform->l_menu_fontsize);
}

void Processing() // processing of HUE, Saturation, Value changes
{
    cvtColor(src,img,COLOR_RGB2HSV);

    int hue = hue_elem - 255;
    int saturation = satur_elem - 255;
    int value = value_elem - 255;

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

            if (cur1 < 0) cur1 = 0; else if (cur1 > 255) cur1 = 255;
            if (cur2 < 0) cur2 = 0; else if (cur2 > 255) cur2 = 255;
            if (cur3 < 0) cur3 = 0; else if (cur3 > 255) cur3 = 255;

            img.at<Vec3b>(Point(y,x))[0] = cur1;
            img.at<Vec3b>(Point(y,x))[1] = cur2;
            img.at<Vec3b>(Point(y,x))[2] = cur3;
        }
    }

    cvtColor(img,image,COLOR_HSV2RGB);

    //if (ocvform->showlabel)
    //    sprintf(ocvform->l_str,"Attention: %d",elem4);

    if (!ocvform->hueonly)
    {
        if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
        {
            cam>>trp;
            flip(trp,trp,1);
            cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
            addWeighted(image, alphaval, trp, 1 - alphaval, 0, dst);
        }
        else
            addWeighted(image, alphaval, srccopy, 1 - alphaval, 0, dst);                

        if (ocvform->showmenu)
        {           
            menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();         
            if (ocvform->color_overlay_flow)
                drawcontrolmenu(dst,1);
            else if (ocvform->dreamflow)
                drawcontrolmenu(dst,2);
            else if (ocvform->puzzleflow_on)
                drawcontrolmenu(dst,3);
            else
                drawcontrolmenu(dst,0);
        }
        imshow("MindOCV", dst);
    }
    else
    {
        if (ocvform->showmenu)
        {           
            menu_area = image(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();         
            if (ocvform->color_overlay_flow)
                drawcontrolmenu(image,1);
            else if (ocvform->dreamflow)
                drawcontrolmenu(image,2);
            else if (ocvform->puzzleflow_on)
                drawcontrolmenu(image,3);
            else
                drawcontrolmenu(image,0);
        }
        imshow("MindOCV", image);
    }
}

void ProcessingMix() // processing of overlay changes, alphaval - transparency
{
    alphaval = (double) overlay_elem / 100;
    if ((estattention) || (simulated_eeg))  // if MindWave connected and attention values are streaming
    {               
        if (ocvform->color_overlay_flow)
        {
            if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
            {
                cam>>trp;
                flip(trp,trp,1);
                //pyrUp(trp, trp, Size(image.cols, image.rows));
                cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
                addWeighted(image, alphaval, trp, 1 - alphaval, 0, dst);
            } else
                addWeighted(image, alphaval, srccopy, 1 - alphaval, 0, dst);
        }
        else
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
    }
    else         
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);    
    clear_dst.release();
    clear_dst = dst.clone();

    if (ocvform->showmenu)
    {       
        menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();     
        if (ocvform->color_overlay_flow)
            drawcontrolmenu(dst,1);
        else if (ocvform->dreamflow)
            drawcontrolmenu(dst,2);
        else if (ocvform->puzzleflow_on)
            drawcontrolmenu(dst,3);
        else
            drawcontrolmenu(dst,0);
    }
    imshow("MindOCV", dst);
}

void fillpolygon(Mat img)
{
    int lineType = LINE_8;   
    Point rook_points[1][ocvform->pointsinpoly];
    for (int i=0; i<ocvform->pointsinpoly; i++)
        rook_points[0][i] = Point(qrand()%dstt.rows,qrand()%dstt.cols);
    const Point* ppt[1] = {rook_points[0]};
    int npt[] = { ocvform->pointsinpoly };
    fillPoly(img,ppt,npt,1,Scalar( 255, 255, 255 ),lineType);
}

Mat adaptivemask(Rect rt)
{
    srct = dst(rt);
    cvtColor(srct,gray_element,COLOR_BGR2GRAY);
    int blocksize = 15;
    int constant = 2;
    adaptiveThreshold(gray_element,gray_element,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,blocksize,constant);
    return gray_element;
}

void smoothtransp(Rect srcDstRect, bool indreamflow)
{
    blob.release();
    blob = dst(srcDstRect).clone();
    int n = 30; int step = dstt.cols/n;
    double alpha = 1 - (double)ocvform->transp/100;
    double incstep = alpha / n;
    for (int i=0; i<n; i++)
    {
        Rect droprect(i*step/2, i*step/2, dstt.cols - step*i, dstt.cols - step*i);
        addWeighted(dstt(droprect), (i+1)*incstep, blob(droprect), 1 - (i+1)*incstep, 0, blob(droprect));
    }
    if ((ocvform->circle_brush) || (ocvform->polygonmask))
    {
        Mat mask_image(dstt.size(), CV_8U, Scalar(0)); // mask for region

        if (indreamflow)
        {
            if (!ocvform->polygonmask)
            {
                if (ocvform->circle_brush)
                {
                    if (!ocvform->dropsmode)
                        circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), mask_image.rows/2, CV_RGB(255, 255, 255),-1,LINE_AA);
                    else
                        circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->drflow_area/2, CV_RGB(255, 255, 255),-1,LINE_AA);
                }
            }
            else
                fillpolygon(mask_image);
        }
        else
            circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), mask_image.rows / 2, CV_RGB(255, 255, 255),-1,LINE_AA);

        blob.copyTo(dst(srcDstRect),mask_image);
    }
    else
        blob.copyTo(dst(srcDstRect));
}

void onMouse( int event, int x, int y, int flags, void* )   // Mouse clicks and moves processing
{    
    int margin;
    if (!ocvform->focuseddreamflow)
        margin = ocvform->currfilterarea;
    else
        margin = ocvform->fdfarea;

    // change of brush areas for dream flow and focused dreamflow
    if ((!ocvform->color_overlay_flow) && (event == EVENT_MOUSEWHEEL) && (y<dst.rows-margin/2) && (x<dst.cols-margin/2) && (y>margin/2) && (x>margin/2))
    {
        if (getMouseWheelDelta(flags) > 0)
        {
            if (margin<790)
            {
                if (!ocvform->focuseddreamflow)
                    ocvform->setfilterarea(ocvform->currfilterarea+10);
                else
                    ocvform->setfdfarea(ocvform->fdfarea+10);
            }
        }
        else
        {
            if (margin>50)
            {
                if (!ocvform->focuseddreamflow)
                    ocvform->setfilterarea(ocvform->currfilterarea-10);
                else
                    ocvform->setfdfarea(ocvform->fdfarea-10);
            }
        }
    }

    if ((!ocvform->color_overlay_flow) && (event == EVENT_MOUSEMOVE) && (y<dst.rows-margin/2) && (x<dst.cols-margin/2) && (y>margin/2) && (x>margin/2))
    {
        // draw brush contour
        if (ocvform->drawbrushcontour)
        {
            dst0.release();
            dst0 = dst.clone();
            if (ocvform->circle_brush)                            
                circle(dst0, Point(x, y), margin/2, CV_RGB(255, 255, 255),0,LINE_AA);
            else
                rectangle(dst0, Rect(x-margin/2, y-margin/2, margin, margin), CV_RGB(255, 255, 255),0,LINE_AA);
            imshow("MindOCV", dst0);
        }
    }

    if ((!ocvform->color_overlay_flow) && (event == EVENT_MOUSEMOVE) && (y<dst.rows-margin/2) && (x<dst.cols-margin/2) && (y>margin/2) && (x>margin/2))
    {
        ocvform->currmousepos.setX(x);
        ocvform->currmousepos.setY(y);        
    }

    if ((!ocvform->color_overlay_flow) && (event == EVENT_MOUSEMOVE) && ((flags ==  EVENT_FLAG_LBUTTON) || (keepfiltering)) && (y<dst.rows-margin/2) && (x<dst.cols-margin/2) && (y>margin/2) && (x>margin/2))
    {      
        curr_iter++; // determines how often with mouse moves and click pressed will be applied brush
        if (curr_iter >= ocvform->currfilterrate)
        {
            Rect srcDstRect(x-margin/2, y-margin/2, margin, margin);
            dstt = dst(srcDstRect); // dst - full resulting overlay pic of main (src) and overlay (srccopy)

            applyfilt(ocvform->currfilttype,srcDstRect);           

            if (ocvform->currfilttype==5)
                smoothtransp(srcDstRect,false);
            else
            {
                if (ocvform->circle_brush)
                {
                    Mat mask_image(Size(margin,margin), CV_8U, Scalar(0)); // mask to have only circle of region
                    circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), margin/2, CV_RGB(255, 255, 255),-1,LINE_AA);
                    dstt.copyTo(dst(srcDstRect),mask_image);
                }
                else
                    dstt.copyTo(dst(srcDstRect));
            }

            menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();

            imshow("MindOCV", dst);

            curr_iter=0;
        }
    }

    if ((!ocvform->color_overlay_flow) && (event == EVENT_LBUTTONDOWN)  && (y<dst.rows-margin/2) && (x<dst.cols-margin/2) && (y>margin/2) && (x>margin/2))
    {
        Rect srcDstRect(x-margin/2, y-margin/2, margin, margin);
        dstt = dst(srcDstRect); // dst - full resulting overlay pic of main (src) and overlay (srccopy)

        if ((ocvform->currfilttype==5) && (ocvform->changerandpic_byclick))
            ocvform->changerandpic();

        applyfilt(ocvform->currfilttype,srcDstRect);

        prev_dst = dst.clone(); // for keeping state before last action (using in "cancel last")

        if (ocvform->currfilttype==5)            
            smoothtransp(srcDstRect,false);
        else
        {
            if (ocvform->circle_brush)
            {
                Mat mask_image(Size(margin,margin), CV_8U, Scalar(0)); // mask to have only circle of region
                circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), margin/2, CV_RGB(255, 255, 255),-1,LINE_AA);
                dstt.copyTo(dst(srcDstRect),mask_image);
            }
            else
                dstt.copyTo(dst(srcDstRect));
        }

        menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();

        imshow("MindOCV", dst);
    }

    // starting dreamflow drops by middle mouse down
    if ((!ocvform->color_overlay_flow) && (event == EVENT_MBUTTONDOWN) && (!ocvform->dreamflow) && (!ocvform->drops_by_click_mode)
        && (y<dst.rows-margin/2) && (x<dst.cols-margin/2) && (y>margin/2) && (x>margin/2))
    {
        ocvform->drops_by_click_mode = true;
        ocvform->plotdroprect = false;
        ocvform->dropsmode = true;
        ocvform->randmixer_mode_on();
        ocvform->drop_center_from_mousepos();        
        ocvform->updateformvals();
        if (ocvform->changerandpic_byclick)
            ocvform->changerandpic();
        prev_dst = dst.clone();
        ocvform->start_stop_dreamflow(true);
    } else
    // stop dreamflow drops by middle mouse down
    if ((!ocvform->color_overlay_flow) && (event == EVENT_MBUTTONUP) && (ocvform->drops_by_click_mode))
    {
       ocvform->start_stop_dreamflow(false);
       ocvform->drops_by_click_mode = false;
    }
    else
    if ((event == EVENT_MBUTTONDOWN) && (ocvform->multi_img_dflow))
    {
        // reset multi-dreamflow buffer: update pics array in buffer
        ocvform->mww->init_img_set();
    }
    else
    if ((event == EVENT_MBUTTONDOWN) && (ocvform->color_overlay_flow))
    {
        // random choice of main pic and updates of corresponded vectors for left and right pictures
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
        cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
        if ((ocvform->color_overlay_flow) && (!ocvform->hueonly))
        {
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
            imshow("MindOCV", dst);
        }
        if (ocvform->showmenu)
            drawcontrolmenu(dst,2);
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
        cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        if (ocvform->color_overlay_flow)
        {
            if (!ocvform->hueonly)
            {
                addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
                imshow("MindOCV", dst);
            }
            if (ocvform->showmenu)
                drawcontrolmenu(dst,2);
        }
    }
    else    
    if (event == EVENT_LBUTTONDBLCLK)
    {
        // fullscreen mode activation
        if (!fullscr)
        {
            rs->setGeometry(0,0,1940,70);
            rs->move(0,0);
            ocvform->move(QPoint(373,950));
            cv::setWindowProperty("MindOCV",cv::WND_PROP_FULLSCREEN,1);
            fullscr=true;
        }
        else
        {
            rs->setGeometry(0,0,1600,70);
            rs->move(158,0);
            ocvform->move(QPoint(373,900));
            cv::setWindowProperty("MindOCV",cv::WND_PROP_FULLSCREEN,0);
            fullscr=false;
        }
        rs->changefsize(fullscr);
    }
}

Mat ripples(Rect srcRect)  // ripples effect filter
{
    Mat dstg = clear_dst(srcRect).clone();
    Mat dstw = dstg.clone();
    Mat tt1, tt2;
    int bshift = 2, k = 0;
    float damping = 0.995;

    dstg.convertTo(tt1, CV_32FC3);
    split(tt1,trimg);
    dstw.convertTo(tt2, CV_32FC3);
    split(tt2,resimg);

    while (k<3)
    {
        for (int t=0; t<3; t++)
        {
            for (int i=bshift; i<dstg.rows-bshift; i++)
            {
                for (int j=bshift; j<dstg.cols-bshift; j++)
                {
                    resimg[t].at<float>(i,j) =
                            ( trimg[t].at<float>(i-bshift,j) + trimg[t].at<float>(i+bshift,j)  +
                              trimg[t].at<float>(i,j-bshift) + trimg[t].at<float>(i,j+bshift) ) / 2 -
                                resimg[t].at<float>(i,j);
                    resimg[t].at<float>(i,j) = resimg[t].at<float>(i,j) * damping;
                }
            }
        }

      //  Mat mask_image(dstg.size(), CV_8U, Scalar(0));
      //  circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);
      //  dst1.copyTo(dst(srcRect));
      //  imshow("MindOCV", dst);

        for (int t=0; t<3; t++)
        {
            temptrio[t] = resimg[t].clone();
            resimg[t] = trimg[t].clone();
            trimg[t] = temptrio[t].clone();
        }

      //  delay(50);
        k++;
    }

    channels.clear();
    channels.push_back(resimg[0]);
    channels.push_back(resimg[1]);
    channels.push_back(resimg[2]);
    merge(channels, dst1);

    dst1.convertTo(dst1, CV_8UC3);
    return dst1;
}

Mat waves(Rect srcRect) // waves filter
{       
    Mat dstg = dst(srcRect).clone();
    Mat dstw = dstg.clone();
    int offset_x;
    int offset_y;
    int bshift = 10;
    for (int i=bshift; i<dstg.rows-bshift; i++)
        for (int j=bshift; j<dstg.cols-bshift; j++)
        {
            offset_x = int(ocvform->wave_amp * sin(2 * 3.14 * i / ocvform->wave_freqs));
            offset_y = int(ocvform->wave_amp * cos(2 * 3.14 * j / ocvform->wave_freqs));
            if ((i+offset_y < dstg.rows) && (j+offset_x < dstg.cols))
                dstg.at<Vec3b>(Point(i,j)) = dstw.at<Vec3b>(Point((i+offset_y)%dstg.rows,(j+offset_x)%dstg.cols));
          //  else
           //     dstg.at<Vec3b>(Point(i,j)) = 0;
    }    
    return dstg;
}

Mat dilate(Rect srcRect) // dilate filter
{
    srct = dst(srcRect);
    dstg.release();
    MorphShapes dilation_type;
    if (ocvform->dilation_elem == 1)
        dilation_type = MORPH_RECT;
    else if (ocvform->dilation_elem == 2)
        dilation_type = MORPH_CROSS;
    else
        dilation_type = MORPH_ELLIPSE;
    element = getStructuringElement(dilation_type, Size(2*ocvform->dilation_size + 1, 2*ocvform->dilation_size+1 ), Point( ocvform->dilation_size, ocvform->dilation_size ) );
    dilate( srct, dstg, element,Point(-1,-1),1,BORDER_REPLICATE);    
    return dstg;
}

Mat cartoon(Rect srcRect) // cartoonize filter
{
    srct = dst(srcRect);
    int num_repetitions = 1;
    //int ds_factor = 2;
    cvtColor(srct,gray_element,COLOR_BGR2GRAY);
    medianBlur(gray_element,gray_element,ocvform->kernel_s); // Apply median filter to the grayscale image
    edges.release();
    mask.release();
    trp.release();
    Laplacian(gray_element,edges,CV_8U,ocvform->kernel_s); // // Detect edges in the image and threshold it
    threshold(edges, mask, 180, 255, THRESH_BINARY_INV); // 'mask' is the sketch of the image

    //  cv::resize(srct, srct, cv::Size(srct.cols/ds_factor,srct.rows/ds_factor), 0, 0, cv::INTER_AREA); //  Resize the image to a smaller size for faster computation
    for (int i=0; i < num_repetitions; i++) // Apply bilateral filter the image multiple times
    {
        bilateralFilter(srct,trp,ocvform->kernel_s,ocvform->sigma_color,ocvform->sigma_space);
        srct=trp.clone();
    }    
   // cv::resize(srct, srct, cv::Size(srct.cols*ds_factor,srct.rows*ds_factor), 0, 0, cv::INTER_LINEAR);
    dstg.release();
    bitwise_and(srct,srct,dstg,mask); // Add the thick boundary lines to the image using 'AND' operator
    //Mat msk;
    //inRange(dstg, Scalar(0,0,0), Scalar(0,0,0), msk);
    //dstg.setTo(Scalar(255,255,255), msk);
    return dstg;
}

Scalar qcolor2scalar(QColor color)
{
    int r,g,b;
    color.getRgb(&r, &g, &b);
    return cv::Scalar(b,g,r);
}

Mat orbdetect(Rect srcRect) // ORB features detector
{
    srct = dst(srcRect);
    vector<KeyPoint> kpts;
    cvtColor(srct,gray_element,COLOR_BGR2GRAY);
    Ptr<ORB> orbdet = ORB::create(ocvform->nfeatures,ocvform->scalef,ocvform->nlevels,ocvform->edgetreshold);
    orbdet->detect(gray_element,kpts);
    dstg.release();
    dstg = srct.clone();
    if (ocvform->randfcolor)
        cv::drawKeypoints(srct,kpts,dstg,Scalar(qrand()%256,qrand()%256,qrand()%256));
    else
        cv::drawKeypoints(srct,kpts,dstg,qcolor2scalar(ocvform->fcolor));
    return dstg;
}

Mat mixfilt(Rect srcRect) // mixer filter: copy region from main/overlay/random pic with transparency
{
    dstg.release();  
    if (ocvform->mixtype==1)
        dstg = src(srcRect).clone();
    else if (ocvform->mixtype==2)
        dstg = srccopy(srcRect).clone();
    else
        dstg = ocvform->randpic(srcRect).clone();
    return dstg;
}

void defineiconsarr()
{
    // define and update left pictures vector depending on action and state
    // different conditions to prevent duplicates for current main and overlay pictures
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
    // define and update right pictures vector depending on action and state
    // different conditions to prevent duplicates for current main and overlay pictures
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

void shuffleicons(bool left) // random shuffling of pictures
{
    if (left)
        random_shuffle(iconsarr.begin(), iconsarr.end());
    else
        random_shuffle(riconsarr.begin(), riconsarr.end());
}

// ==== openCV related functions implementation end ====

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::usingcam(bool fl)
{
    if (fl)
        cam.open(0);
    else
        cam.release();
}

void MainWindow::processingcolor(int hue_val)
{
    hue_elem = hue_val;
    ocvform->updatelevels(hue_elem,overlay_elem,atten_elem,border_elem);
    Processing();
}

void MainWindow::processingoverlay(int overlay_val)
{
    overlay_elem = overlay_val;
    ocvform->updatelevels(hue_elem,overlay_elem,atten_elem,border_elem);
    ProcessingMix();
}

void MainWindow::streamflows_Update() // streaming flows to MindPlay
{
    if ((opencvstart) && (plotw->start) && (plotw->filteringback) && (!plotw->colorizeback))
    {
        QPixmap pm;
        if (!ocvform->hueonly)
            pm = QPixmap::fromImage(Mat2QImageRGB(dst));
        else
            pm = QPixmap::fromImage(Mat2QImageRGB(image));
        plotw->setbackimage(pm,true);       
    }
}

void MainWindow::fillpuzzle_withneighbours() // filling puzzle with neighbouring pics
{
    if ((paintw_started) && (paintw->puzzlemode))
    {
        if (ocvform->flowdirection==1)
            paintw->updateset_allpics_similarly(nearest_pics);
        else if (ocvform->flowdirection==-1)
            paintw->updateset_allpics_similarly(farest_pics);
    }
}

void MainWindow::fillmaininpuzzle(int t)    // fill main pic in puzzle
{
    QString ocvpic = folderpath+"/"+imglist.at(t);
    if ((paintw_started) && (paintw->puzzlemode))
        paintw->setbackimageocv(ocvpic);
}


int MainWindow::getchi2distsize()       // get size of vector with chi2dist
{
    return chi2distances.size();
}

// get number of image in image list / folder (for "choose" option on panels // open file dialog)
int MainWindow::getimagenum(QString st)
{
    int nt = -1;
    for (int i=0; i<imglist.length(); i++)
        if (folderpath+"/"+imglist.at(i)==st)
        {
            nt = i;
            break;
        }
    return nt;
}

QString MainWindow::getfolderpath()     // return pics folder
{
    return folderpath;
}

QString MainWindow::getimagepath(int t) // return image path for ocvcontrol form in update random image
{
    return folderpath+"/"+imglist.at(t);
}

void MainWindow::setdream0() // grab initial image before starting expanding mode in dreamflow
{   
    if (ocvform->showmenu)
    {
       // label_area.copyTo(dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)));
        menu_area.copyTo(dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)));
    }
    dream0 = dst.clone();
}

void MainWindow::setprevdfrect(int x, int y, int w, int h) // save prev expanding window in dreamflow
{
    df_srcDstRect = Rect(x,y,w,h);
}

void MainWindow::drawwindow(int x, int y, int w, int h) // draw rect of window for expanding mode in dreamflow
{
    dst(df_srcDstRect).copyTo(dream0(df_srcDstRect)); // copy updated rect on image without window box    
    dst = dream0.clone();
    dream0.release();  
    dream0 = dst.clone();
    rectangle(dst, Point(x-2,y-2), Point(x+w+2,y+h+2),ocvform->wcolor,2);    
    imshow("MindOCV",dst);
}

void MainWindow::save_and_add_overlaypic() // save and add current overlay to pictures
{
    QString fimg = "newimg-"+QDateTime::currentDateTime().toString("ddMMyyyy-hhmmss")+".jpg";
    QString fn = folderpath + "/" + fimg;
    imwrite(fn.toStdString(),dst);
    imglist.append(fimg);
    leftpw->imgnumber++;
    rightpw->imgnumber++;
    int newlen = imglist.length();
    imgarray.resize(newlen);
    Size size(138,138);
    cv::resize(dst,dstemp,size);
    QImage qm = Mat2QImageRGB(dstemp);
    imgarray[newlen-1]=QPixmap::fromImage(qm);
    iconsarr.push_back(newlen-1);
    riconsarr.push_back(newlen-1);
}

void MainWindow::setopencvt(int i)  // change openCV timer interval
{
    opencvinterval = i;
    picfilt->setInterval(opencvinterval);
}

void MainWindow::cancellast()   // cancel last filtering action
{
    dst = prev_dst;
    imshow("MindOCV", dst);
}

void MainWindow::cancelall()    // cancel all filtering actions
{
    dst = clear_dst.clone();
    imshow("MindOCV", dst);
}

void MainWindow::set_dreamflow_interval(int t)  // set dreamflow time interval
{
    dreamflow_timer->setInterval(t);
}

void MainWindow::focuseddflow_Update() // focused dreamflow timer: change of overlay pic
{
    // random choice of overlay pic and updates of corresponded vectors for left and right pictures
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
    cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
}

void MainWindow::dreamflow_Update() // timer for dreamflow mode, when new pic appears by random fragment over old
{
    int x, y;
    ocvform->drflow_area = 40;
    Rect srcDstRect; 
    if (ocvform->showmenu)
    {      
        menu_area.copyTo(dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)));
        menu_area.copyTo(dream0(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)));
    }

    if (!ocvform->dropsmode)
    {
        x = ocvform->currfilterarea/2 + qrand() % (dst.cols - ocvform->currfilterarea);
        y = ocvform->currfilterarea/2 + qrand() % (dst.rows - ocvform->currfilterarea);
        srcDstRect = Rect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea);
    } else
    {
        // define area inside current expanding window
        if (ocvform->drops_by_att)
            ocvform->drflow_area = (((ocvform->x_right-ocvform->x_left)/2-5)*atten_elem)/100+3;
        else
            ocvform->drflow_area = qrand()%((ocvform->x_right-ocvform->x_left)/2-5)+3;

        x = ocvform->x_left + ocvform->drflow_area/2 + qrand() % (ocvform->x_right - ocvform->x_left - ocvform->drflow_area); // define rect based on area size (inside expanding window)
        y = ocvform->y_top + ocvform->drflow_area/2 + qrand() % (ocvform->y_bottom - ocvform->y_top - ocvform->drflow_area);
        srcDstRect = Rect(x-ocvform->drflow_area/2, y-ocvform->drflow_area/2, ocvform->drflow_area, ocvform->drflow_area);
    }

    if (ocvform->multi_img_dflow)
    {
        int t = qrand()%ocvform->multi_set_size;
        ocvform->randpic = curr_img_set[t];       
    }

    dstt = ocvform->randpic(srcDstRect).clone();
    smoothtransp(srcDstRect,true);

    if (ocvform->showmenu)
    {       
        menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();       
        drawcontrolmenu(dst,2);
        drawcontrolmenu(dream0,2);
    }    

    imshow("MindOCV", dst);
}

inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
   {
      switch ( inImage.format() )
      {
         // 8-bit, 4 channel
         case QImage::Format_ARGB32:
         case QImage::Format_ARGB32_Premultiplied:
         {          
            cv::Mat  mat( inImage.height(), inImage.width(),CV_8UC4,const_cast<uchar*>(inImage.bits()),static_cast<size_t>(inImage.bytesPerLine()));
            return (inCloneImageData ? mat.clone() : mat);
         }

         // 8-bit, 3 channel
         case QImage::Format_RGB32:
         {          
            cv::Mat  mat( inImage.height(), inImage.width(),CV_8UC4,const_cast<uchar*>(inImage.bits()),static_cast<size_t>(inImage.bytesPerLine()));
            cv::Mat  matNoAlpha;
            cv::cvtColor( mat, matNoAlpha, cv::COLOR_BGRA2BGR );   // drop the all-white alpha channel
            return matNoAlpha;
         }

         // 8-bit, 3 channel
         case QImage::Format_RGB888:
         {
            QImage   swapped = inImage.rgbSwapped();
            return cv::Mat( swapped.height(), swapped.width(),CV_8UC3,const_cast<uchar*>(swapped.bits()),static_cast<size_t>(swapped.bytesPerLine())).clone();
         }

         // 8-bit, 1 channel
         case QImage::Format_Indexed8:
         {
            cv::Mat  mat( inImage.height(), inImage.width(),CV_8UC1,const_cast<uchar*>(inImage.bits()),static_cast<size_t>(inImage.bytesPerLine()));
            return (inCloneImageData ? mat.clone() : mat);
         }

         default:
            qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
            break;
      }
      return cv::Mat();
   }

void MainWindow::setdstfromplay(QImage qm) // set openCV picture from MindPlay picture, only in not flow mode - for filtering
{
    Mat dsnt = QImageToCvMat(qm);
    cv::resize(dsnt, dsnt, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
    cvtColor(dsnt,dsnt,COLOR_BGRA2BGR);
    clear_dst = dsnt.clone();
    dst = dsnt.clone();
    imshow("MindOCV", dst);
}

int MainWindow::geticonnum(int t, bool left) // for updating icons in left and right panels
{
    if (left)
        return iconsarr[t];
    else
        return riconsarr[t];
}

int MainWindow::getmainpic() // return index of main pic in icons arr
{
    return currmainpic;
}

int MainWindow::getoverpic() // return index of overlay pic in icons arr
{
    return curroverpic;
}

// update main pic without plotting and related changes in other forms (for dreamflow pics change)
void MainWindow::just_update_mainpic(int num)
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
    cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
    curr_img_set[0]=src.clone();
}

void MainWindow::updatemainpic(int num)
{           
    // function for dbclick on left panel - updating main pic and vectors of icons
    // also MindPlay and MindDraw pictures, if corresponded windows opened
    just_update_mainpic(num);
    QString ocvpic=folderpath+"/"+imglist.at(currmainpic);
    if ((ocvform->color_overlay_flow) && (!ocvform->hueonly))
    {
        if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
        {
            cam>>trp;
            flip(trp,trp,1);
            cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
            addWeighted(src, alphaval, trp, 1 - alphaval, 0, dst);
        }
        else
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("MindOCV", dst);
    }
    if (!ocvform->color_overlay_flow)
        ocvform->setcurrdream(currmainpic);
    if ((plotw->start) && !((plotw->filteringback) && (!plotw->colorizeback)) && (!plotw->camerainp))
        plotw->setbackimg_fromleftpanel(ocvpic);
    if (paintw_started)
        paintw->setbackimageocv(ocvpic);     

    if (ocvform->flowdirection!=0)
    {
        getchi2dists(currmainpic);
        fillpuzzle_withneighbours();
    }
}

void MainWindow::updateoverpic(int num)
{
    // function for dbclick on right panel - updating overlay pic and vectors of icons
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
    cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
    curr_img_set[1] = srccopy.clone();
    if ((ocvform->color_overlay_flow) && (!ocvform->hueonly))
    {
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("MindOCV", dst);
    }
}

void MainWindow::printdata(QString str) // updating execution log
{   
    strList1.push_front(str);    
    strListM1->setStringList(strList1);
}

void MainWindow::on_pushButton_clicked() // close the App
{
    ocvform->color_overlay_flow = false;
    if (mwconnected)
        TG_FreeConnection(connectionId);
    if (plotw->start)
    {
        plotw->quitthreads();
        if (plotw->total_intervals>0)
        {
            plotw->streamrec.str(std::string());
            plotw->streamrec << "Session ended: " << (QDateTime::currentDateTime().toString("ddMMyyyy-hhmmss")).toStdString();
            plotw->recordFile.write(plotw->streamrec.str().data(), plotw->streamrec.str().length());
            plotw->recordFile.close();
        }        
    }
    plotw->cameraoff();
    openglw->close();
    cv::destroyAllWindows();
    QApplication::quit();
}

void MainWindow::on_pushButton_2_clicked() // BCI2000 window run
// in case of other device than MindWave NeuroSky -
// the function should be adapted for particular EEG device parameters used with BCI2000
// to account for number of samples in streaming blocks and sampling rate
// also functions of appconnect module (connectWin object here) should be adapted
{          
    bool ok1,ok2;
    stringstream str1, str2;
    str1 << "Enter sampling rate: ";
    int srate = QInputDialog::getInt(this,"Sampling rate",str1.str().c_str(), 512, 1, 1024, 1, &ok1);
    if (ok1)
        plotw->srfr = srate;
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
            plotw->srfr = srate;
            ui->pushButton_5->setEnabled(false);
            ui->label->setText("Channel number: "+QString::number(channel));
            ui->label->setVisible(true);
            bciconnect = true;
            ui->pushButton_4->setEnabled(false);            
        } else
            printdata("Connection with BCI2000 not established.. ( ");
    }
}

void MainWindow::on_pushButton_3_clicked() // MindPlay window run
{
    plotw->show();
    plotw->doplot();
    plotw->appcn=connectWin;
    paintw->scene->init(plotw, this);
    connectWin->ps=paintw->scene;    
    paintw->move(QApplication::desktop()->screen()->rect().center() - paintw->rect().center()+QPoint(-10,-35));
    paintw->scene->clear();
    paintw->loadempty();
    plotw->paintfstart=true;
    paintw->setpicfolder(folderpath);
    connectWin->ps=paintw->scene;
    if (plotw->start)
        plotw->pss=paintw->scene;
    streamflows->start();
    paintw->hide();
    plotw->setFocus();
}

void MainWindow::on_pushButton_4_clicked() // MindDraw window run
{
    on_pushButton_3_clicked();
    paintw->show();
}

void MainWindow::on_pushButton_5_clicked()
{
    mindwaveconnect();
}

void MainWindow::setsourceimg(QString fpath) // set openCV main image from path
{
    src = imread(fpath.toStdString());
    cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
    imshow("MindOCV", src );
}

void MainWindow::setsourceimgd(QImage qp) // set openCV main image from QImage (MindPlay/MindDraw windows)
{
    cv::Mat matp(qp.height(),qp.width(),CV_8UC3, qp.bits());
    //src = matp;
    imshow("MindOCV", src );
}

void MainWindow::setattent(int i)
{
    atten_elem=i;
    ocvform->updatelevels(hue_elem,overlay_elem,atten_elem,border_elem);
    // opencvinterval = 25;// + (100 - elem4)/2;
    // picfilt->setInterval(opencvinterval);

    if (ocvform->puzzleflow_on)
    {        
        if (ocvform->corrcells_by_att)
        {
            puzzleflow->stop();
            ocvform->corr_cell_part = (double)atten_elem/90;
            if (atten_elem<10)
                ocvform->corr_cell_part = 0.1;
            if (atten_elem>ocvform->changepuzzleborder)
                ocvform->corr_cell_part = 1;
            int m = (int)ocvform->cellnums*ocvform->corr_cell_part;
            for (int t = 0; t < m; t++)
                fillcell(ocvform->cells_indexes[t],ocvform->cols);
            puzzleflow->start();
            ocvform->updateformvals();

            if (atten_elem>ocvform->changepuzzleborder)
            {
                imshow("MindOCV", dst);
                if (canchangepuzzle)
                {
                    canchangepuzzle = false;
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
                    cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
                }
            }
            else
                canchangepuzzle = true;
        }
        if (ocvform->puzzlerate_by_att)
        {
            ocvform->puzzleflowrate = 100 + atten_elem*2.5;
            puzzleflow->setInterval(ocvform->puzzleflowrate);
            ocvform->updateformvals();
        }
        if (ocvform->cellsize_by_att)
        {
            puzzleflow->stop();
            ocvform->cell_size = round((double)(100 + atten_elem*2)/10)*10;
            ocvform->cols = ocvform->picwidth / ocvform->cell_size;
            ocvform->rows = ocvform->picheight / ocvform->cell_size;
            ocvform->cellnums = ocvform->cols * ocvform->rows;
            fillcells();
            puzzleflow->start();
            ocvform->updateformvals();
        }
    }

    if ((ocvform->attmodul_area) && (!ocvform->color_overlay_flow))
    {
        // attention modulated filter area, only not color-overlay flow mode
        if (atten_elem<20)
            atten_elem=20;
        ocvform->currfilterarea=atten_elem*5;
        ocvform->updateformvals(); // updating values on control form
    }

    if ((ocvform->fdfarea_byatt) && (!ocvform->color_overlay_flow))
    {
        // attention modulated focused dream flow area
        if (atten_elem<20)
            atten_elem=20;
        ocvform->fdfarea = atten_elem*5;
        ocvform->updateformvals();
    }

    if ((!ocvform->color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->changebyattention))
    {
        if ((canchangepic) && (atten_elem>border_elem))
        {
            if (!ocvform->multi_img_dflow)
                ocvform->changerandpic();
            else
            {
                prevmainpic = currmainpic;
                currmainpic = curroverpic;
                prevoverpic = curroverpic;
                curroverpic = riconsarr[qrand() % (imglist.length()-2)];
                lchanged=true;
                rchanged=true;
                defineiconsarr();
                define_riconsarr();
                leftpw->fillpics();
                rightpw->fillpics();
                QString ocvpic = folderpath+"/"+imglist.at(curroverpic);
                src = srccopy.clone();
                curr_img_set[0] = src.clone();
                srccopy = imread(ocvpic.toStdString());
                cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
                curr_img_set[1] = srccopy.clone();
            }
            canchangepic = false;
        } else
        if ((!canchangepic) && (atten_elem<border_elem))
            canchangepic = true;
    }

    if ((!ocvform->color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->dreamflow))
    {
        if (ocvform->attent_modulated_dreams)
        {
            ocvform->dreamflowrate=105-atten_elem;
            dreamflow_timer->setInterval(ocvform->dreamflowrate);            
        }
        if (ocvform->drops_by_att)
        {
            ocvform->drops_interval=120-atten_elem; // 30+elem4/2;
            ocvform->dropsT->setInterval(ocvform->drops_interval);            
        }
        if (ocvform->poly_by_att)        
            ocvform->pointsinpoly = atten_elem / 10 + 3;

        if (ocvform->directionswitch_by_att)
        {
            if (atten_elem>border_elem)
                ocvform->flowdirection=1;
            else
                ocvform->flowdirection=-1;            
        }
        if (ocvform->multi_img_by_att)
            ocvform->multi_set_size = (100-atten_elem)/10 + 1;

        ocvform->updateformvals();
    }
    if ((!ocvform->color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->transp_by_att))
    {
        ocvform->transp = 100 - atten_elem/2;
        ocvform->updateformvals();
    }  
}

void MainWindow::setoverlay(int i)
{
    overlay_elem=i;
    setattent(overlay_elem);
    if (!ocvform->hueonly)
        ProcessingMix();
    checkoverlay();
}

void MainWindow::setborder(int i)
{
    border_elem=i;
    ocvform->updatelevels(hue_elem,overlay_elem,atten_elem,border_elem);
}

QImage MainWindow::grabopcvpic()
{
    if (!ocvform->hueonly)
        return Mat2QImageRGB(dst);
    else
        return Mat2QImageRGB(image);
   // cvtColor(dst,dstcopy,COLOR_RGB2HSV);
  //  return Mat2QImage(dstcopy);
}

void MainWindow::run_opencvform()   // MindOCV window run
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
        ocvform->show();
        br_levels->show();
        plotw->doplot();
        plotw->paintfstart = true;
        paintw_started = true;
        set_showlevelscheckbox(true);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a folder with .jpg files!");
        msgBox.exec();
    }
}

void MainWindow::on_pushButton_6_clicked()
{  
    run_opencvform();
}

void MainWindow::startopencv() // MindOCV initialization function
{
    if (!opencvstart)
    {
        startWindowThread();

        namedWindow("MindOCV",WINDOW_NORMAL);  // WINDOW_OPENGL);

        setMouseCallback("MindOCV", onMouse, 0 );

        currmainpic = qrand() % imglist.length();
        lchanged=false;
        rchanged=false;
        defineiconsarr();
        opencvpic = folderpath+"/"+imglist.at(currmainpic);

        src = imread(opencvpic.toStdString());                
        image = imread(opencvpic.toStdString());
        dst = imread(opencvpic.toStdString());
        clear_dst = imread(opencvpic.toStdString());        
        cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        cv::resize(image, image, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        cv::resize(dst, dst, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        cv::resize(clear_dst, clear_dst, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        ocvform->randpicn = currmainpic;
        ocvform->randpic = src.clone();

        curroverpic = iconsarr[qrand() % (imglist.length()-1)];
        define_riconsarr();
        QString stp = folderpath+"/"+imglist.at(curroverpic);
        srccopy = imread(stp.toStdString());
        cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);

        firstrun=false;    
        defineiconsarr();        
        resizeWindow("MindOCV",1600,828);
        resize(1600,900);
        moveWindow("MindOCV", 150,39);     
        ProcessingMix();

        init_img_set();

        opencvstart=true;
        ocvform->opencvstart=true;
        plotw->opencvstart=true;
        if (plotw->start)
            plotw->enablehue();
        ocvform->updatelevels(hue_elem,overlay_elem,atten_elem,border_elem);

        keyprocess->start();
    }
    else
        imshow("MindOCV", src);
}

void MainWindow::init_img_set()  // init set of images for dreamflow in multi-image mode
{
    curr_img_set.clear();
    curr_img_set.push_back(src.clone());
    curr_img_set.push_back(srccopy.clone());
    img_num_set.clear();
    img_num_set.insert(currmainpic);
    img_num_set.insert(curroverpic);

    for (size_t i=0; i<20; i++)
    {
        int tp = qrand() % imglist.length();
        img_num_set.insert(tp);
    }
    img_num_set.erase(currmainpic);
    img_num_set.erase(curroverpic);

    set<int>::iterator iter = img_num_set.begin();
    for (size_t i=0; i<11; i++)
    {
        int tp = *iter;
        Mat mt = imread((folderpath+"/"+imglist.at(tp)).toStdString());
        cv::resize(mt, mt, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        curr_img_set.push_back(mt.clone());
        iter++;
    }
}

void MainWindow::checkoverlay()
{
    // check if overlay should happen - if attention > border and it's new overcome of border
    if ((overlay_elem>border_elem) && (canchangepic))
    {
        if (!storymode)
        {
            prevmainpic = currmainpic;
            currmainpic = curroverpic;
            prevoverpic = curroverpic;

            if (ocvform->flowdirection==0)
                curroverpic = riconsarr[qrand() % (imglist.length()-2)];                        
            else if (ocvform->flowdirection==1)
            {
                getchi2dists(prevoverpic);
                curroverpic = nearest_pics[ocvform->ngbarea/5+qrand()%ocvform->ngbarea];
                getchi2dists(curroverpic);
            }
            else
            {
                getchi2dists(prevoverpic);
                curroverpic = farest_pics[ocvform->ngbarea/5+qrand()%ocvform->ngbarea];
                getchi2dists(curroverpic);
            }

            // fillpuzzle_withneighbours();
            fillmaininpuzzle(curroverpic);
            lchanged=true;
            rchanged=true;
            defineiconsarr();
            define_riconsarr();
            leftpw->fillpics();
            rightpw->fillpics();            
            QString ocvpic = folderpath+"/"+imglist.at(curroverpic);
            src = srccopy.clone();
            curr_img_set[0] = src.clone();
            srccopy = imread(ocvpic.toStdString());                        
            cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
            curr_img_set[1] = srccopy.clone();        
            if (!ocvform->hueonly)
            {
                if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
                {
                    cam>>trp;
                    flip(trp,trp,1);
                    cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
                    addWeighted(src, alphaval, trp, 1 - alphaval, 0, dst);
                }
                else
                    addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
                imshow("MindOCV", dst);
            }
        } else
        {
            prevoverpic = curroverpic;
            curroverpic = iconsarr[qrand() % (imglist.length()-2)];
            lchanged=false;
            rchanged=true;
            defineiconsarr();
            define_riconsarr();
            leftpw->fillpics();
            rightpw->fillpics();
            QString ocvpic=folderpath+"/"+imglist.at(curroverpic);
            srccopy = imread(ocvpic.toStdString());
            cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
            curr_img_set[1] = srccopy.clone();
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
          //  imshow("MindOCV", dst);
        }
        canchangepic=false;
    }
    if ((overlay_elem<border_elem) && (!canchangepic))
        canchangepic=true;
}

void MainWindow::makeHistFeatures()     // computing image histogram features
{
    QString picst;
    hist_features.clear();
    for (int i=0; i<imglist.length(); i++)
    {        
        picst = folderpath+"/"+imglist.at(i);
        tempimg = imread(picst.toStdString());
        pichist = tempimg;
        gethistfeatures();
    }
    histfinished = true;
    emit histFinished();
    ocvform->histFeaturesReady = true;
    if ((opencvstart) && (ocvform->allngb<imglist.length()))
        emit ocvform->flow_direction_available();
}

void MainWindow::addScaledImage(int num) // add rescaled image
{
    imgarray[num]=QPixmap::fromImage(imageScaling->resultAt(num));
}

void MainWindow::scalingFinished()  // when all rescaling through QtConcurrent is finished
{
   // qDebug() << time_take.elapsed();
    ui->pushButton_7->setEnabled(true);
    if (imglist.size()>7)
    {
        ui->pushButton_6->setEnabled(true);
        if (opencvstart)
        {
            shuffleicons(true);
            leftpw->fillpics();
            shuffleicons(false);
            rightpw->fillpics();
            ProcessingMix();
        }
    }
}

void MainWindow::shuffleiconss(bool left) // shuffling icons by click on panels "random" button
{
    shuffleicons(left);
}

void MainWindow::setfolderpath(QString fp) // set path for pictures folder
{
    folderpath=fp;
    QDir fd(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    ui->lineEdit->setText(folderpath);
    leftpw->imgnumber = imglist.length()-2;
    rightpw->imgnumber = imglist.length()-2;
}

void MainWindow::mindwaveconnect() // function to connect to MindWave device
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
        ui->pushButton_8->setEnabled(false);
        simulateEEG->stop();
        simeeg=false;
        plotw->srfr=512;     // sampling rate
        plotw->imlength=256; // length of single EEG interval
        mindwt->start();
        plotw->mindwstart=true;
        plotw->simeeg=false;
        plotw->mental_activations->start();
        rs->starting();
        ui->checkBox->setEnabled(true);
        ui->pushButton_5->setEnabled(false);
        if (!estattention)
            estattention=true;
        rs->show();
        ui->checkBox_2->setChecked(true);
        on_checkBox_2_clicked();
        mwconnected=true;
        printdata("");
        printdata("Use RightMouse click - start/pause brain waves flow in MindPlay");
    }
    else
        printdata("Connection with MindWave could not be established! :(");
    packetsRead = 0;
}

void MainWindow::mindwtUpdate() // processing data from MindWave device
{
    //int c=0;
    int errCode = TG_ReadPackets( connectionId,1);
    if( errCode == 1 )
    {      
        plotw->chnums=1;
        if ( TG_GetValueStatus(connectionId, TG_DATA_RAW) != 0 )
        {
            mw_raw = TG_GetValue(connectionId, TG_DATA_RAW);         

            if (abs(mw_raw)<180)
            if (plotw->start)
                plotw->getandprocess_eeg_data(mw_raw/2);
            if (rs->start)
                rs->updatesignal(mw_raw/2);
            if (abs(mw_raw)<180) // anti blink artifacts for drawing
            if (paintw_started)
                paintw->scene->getdata(mw_raw/2);
                //cout<<"RAW value: "<<mw_raw<<endl;
        }
        if ( TG_GetValueStatus(connectionId, TG_DATA_ATTENTION) != 0 )
        {
            if (TG_GetValue(connectionId, TG_DATA_ATTENTION)>0)
            {
                mw_atten=TG_GetValue(connectionId, TG_DATA_ATTENTION);
                if (plotw->start)
                    plotw->update_curr_attention(mw_atten);
                if ((opencvstart) && (canchangehue))
                {                                                         
                    curhue=100+mw_atten*4;
                    canchangehue=false;
                }                
                if (paintw_started)
                {
                    paintw->updateattentionplot(mw_atten);
                    paintw->updateattention(plotw->attent);
                    if (opencvstart)
                    {
                        if (ocvform->attent_modul)
                        {
                            if (br_levels->attention_2nd)     // update attention by estimated value
                                setattent(paintw->getestattval());
                            else
                                setattent(mw_atten);       // update attention by value from device
                        }
                        else
                            setattent(mw_medit);
                        curoverl=atten_elem;
                    }
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
                    plotw->update_curr_meditation(mw_medit);
                if (paintw_started)
                    paintw->updatemeditation(plotw->meditt);
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

void MainWindow::simulateEEGUpdate() // simulated EEG data (in development)
{
    const double mean = 0.0;
    const double stddev = 0.7;
    static std::mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<double> dist(mean, stddev);
    double noise = 20*dist(gen);
    deltaamp = zdeltaamp + dist(gen)*10;
    thetaamp = zthetaamp + dist(gen)*24;
    alphaamp = zalphaamp + dist(gen)*26;
    betaamp = zbetaamp + dist(gen)*20;
    gammaamp = zgammaamp + dist(gen)*20;
    hgammaamp = zhgammaamp + dist(gen)*8;
    if (currentel%250==0)       // change phase and base amplitude each 1/2 sec
    {
        zdeltaamp = 2 + qrand() % 10;
        zthetaamp = 8 + qrand() % 26;
        zalphaamp = 9 + qrand() % 21;
        zbetaamp = 8 + qrand() % 20;
        zgammaamp = 10 + qrand() % 10;
        zhgammaamp = 8 + qrand() % 10;
        deltaphs = qrand() % 20;
        thetaphs = qrand() % 20;
        alphaphs = qrand() % 20;
        betaphs = qrand() % 20;
        gammaphs = qrand() % 20;        
    }
    if (currentel==500)
    {
        currentel=0;
        double estattt = paintw->getestattval();
        if (opencvstart)
        {
            if (ocvform->attent_modul)
                setattent(estattt);
            else
                setattent(plotw->meditt);

            curoverl=atten_elem;
            if (canchangehue)
            {
                curhue=100+estattt*4;
                canchangehue=false;
            }
        }
        paintw->updateattentionplot(plotw->curr_estatt);
        paintw->updateattention(plotw->attent);
        paintw->updatemeditation(plotw->meditt);
    }
    currentel++;
    currentsimdata = deltaamp*sin(deltafr*2*M_PI/srfr*(currentel+deltaphs)) + thetaamp*sin(thetafr*2*M_PI/srfr*(currentel+thetaphs)) + alphaamp*sin(alphafr*2*M_PI/srfr*(currentel+alphaphs)) + betaamp*sin(betafr*2*M_PI/srfr*(currentel+betaphs)) + gammaamp*sin(gammafr*2*M_PI/srfr*(currentel+gammaphs)) + hgammaamp*sin(hgammafr*2*M_PI/srfr*(currentel+gammaphs)) + noise;
    //currentsimdata *= 2;    
    rs->updatesignal(currentsimdata);
    if (plotw->start)
        plotw->getandprocess_eeg_data(currentsimdata);
    if (paintw_started)    
        paintw->scene->getdata(currentsimdata/4);            
}

void MainWindow::swap_main_overlay() // swap main and overlay pics
{
    prevoverpic = curroverpic;
    prevmainpic = currmainpic;
    currmainpic = curroverpic;
    curroverpic = prevmainpic;

    leftpw->fillpics();
    rightpw->fillpics();

    dstt = src.clone();
    src = srccopy.clone();
    srccopy = dstt.clone();

    curr_img_set[0] = src.clone();
    curr_img_set[1] = srccopy.clone();
}

QPoint MainWindow::getcellposition(int t, int cols) // get cell pixels coordinates from index
{
    QPoint posxy;
    int row_index = (t-1) / cols + 1;
    int col_index = t - (row_index-1)*cols;
    posxy.setY((row_index-1)*ocvform->cell_size);
    posxy.setX((col_index-1)*ocvform->cell_size);
    return posxy;
}

void MainWindow::swapcells(int t1, int t2, int cols)  // swap cells by indexes
{
    QPoint cellpos1 = getcellposition(t1, cols);
    QPoint cellpos2 = getcellposition(t2, cols);
    Rect cellRect1(cellpos1.x(), cellpos1.y(), ocvform->cell_size, ocvform->cell_size);
    Rect cellRect2(cellpos2.x(), cellpos2.y(), ocvform->cell_size, ocvform->cell_size);
    // int pic1n = qrand()%curr_img_set.size();
    // int pic2n = qrand()%curr_img_set.size();
    // dstt = curr_img_set[pic1n](cellRect1).clone(); // multi-pics puzzling

    if (ocvform->doublepicsmode)
        dstt = srccopy(cellRect1).clone();
    else
        dstt = src(cellRect1).clone();

    //addWeighted(dst(cellRect2), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

    if (ocvform->puzzle_edges)
    {
        if (ocvform->white_edges)
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(255,255,255),2);
        else
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(qrand()%256,qrand()%256,qrand()%256),2);
    }
    if (ocvform->doublepicsmode)
        dstt.copyTo(dst(cellRect1));
    else
        dstt.copyTo(dst(cellRect2));

    // dstt = curr_img_set[pic2n](cellRect2).clone(); // multi-pics puzzling    
    if (ocvform->doublepicsmode)
        dstt = srccopy(cellRect2).clone();
    else
        dstt = src(cellRect2).clone();

    //addWeighted(dst(cellRect1), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

    if (ocvform->puzzle_edges)
    {
        if (ocvform->white_edges)
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(255,255,255),2);
        else
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(qrand()%256,qrand()%256,qrand()%256),2);
    }
    if (ocvform->doublepicsmode)
        dstt.copyTo(dst(cellRect2));
    else
        dstt.copyTo(dst(cellRect1));
}

void MainWindow::fillcell(int t, int cols)  // filling cell by its index
{
    QPoint cellpos = getcellposition(t, cols);
    Rect cellRect(cellpos.x(), cellpos.y(), ocvform->cell_size, ocvform->cell_size);
    dstt = src(cellRect).clone();
  //  addWeighted(dst(cellRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);
    dstt.copyTo(dst(cellRect));    
}

void MainWindow::fillcells()    // filling vector of indexes and correct cells
{    
    ocvform->cells_indexes.clear();
    for (size_t t = 1; t <= ocvform->cellnums; t++)
        ocvform->cells_indexes.push_back(t);
    random_shuffle(ocvform->cells_indexes.begin(), ocvform->cells_indexes.end());
    for (size_t t = 0; t < (unsigned int)(ocvform->cellnums*ocvform->corr_cell_part); t++)
        fillcell(ocvform->cells_indexes[t],ocvform->cols);
}

void::MainWindow::puzzleflow_Update() // timer for puzzle flow gathering
{    
    dst = src.clone();
    int m = (int)(ocvform->cellnums*ocvform->corr_cell_part);
    ocvform->free_cells.clear();
    for (size_t t = m; t < ocvform->cellnums; t++)
        ocvform->free_cells.push_back(ocvform->cells_indexes[t]);
    while (ocvform->free_cells.size()>1)
    {
        int p = qrand()%(ocvform->free_cells.size()-1)+1;
        swapcells(ocvform->free_cells[0],ocvform->free_cells[p],ocvform->cols);
        ocvform->free_cells.erase(ocvform->free_cells.begin()+p);
        ocvform->free_cells.erase(ocvform->free_cells.begin());        
    }
    //if (ocvform->free_cells.size()==0)
    //    fillcell(ocvform->free_cells[1],ocvform->cols);

    if (ocvform->showmenu)
    {       
        menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();      
        drawcontrolmenu(dst,3);
    }

    imshow("MindOCV", dst);
}

void MainWindow::grab_labels_areas() // grab area under menu and attention labels (for correct overlay in dreamflow)
{  
    menu_area = dst(Rect(ocvform->l_menu_posx-5,ocvform->l_menu_posy-30,ocvform->lmenuw,ocvform->lmenuh)).clone();
}

void MainWindow::stop_all_flows()  // stop all flows
{
    if (ocvform->color_overlay_flow)
    {
        clear_dst = dst.clone();
        ocvform->setcameracheckbox(false);
        if (ocvform->camerainp)
        {
            cam.release();
            ocvform->camerainp = false;
            ocvform->updateformvals();
        }
        ocvform->color_overlay_flow = false;
    }
    if (ocvform->dreamflow)
        ocvform->start_stop_dreamflow(false);
    if (ocvform->puzzleflow_on)
        ocvform->start_stop_puzzleflow(false);
    if (ocvform->showmenu)
        drawcontrolmenu(dst,0);
    ocvform->drawbrushcontour = true;
    ocvform->currfilttype = ocvform->totalfilts-1;
    ocvform->updateformvals();
    ocvform->blocktabs(0);
}

void MainWindow::keys_processing()      // processing keys pressing
{
    char key = cv::waitKey(5) % 256;
    if (key == 't') // test stuff button    
    {
      //  neurostyle();
    }
    else if (key == 'w')      // swap main and overlay pics
        swap_main_overlay();
    else if (key == 'a') // save and add current overlay to pictures
        save_and_add_overlaypic();
    else if (key == 'q') // save and add current overlay to pictures, set main pic to current overlay
    {
        save_and_add_overlaypic();
        prevmainpic = currmainpic;
        currmainpic = iconsarr[imglist.length()-3];
        lchanged=true;
        rchanged=false;
        defineiconsarr();
        define_riconsarr();
        leftpw->fillpics();
        rightpw->fillpics();
        QString ocvpic=folderpath+"/"+imglist.at(currmainpic);
        src = imread(ocvpic.toStdString());
        addWeighted(src, alphaval, src, 1 - alphaval, 0, dst);
        imshow("MindOCV", dst);
    }
    else if ((key == ' ') && (!ocvform->color_overlay_flow)) // turn on continuous filtering with mouse move mode
        keepfiltering = !keepfiltering;
    else if (key == 'c') // transfer current overlat picture to MindPlay and MindDraw windows
    {
        if (plotw->start)
        {
            QPixmap pm = QPixmap::fromImage(Mat2QImageRGB(dst));
            plotw->setbackimage(pm,true);
        }
        if (paintw_started)
        {
            QPixmap pm = QPixmap::fromImage(Mat2QImageRGB(dst));
            paintw->setbackimage(pm);
        }
    }
    else if (key == 'v')  // show/hide openCV filters control form
    {
        if (ocvform->formshown)
            ocvform->hide();
        else
            ocvform->show();
        ocvform->formshown = !ocvform->formshown;
    }
    else if (key == 27)  // 'ESC' press start/stop overlay-hue flow / dreamflow
        stop_all_flows();
    else
    if (key == '0')         // shortcuts for setting Border values
        setborder(100);
    else if (key == '9')
        setborder(90);
    else if (key == '8')
        setborder(80);
    else if (key == '7')
        setborder(70);
    else if ((key == '1') && (!ocvform->color_overlay_flow)) // start color-overlay flow
    {
        stop_all_flows();
        ocvform->color_overlay_flow = true;
        picfilt->start();
        keepfiltering = false;
        ocvform->setcameracheckbox(true);
        if (ocvform->showmenu)
            drawcontrolmenu(dst,1);
        imshow("MindOCV", dst);
        ocvform->blocktabs(1);
    }
    else if ((key == '2') && (!ocvform->dreamflow)) // start dreamflow
    {
        stop_all_flows();
        picfilt->stop();
        if (ocvform->showmenu)
            drawcontrolmenu(dst,2);
        ocvform->drawbrushcontour = false;
        ocvform->updateformvals();
        ocvform->start_stop_dreamflow(true);
        ocvform->blocktabs(2);
    }
    else if ((key == '3') && (!ocvform->puzzleflow_on)) // start puzzle gathering flow
    {
        stop_all_flows();
        picfilt->stop();
        if (ocvform->showmenu)
            drawcontrolmenu(dst,3);
        ocvform->drawbrushcontour = false;
        ocvform->updateformvals();
        ocvform->start_stop_puzzleflow(true);
        ocvform->blocktabs(3);
    }
    else if (key == '5')
    {
        ocvform->drawbrushcontour=!ocvform->drawbrushcontour;
        ocvform->updateformvals();
        imshow("MindOCV", dst);
    }
    else if (key == '6')
    {
        ocvform->plotdroprect=!ocvform->plotdroprect;
        ocvform->updateformvals();
    }
    else if (key == '-')                        // hide / show menu
        ocvform->showmenu = !ocvform->showmenu;        
    else if ((key == 'z') && (!ocvform->color_overlay_flow))     // change filter on the left one
    {
        if (ocvform->currfilttype==1)
            ocvform->currfilttype=ocvform->totalfilts;
        else
            ocvform->currfilttype--;
        ocvform->updateformvals();
    }
    else if ((key == 'x') && (!ocvform->color_overlay_flow))     // change filter on the right one
    {
        if (ocvform->currfilttype==ocvform->totalfilts)
            ocvform->currfilttype=1;
        else
            ocvform->currfilttype++;
        ocvform->updateformvals();
    }
    else if ((key == 'k') && (ocvform->color_overlay_flow))      // start / stop receiving camera input for overlay flow
    {
        if (!ocvform->camerainp)
        {
            cam.open(0);
            ocvform->camerainp = true;
            ocvform->updateformvals();
        } else
        {
            cam.release();
            ocvform->camerainp = false;
            ocvform->updateformvals();
        }
    }      
    else if ((key == '.') && (ocvform->transp>1))      // decrease transparency for mixer filter
    {
        ocvform->transp--;
        ocvform->updateformvals();
    }
    else if ((key == '/') && (ocvform->transp<99))     // increase transparency for mixer filter
    {
        ocvform->transp++;
        ocvform->updateformvals();
    }    
    else if (key == 'u')
    {
        ocvform->hueonly=!ocvform->hueonly;
        ocvform->updateformvals();
    }
    else if (key == 'h')                   // story mode: main pic is fixed, overlay pic change by attention > border
        storymode=!storymode;
    else if ((key == 'g') && (!ocvform->color_overlay_flow)) // start / stop timer for pics change
    {
        ocvform->changepic_bytime=!ocvform->changepic_bytime;
        if (ocvform->changepic_bytime)
            ocvform->pichngT->start();
        else
            ocvform->pichngT->stop();
        ocvform->updateformvals();
    }    
    else if (key == 'f')                   // start / stop timer for focused dreamflow
    {        
        if (!focuseddflow_timer->isActive())
            focuseddflow_timer->start();
        else
            focuseddflow_timer->stop();
    }
    else if ((key == 'd') && (!ocvform->color_overlay_flow)) // turn on / off multi-dreamflow
    {
        ocvform->multi_img_dflow = !ocvform->multi_img_dflow;
        ocvform->updateformvals();
    }
    else if (key == 'r')                   // extend / shorten raw plot form
    {
        if (rawplotshort)
        {
            rs->setGeometry(0,0,1940,70);
            rs->move(0,0);
            rawplotshort = false;
        }
        else
        {
            rs->setGeometry(0,0,1600,70);
            rs->move(158,0);
            rawplotshort = true;
        }
        rs->changefsize(!rawplotshort);
    }
    else if (key=='l')  // show brain levels form
    {
        br_levels->show();
        set_showlevelscheckbox(true);
    }
    else if (key=='m')  // show main window
    {
        QApplication::setActiveWindow(this);
    }
}

void MainWindow::picfiltUpdate() // function for MindOCV hue-overlay flow updates
{
    if (ocvform->color_overlay_flow)
    {
        // change of HUE values untill previous ~ new one, then can change previous HUE
        if (abs(curhue-prevhue)<2)
            canchangehue=true;
        else
        { 
            if (curhue<prevhue)
                prevhue-=2;
            else
                prevhue+=2;
            hue_elem=prevhue;
            processingcolor(hue_elem);
        }

        // change of overlay values until previous ~ new one, then can change previous overlay
        //if ((abs(curoverl-prevoverl)==0) || ((abs(curoverl-prevoverl)==1)))
        if (abs(curoverl-prevoverl)<2)
            canchangeoverlay=true;
        else
        {
            if (curoverl<prevoverl)
                prevoverl-=2;
            else
                prevoverl+=2;
            overlay_elem=prevoverl;
            processingoverlay(overlay_elem);
        }

        checkoverlay(); // check if should change pictures        
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    // choose folder for pictures, make icons for new folder
    // define new main and overlay pics, fill vectors for left and right panels
    QString fPath=QFileDialog::getExistingDirectory(this, "Folder with .jpgs", "D://");
    QDir fd(fPath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    if (imglist.length()<1)
    {
        QMessageBox msgBox;
        msgBox.setText("Please choose a folder with .jpg files!");
        msgBox.exec();
        on_pushButton_7_clicked();
    }
    else
    {
        setfolderpath(fPath);
        saveFolderpath();
        if (plotw->paintfstart)
            paintw->setpicfolder(folderpath);

        imgarray.clear();

        ui->pushButton_7->setEnabled(false);
        ui->pushButton_6->setEnabled(false);
        ocvform->histFeaturesReady=false;
        if (opencvstart)
            emit ocvform->flow_direction_available();
        makeIconsAndHists();

        firstrun=true;
        currmainpic = qrand() % imglist.length();
        lchanged=false;
        rchanged=false;
        prevmainpic = -1;
        prevoverpic = -1;
        iconsarr.clear();
        riconsarr.clear();
        defineiconsarr();
        opencvpic = folderpath+"/"+imglist.at(currmainpic);
        src = imread(opencvpic.toStdString());
        image = imread(opencvpic.toStdString());
        cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        cv::resize(image, image, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        curroverpic = iconsarr[qrand() % (imglist.length()-1)];
        define_riconsarr();
        QString stp = folderpath+"/"+imglist.at(curroverpic);
        srccopy =  imread(stp.toStdString());
        cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        firstrun = false;
        defineiconsarr();
        init_img_set();
        ocvform->changerandpic();
    }
}

void getsvdimage(int r) // get image from SVD compressed to rank r
{
    for (int k=0; k<3; k++)
    {

        t_W=svd_W[k](Range(0,r),Range(0,r));
        t_u=svd_u[k](Range::all(),Range(0,r));
        t_u.convertTo(t_u,CV_32FC1);
        t_vt=svd_vt[k](Range(0,r),Range::all());
        t_vt.convertTo(t_vt,CV_32FC1);
        resimg[k]=t_u*t_W*t_vt;
    }

    channels.clear();
    channels.push_back(resimg[0]);
    channels.push_back(resimg[1]);
    channels.push_back(resimg[2]);
    merge(channels, svd_img);
    imshow("MindOCV", svd_img);
}

void dosvdtransform() // compute SVD vectors for each color channel of image
{
    Mat tt;
    src.convertTo(tt, CV_32FC3, 1.0/255);
    split(tt,trimg);
    for (int k=0; k<3; k++)
    {
        svdtr.compute(trimg[k],svd_w[k],svd_u[k],svd_vt[k]);
        svd_W[k] = Mat::zeros(svd_w[k].rows,svd_w[k].rows,CV_32FC1);
        for(int i=0; i<svd_w[k].rows; i++)
            svd_W[k].at<float>(i,i)=svd_w[k].at<float>(i);
    }
}

vector<float> gethistogram(Mat image, Mat maskx)
{
    int hb = 8, sb = 16, vb = 16;
    int histSize[] = {hb, sb, vb};
    float hranges[] = {0, 180};
    float sranges[] = {0, 256};
    float vranges[] = {0, 256};
    const float* ranges[] = {hranges, sranges, vranges};
    Mat hist;
    int channelst[] = {0, 1, 2};
    cv::calcHist(&image, 1, channelst,maskx,hist,3,histSize,ranges,true,false);
    cv::normalize(hist, hist, 1, 0, NORM_L2, -1, Mat());
    hist = hist.reshape(0,1);
    vector<float> arr;
    for (int i = 0; i < hist.rows; i++)
        arr.insert(arr.end(), hist.ptr<float>(i), hist.ptr<float>(i)+hist.cols);
    return arr;
}

float MainWindow::chi2_distance(vector<float> f1, vector<float> f2)
{
    float res = 0;
    for (int i=0; i<(int)f1.size(); i++)
        res += pow(f1[i]-f2[i],2)/(f1[i]+f2[i]+1e-8);
    return res / 2;
}

void MainWindow::gethistfeatures() // function to get histogram representation of image with 5 regions split
{
    // based on article of Adrian Rosebrock
    // https://www.pyimagesearch.com/2014/12/01/complete-guide-building-image-search-engine-python-opencv/
    cvtColor(pichist,imghist,COLOR_RGB2HSV);
    vector<float> features, ftemp;
    int h = ocvform->picheight;
    int w = ocvform->picwidth;
    int cX = h/2; int cY = w/2;

    int regions[4][4] =
    {{0, cX, 0, cY},
     {cX, w, 0, cY},
     {cX, w, cY, h},
     {0, cX, cY, h}};

    int axesX = int(w * 0.75) / 2;
    int axesY = int(h * 0.75) / 2;

    Mat maskellip(imghist.size(), CV_8U, Scalar(0));
    cv::ellipse(maskellip, Point(cX, cY), Size(axesX, axesY), 0, 0, 360, Scalar(255), -1);    

    for (int i=0; i<4; i++)
    {
        Mat maskrect(imghist.size(), CV_8U, Scalar(0));
        cv::rectangle(maskrect,Point(regions[i][0],regions[i][2]),Point(regions[i][1],regions[i][3]),Scalar(255),-1);
        cv::subtract(maskrect,maskellip,maskrect);
        ftemp = gethistogram(imghist,maskrect);
        features.insert(features.end(),ftemp.begin(),ftemp.end());
    }
    ftemp = gethistogram(imghist,maskellip);

    features.insert(features.end(),ftemp.begin(),ftemp.end());
    hist_features.push_back(features);
}

void MainWindow::getchi2dists(int t)
{
    chi2distances.clear(); 

    for (int i=0; i<imglist.length(); i++)
        if (i!=t)
            chi2distances.push_back(pairt(i,chi2_distance(hist_features[t],hist_features[i])));

    std::sort(chi2distances.begin(), chi2distances.end(),[](const pairt& l, const pairt& r)
    {
        if (l.second != r.second)  return l.second < r.second;
            return l.first < r.first;
    });

    int tl = chi2distances.size();
    for (int i=0; i<ocvform->allngb; i++)
    {
        nearest_pics[i]=chi2distances.at(i).first;
        farest_pics[i]=chi2distances.at(tl-i-1).first;
    }
}

void MainWindow::set_showlevelscheckbox(bool fl)
{
    ui->checkBox_2->setChecked(fl);
}

void MainWindow::on_checkBox_clicked()
{
    if (ui->checkBox->isEnabled())
        plotw->oscstreaming = !plotw->oscstreaming;
}

void MainWindow::on_checkBox_2_clicked()
{
    if (ui->checkBox_2->isChecked())
    {
        plotw->doplot();
        plotw->paintfstart=true;
        br_levels->show();
    }
    else
        br_levels->hide();
}

void MainWindow::on_pushButton_8_clicked()
{
    simeeg = true; plotw->simeeg = true;
    plotw->imlength = 250;
    simulated_eeg = true;
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_8->setEnabled(false);
    rs->show();
    rs->starting();
    printdata("Waves Generator mode is activated!");
    ui->checkBox->setEnabled(true);
    ui->checkBox_2->setChecked(true);
    on_checkBox_2_clicked();
    simulateEEG->start();
    plotw->mental_activations->start();
    printdata("");
    printdata("Use RightMouse click - start/pause brain waves flow in MindPlay");
}

void MainWindow::on_pushButton_9_clicked()
{
    statsWin->update_filenames();
    if (statsWin->recordsfound)
        statsWin->update_plots(0,0);
    stsecond->update_filenames();
    if (stsecond->recordsfound)
        stsecond->update_plots(0,0);
    statsWin->show();
}

void MainWindow::on_pushButton_10_clicked()
{
    openglw->startflow(25);
    openglw->show();
}
