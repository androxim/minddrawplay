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
#include "opencv2/highgui.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <thread>
#include "qmath.h"
#include <QDate>
#include "algorithm"

// TO DO:

using namespace cv;
typedef std::pair<int,float> pairt;

// ==== openCV functions ====
void Processing();
void ProcessingMix();
void Hue( int, void* );
void Saturation( int, void* );
void Value( int, void* );
void Attent( int, void* );
void Overlay( int, void* );
void Border( int, void* );
void onMouse(int event, int x, int y, int flags, void* );

void dosvdtransform();
void getsvdimage(int r);
vector<float> gethistogram(Mat image, Mat maskx);
void applyfilt(int type, Rect rt);
void fillpolygon(Mat im);
Mat adaptivemask(Rect rt);

Mat dilate(Rect srcRect);               // dilation filter
Mat waves(Rect srcRect);                // waves filter
Mat cartoon(Rect srcRect, int ksize);   // cartoonize filter
Mat orbdetect(Rect srcRect);            // ORB features detector
Mat mixfilt(Rect srcRect);              // mixer of pics with transparency
// ==== openCV functions end ====

// ==== openCV variables ====
// different Mat variables, main of them:
// src - main pic on which HUE change applying, chosen from left panel, more clear when attention high
// srccopy - overlay pic, chosen from right panel, more clear when attention low
// dst - resulting overlay pic for area filtering
Mat src, srccopy, dst, dstcopy, prev_dst, clear_dst, img, image, dstg, edges, mask, trp, randpic;
Mat tempimg, dstemp, dst0, srg, srct, srwt, dstt, svd_img, gray_element, element, dream0, imghist, pichist;
int currmainpic, curroverpic, prevmainpic = -1, prevoverpic = -1;
Rect df_srcDstRect;

bool firstrun = true;
bool estattention = false;  // if attention is streaming from MindWave device
bool fullscr = false;
bool mwconnected = false;

int elem1 = 255;  // HUE variable
int elem2 = 255;  // Saturation
int elem3 = 255;  // Value
int elem4 = 50;   // Attention
int elem5 = 50;   // Overlay
int elem6 = 90;   // Border for change of pictures
int const max_elem = 500;
int const max_elem2 = 100;

VideoCapture cam;
vector<vector<float>> hist_features; // vector of histogramm features for all pics
vector<pairt> chi2distances; // dict of chi2 distances between selected and other pics
bool canchangepic = true; // to prevent constant change of pics when attention > border => can change pics only when new attention became lower than border
bool activeflow = false; // if false - mode when hue/overlay flow is not started / paused and region filtering is available
bool keepfiltering = false; // additional mode to "!activeflow" with continous filtering with mouse move, activated by "space" press

double alphaval = 0.5; // transparency variable for overlay
int curr_iter = 0; // variable defines how often filter is applied during mousemove
// ==== openCV variables end ====

// ==== SVD variables ====
cv::SVD svdtr;
Mat trimg[3], resimg[3];
Mat svd_w[3], svd_u[3], svd_vt[3], svd_W[3];
Mat t_W,t_u,t_vt;
int svdt = 80; // number of eigen values for picture compression
// ==== SVD variables end ====

leftpanel *leftpw;          // left panel form for pictures choice
rightpanel *rightpw;        // right panel form for pictures choice
rawsignal *rs;              // raw signal form
ocvcontrols *ocvform;       // openCV filters control form

vector <int> iconsarr, riconsarr;   // vectors of right and left panels pictures
void defineiconsarr();              // function for updationg vector of left panel pictures
void define_riconsarr();            // function for updationg vector of right panel pictures
void shuffleicons(bool left);       // function for random shuffling of panels pictures
bool rchanged, lchanged;

QString folderpath;     // path to pictures folder
QStringList imglist;    // list of pictures paths
QStringList strList1;
QStringListModel *strListM1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{     
    setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);

    plotw = new plotwindow();
    plotw->setWindowTitle("MindDrawPlay beta 2.4 | MindPlay");
    plotw->setFixedSize(1560,978);
    plotw->move(QApplication::desktop()->screen()->rect().center() - plotw->rect().center()-QPoint(10,30));
    plotw->start=false;

    paintw = new paintform();
    paintw->setWindowTitle("MindDrawPlay beta 2.4 | MindDraw");
    paintw->pw=plotw;
    paintw->mww=this;
    paintw->setFixedSize(1560,970);    

    rs = new rawsignal();           // raw signal form
    rs->setGeometry(0,0,1600,80);
    rs->move(158,0);
    plotw->rws=rs;

    leftpw = new leftpanel();
    leftpw->mww = this;
    leftpw->setFixedSize(148,1030);
    leftpw->move(QPoint(0,0));

    rightpw = new rightpanel();
    rightpw->mww = this;
    rightpw->setFixedSize(148,1030);
    rightpw->move(QPoint(1769,0));

    ocvform = new ocvcontrols();    // openCV filters controls form
    ocvform->mww = this;
    ocvform->setFixedSize(736,130);
    ocvform->move(QPoint(278,844));
    ocvform->leftpan = leftpw;

    folderpath="D:/PICS";       // default path for pictures
    QDir fd(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    leftpw->imgnumber = imglist.length()-2;     // -2 because excluding current main and overlay pics
    rightpw->imgnumber = imglist.length()-2;    
    picsarr = vector<int>(imglist.length());
    ocvform->randpicn = qrand()%imglist.length();
    ui->lineEdit->setText(folderpath);

    ocvcontrshow = true;             // if openCV filters control form shown

    pwstart = false;                 // MindPlay window run detector
    paintw_started = false;                 // MindDraw window run detector
    opencvstart = false;             // MindOCV window run detector
    storymode = false;

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

    mindwt = new QTimer(this);  // timer for processing data from MindWave device
    mindwt->connect(mindwt, SIGNAL(timeout()), this, SLOT(mindwtUpdate()));
    mindwt->setInterval(0);

    // ==== set of variables for simulated EEG data mode (in development) ====
    srfr = 500; // sampling rate
    deltaphs = 4; thetaphs = 8; alphaphs = 0; betaphs = 7; gammaphs = 8;
    deltafr = 2; thetafr = 5; alphafr = 9; betafr = 21; gammafr=33; hgammafr=64;
    zdeltaamp = 7; zthetaamp = 7; zalphaamp = 7; zbetaamp = 7; zgammaamp = 5; zhgammaamp = 3;
    currentel = 0; currentsimdata = 0;
    simulateEEG = new QTimer(this);
    simulateEEG->connect(simulateEEG,SIGNAL(timeout()), this, SLOT(simulateEEGUpdate()));
    simulateEEG->setInterval(1);
    simeeg = false; plotw->simeeg = false;
    // simulateEEG->start();
    // ==== set of variables for simulated EEG data mode end ====

    canchangehue = true;  // HUE and Overlay values are changing from previous to new values in cycle
    canchangeoverlay = true;
    curhue = prevhue = 255;
    curoverl = prevoverl = 50;   

    opencvinterval = 25;      // timer for openCV transitions in HUE, overlay flow
    picfilt = new QTimer(this);
    picfilt->connect(picfilt,SIGNAL(timeout()), this, SLOT(picfiltUpdate()));
    picfilt->setInterval(opencvinterval);    

    transfert_interval = 50;      // timer for camera flow
    cameraflow = new QTimer(this);
    cameraflow->connect(cameraflow,SIGNAL(timeout()), this, SLOT(transfert_Update()));
    cameraflow->setInterval(transfert_interval);

    dreamflow_timer = new QTimer(this);     // timer for auto dreamflow mode, when new picture appears by fragments
    dreamflow_timer->connect(dreamflow_timer,SIGNAL(timeout()), this, SLOT(dreamflow_Update()));
    dreamflow_timer->setInterval(ocvform->dreamflowrate);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());      

    makeicons(); // function for making icons of picturs
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
            dstt = cartoon(rt,ocvform->kernel_s);
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
    }
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

void Border(int, void *) {  }

void Overlay( int, void* )
{
   // getsvdimage(elem5);
    if (!ocvform->hueonly)
        ProcessingMix();
}

void Attent( int, void* ) { }

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

void Processing() // processing of HUE, Saturation, Value changes
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

            if (cur1 < 0) cur1 = 0; else if (cur1 > 255) cur1 = 255;
            if (cur2 < 0) cur2 = 0; else if (cur2 > 255) cur2 = 255;
            if (cur3 < 0) cur3 = 0; else if (cur3 > 255) cur3 = 255;

            img.at<Vec3b>(Point(y,x))[0] = cur1;
            img.at<Vec3b>(Point(y,x))[1] = cur2;
            img.at<Vec3b>(Point(y,x))[2] = cur3;
        }
    }

    cvtColor(img,image,COLOR_HSV2RGB);

    if (!ocvform->hueonly)
    {
        if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
        {
            cam>>trp;
            cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
            addWeighted(image, alphaval, trp, 1 - alphaval, 0, dst);
        }
        else
            addWeighted(image, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("image", dst);
    }
    else
        imshow( "image", image );
}

void ProcessingMix() // processing of overlay changes, alphaval - transparency
{
    alphaval = (double) elem5 / 100;    
    if (estattention)   // if MindWave connected and attention values are streaming
    {               
        if (activeflow)                   
        {
            if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
            {
                cam>>trp;
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
    imshow("image", dst);
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

void onMouse( int event, int x, int y, int flags, void* )   // Mouse clicks and moves processing
{
    // !dofiltering - mode when flow is not started / paused and region filtering is available
    if ((!activeflow) && (event == EVENT_MOUSEMOVE) && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {
        // draw brush contour
        if (ocvform->drawbrushcontour)
        {
            dst0.release();
            dst0 = dst.clone();
            circle(dst0, Point(x, y), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),0,LINE_AA);
            imshow("image", dst0);
        }
    }
    if ((!activeflow) && (event == EVENT_MOUSEMOVE) && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {
        ocvform->currmousepos.setX(x);
        ocvform->currmousepos.setY(y);
    }
    if ((!activeflow) && (event == EVENT_MOUSEMOVE) && ((flags ==  EVENT_FLAG_LBUTTON) || (keepfiltering)) && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {      
        curr_iter++; // determines how often with mouse moves will be doing filtering
        if (curr_iter >= ocvform->currfilterrate)
        {
            Rect srcDstRect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea);
            dstt = dst(srcDstRect); // dst - full resulting overlay pic of main (src) and overlay (srccopy)

            applyfilt(ocvform->currfilttype,srcDstRect);

            Mat mask_image(dstt.size(), CV_8U, Scalar(0)); // mask to have only circle of region                        
            circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);

            if (ocvform->currfilttype==5)
                addWeighted(dst(srcDstRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

            dstt.copyTo(dst(srcDstRect),mask_image);
            imshow("image", dst);
            curr_iter=0;
        }
    }
    if ((!activeflow) && (event == EVENT_LBUTTONDOWN)  && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {
        Rect srcDstRect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea);
        dstt = dst(srcDstRect); // dst - full resulting overlay pic of main (src) and overlay (srccopy)

        if ((ocvform->currfilttype==5) && (ocvform->mixtype==3) && (ocvform->changerandpic_byclick))
            ocvform->changerandpic();

        applyfilt(ocvform->currfilttype,srcDstRect);

        Mat mask_image( dstt.size(), CV_8U, Scalar(0));                
        circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);

        prev_dst = dst.clone(); // for keeping state before last action (using in "cancel last")

        if (ocvform->currfilttype==5)
            addWeighted(dst(srcDstRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

        dstt.copyTo(dst(srcDstRect),mask_image);
        imshow("image", dst);   
    } else
    if (event == EVENT_MBUTTONDOWN)
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
        cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
        if ((activeflow) && (!ocvform->hueonly))
        {
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
            imshow("image", dst);
        }
    } else
    if ((event == EVENT_RBUTTONDOWN) && (activeflow))
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
        if (!ocvform->hueonly)
        {
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
            imshow("image", dst);
        }
    } else
    if (event == EVENT_LBUTTONDBLCLK)
    {
        // fullscreen mode activation
        if (!fullscr)
        {
            rs->setGeometry(0,0,1940,80);
            rs->move(0,0);
            ocvform->move(QPoint(-10,968));
            cv::setWindowProperty("image",cv::WND_PROP_FULLSCREEN,1);
            fullscr=true;
        }
        else
        {
            rs->setGeometry(0,0,1600,80);
            rs->move(158,0);
            ocvform->move(QPoint(278,844));
            cv::setWindowProperty("image",cv::WND_PROP_FULLSCREEN,0);
            fullscr=false;
        }
        rs->changefsize(fullscr);
    }
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

Mat cartoon(Rect srcRect, int ksize) // cartoonize filter
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

void MainWindow::Webcamsource() // grab video from camera
{    
    cam>>trp;
    cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
    src = trp;
   // cam>>dst;
   // imshow("image", dst);
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
    if (paintw_started)
        paintw->setbackimageocv(ocvpic);
}

int MainWindow::getchi2distsize()       // get size of vector with chi2dist
{                                       // need to check if switch between flow direction invoked before any distances computed
    return chi2distances.size();
}

QString MainWindow::getimagepath(int t) // return image path for ocvcontrol form in update random image
{
    return folderpath+"/"+imglist.at(t);
}

void MainWindow::setdream0() // grab initial image before starting expanding mode in dreamflow
{
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
    rectangle(dst, Point(x-2,y-2), Point(x+w+2,y+h+2), cv::Scalar(255,255,255),2);
    imshow("image",dst);
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
    opencvinterval=i;
    picfilt->setInterval(opencvinterval);
}

void MainWindow::cancellast()   // cancel last filtering action
{
    dst=prev_dst;
    imshow("image", dst);
}

void MainWindow::cancelall()    // cancel all filtering actions
{
    dst=clear_dst.clone();
    imshow("image", dst);
}

void MainWindow::dreamflow_Update() // timer for dreamflow mode, when new pic appears by random fragment over old
{
    int x,y, area;
    Rect srcDstRect;
    if (!ocvform->dropsmode)
    {
        x = ocvform->currfilterarea/2 + qrand() % (dst.cols - ocvform->currfilterarea);
        y = ocvform->currfilterarea/2 + qrand() % (dst.rows - ocvform->currfilterarea);
        srcDstRect = Rect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea);
    } else
    {
        // define area inside current expanding window
        if (ocvform->drops_byatt)
            area = (((ocvform->x_right-ocvform->x_left)/2-5)*elem4)/100+3;
        else
            area = qrand()%((ocvform->x_right-ocvform->x_left)/2-5)+3;
        x = ocvform->x_left + area/2 + qrand() % (ocvform->x_right - ocvform->x_left - area); // define rect based on area size (inside expanding window)
        y = ocvform->y_top + area/2 + qrand() % (ocvform->y_bottom - ocvform->y_top - area);
        srcDstRect = Rect(x-area/2, y-area/2, area, area);
    }
    dstt = mixfilt(srcDstRect);
    Mat mask_image(dstt.size(), CV_8U, Scalar(0));
    if (!ocvform->polygonmask)
    {
        if (!ocvform->dropsmode)
            circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);
        else
            circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), area/2, CV_RGB(255, 255, 255),-1,LINE_AA);
    }
    else
        fillpolygon(mask_image);
    addWeighted(dst(srcDstRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);
    dstt.copyTo(dst(srcDstRect),mask_image);

    imshow("image", dst);
}

void MainWindow::transfert_Update() // streams pic from camera to MindPlay window
{
    Webcamsource();
   // plotw->setbackfromcamera();
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
    imshow("image", dst);
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

void MainWindow::updatemainpic(int num)
{           
    // function for dbclick on left panel - updating main pic and vectors of icons
    // also MindPlay and MindDraw pictures, if corresponded windows opened
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
    if ((activeflow) && (!ocvform->hueonly))
    {
        if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
        {
            cam>>trp;
            cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
            addWeighted(src, alphaval, trp, 1 - alphaval, 0, dst);
        }
        else
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("image", dst);
    }
    if ((!activeflow) && (ocvform->currfilttype==5) && (ocvform->mixtype==3))
        ocvform->setcurrdream(currmainpic);
    if (plotw->start)
        plotw->grabopencv(ocvpic);
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
    if ((activeflow) && (!ocvform->hueonly))
    {
        addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
        imshow("image", dst);
    }
}

void MainWindow::printdata(QString str) // updating execution log
{   
    strList1.push_front(str);    
    strListM1->setStringList(strList1);
}

void MainWindow::on_pushButton_clicked() // close the App
{
    if (mwconnected)
        TG_FreeConnection( connectionId );
   // plotw->cleanmem();  
    QApplication::quit();
}

void MainWindow::on_pushButton_3_clicked() // MindPlay window run
{ 
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

void MainWindow::on_pushButton_2_clicked() // BCI2000 window run
{          
    bool ok1,ok2;
    stringstream str1, str2;
    str1 << "Enter sampling rate: ";
    int srate = QInputDialog::getInt(this,"Sampling rate",str1.str().c_str(), 512, 1, 1024, 1, &ok1);
    if (ok1)
        plotw->srfr=srate;
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
            plotw->srfr=512;
            ui->pushButton_5->setEnabled(false);
            ui->label->setText("Channel number: "+QString::number(channel));
            ui->label->setVisible(true);
            bciconnect = true;
            ui->pushButton_4->setEnabled(false);            
        } else
            printdata("Connection with BCI2000 not established.. ( ");
    }
}

void MainWindow::on_pushButton_4_clicked() // MindDraw window run
{
    on_pushButton_3_clicked();
    //plotw->hide();
    paintw->scene->init(plotw, this);  
    plotw->pssstart=true;
    paintw->scene->clear();
    paintw->show();
    paintw->loadempty();
   // paintw->startpolyt();
    connectWin->ps=paintw->scene;
    if (plotw->start)    
        plotw->pss=paintw->scene;    
}

void MainWindow::on_pushButton_5_clicked()
{
    mindwaveconnect();
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
        simulateEEG->stop();
        simeeg=false;
        plotw->srfr=512;
        plotw->imlength=256;
        mindwt->start();
        plotw->mindwstart=true;
        plotw->simeeg=false;
        rs->starting();
        ui->pushButton_5->setEnabled(false);
        if (!estattention)
            estattention=true;
        rs->show();
        mwconnected=true;
    }
    else
        printdata("Connection with MindWave could not be established! :(");
    packetsRead = 0;
}

void MainWindow::setsourceimg(QString fpath) // set openCV main image from path
{
    src = imread(fpath.toStdString());
    cv::resize(src, src, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
    imshow("image", src );
}

void MainWindow::setsourceimgd(QImage qp) // set openCV main image from QImage (MindPlay/MindDraw windows)
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

    opencvinterval = 25;// + (100 - elem4)/2;
    picfilt->setInterval(opencvinterval);

    if ((ocvform->attmodul_area) && (!activeflow))
    {
        // attention modulated filter area with minimum area value, only not activeflow mode
        if (elem4<20)
            elem4=20;
        ocvform->currfilterarea=elem4*5;
        ocvform->updateformvals(); // updating values on openCV filter control form
    }
    if ((!activeflow) && (ocvform->currfilttype==5) && (ocvform->mixtype==3) && (ocvform->changebyattention))
    {
        if (elem4>elem6)
            ocvform->changerandpic();        
    }
    if ((!activeflow) && (ocvform->currfilttype==5) && (ocvform->mixtype==3) && (ocvform->dreamflow))
    {
        if (ocvform->attent_modulated_dreams)
        {
            ocvform->dreamflowrate=105-elem4;
            dreamflow_timer->setInterval(ocvform->dreamflowrate);
            ocvform->updateformvals();
        }
        if (ocvform->drops_byatt)
        {
            ocvform->drops_interval=120-elem4; // 30+elem4/2;
            ocvform->dropsT->setInterval(ocvform->drops_interval);
            ocvform->updateformvals();
        }
        if (ocvform->poly_by_att)
        {
            ocvform->pointsinpoly = elem4 / 10 + 3;
            ocvform->updateformvals();
        }
        if (ocvform->directionswitch_by_att)
        {
            if (elem4>elem6)
                ocvform->flowdirection=1;
            else
                ocvform->flowdirection=-1;
            ocvform->updateformvals();
        }
    }
    if ((!activeflow) && (ocvform->currfilttype==5) && (ocvform->transp_by_att))
    {
        ocvform->transp = 100 - elem4/2;
        ocvform->updateformvals();
    }
}

void MainWindow::setoverlay(int i)
{
    elem5=i;
    setattent(elem5);
    setTrackbarPos("Overlay","image",elem5);
    checkoverlay();
}

void MainWindow::setborder(int i)
{
    elem6=i;
    setTrackbarPos("Border","image",elem6);
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
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Choose a folder with .jpg files!");
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
        namedWindow("image",WINDOW_NORMAL + WINDOW_FREERATIO);  // WINDOW_OPENGL);

        createTrackbar("Hue","image",&elem1, max_elem,Hue);
        // createTrackbar("Saturation","image",&elem2, max_elem,Saturation);
        // createTrackbar("Value","image",&elem3, max_elem,Value);        
        createTrackbar("Overlay","image",&elem5,max_elem2,Overlay);
        createTrackbar("Attention","image",&elem4,max_elem2,Attent);
        createTrackbar("Border","image",&elem6,max_elem2,Border);

        setMouseCallback( "image", onMouse, 0 );

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

        curroverpic = iconsarr[qrand() % (imglist.length()-1)];
        define_riconsarr();
        QString stp = folderpath+"/"+imglist.at(curroverpic);
        srccopy = imread(stp.toStdString());
        cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);

        firstrun=false;    
        defineiconsarr();        
        resizeWindow("image",1600,900);
        resize(1600,900);
        moveWindow("image", 150,39);
        setTrackbarPos("Overlay", "image", elem5);
       // imshow("image", src);
        ProcessingMix();

        opencvstart=true;
        plotw->opencvstart=true;
        if (plotw->start)
            plotw->enablehue();
        picfilt->start();

    }
    else
        imshow("image", src);
}

void MainWindow::simulateEEGUpdate() // simulated EEG data (in development)
{
  //  const double mean = 0.0;
  //  const double stddev = 0.5;
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
    currentsimdata = deltaamp*sin(deltafr*2*M_PI/srfr*(currentel+deltaphs)) + thetaamp*sin(thetafr*2*M_PI/srfr*(currentel+thetaphs)) + alphaamp*sin(alphafr*2*M_PI/srfr*(currentel+alphaphs)) + betaamp*sin(betafr*2*M_PI/srfr*(currentel+betaphs)) + gammaamp*sin(gammafr*2*M_PI/srfr*(currentel+gammaphs)) + hgammaamp*sin(hgammafr*2*M_PI/srfr*(currentel+gammaphs)) + nois;
    currentsimdata *= 3;
    if (plotw->start)
        plotw->bcidata(currentsimdata);
    if (paintw_started)
        paintw->scene->getdata(currentsimdata/4);
    //  qDebug()<<currentsimdata;       
}

void MainWindow::checkoverlay()
{
    // check if overlay should happen - if attention > border and it's new overcome of border
    if ((elem5>elem6) && (canchangepic))
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
            }
            else
            {
                getchi2dists(prevoverpic);
                curroverpic = farest_pics[ocvform->ngbarea/5+qrand()%ocvform->ngbarea];
            }

            getchi2dists(curroverpic);
            fillpuzzle_withneighbours();
            fillmaininpuzzle(curroverpic);

            lchanged=true;
            rchanged=true;
            defineiconsarr();
            define_riconsarr();
            leftpw->fillpics();
            rightpw->fillpics();            
            QString ocvpic=folderpath+"/"+imglist.at(curroverpic);
            src=srccopy.clone();
            srccopy = imread(ocvpic.toStdString());            
            cv::resize(srccopy, srccopy, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_LINEAR);
            if (!ocvform->hueonly)
            {
                if (ocvform->camerainp)  // grab input from camera as overlay (strcopy) pic
                {
                    cam>>trp;
                    cv::resize(trp, trp, cv::Size(ocvform->picwidth,ocvform->picheight), 0, 0, cv::INTER_NEAREST);
                    addWeighted(src, alphaval, trp, 1 - alphaval, 0, dst);
                }
                else
                    addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
                imshow("image", dst);
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
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
          //  imshow("image", dst);
        }
        canchangepic=false;
    }
    if ((elem5<elem6) && (!canchangepic))
        canchangepic=true;
}

void MainWindow::makeicons() // make icons of pictures from folder
{
    QString picst;
    imgarray.resize(imglist.length());
    for (int i=0; i<imglist.length(); i++)
    {
        picst = folderpath+"/"+imglist.at(i);                                
        tempimg = imread(picst.toStdString());
        pichist = tempimg;
        gethistfeatures();
        Size size(138,138);        
        cv::resize(tempimg,dstemp,size);
        QImage qm = Mat2QImageRGB(dstemp);
        imgarray[i]=QPixmap::fromImage(qm);      
    }

  /*  QFile outputFile("D:/Androxim/histfeatures.dat");
    outputFile.open(QIODevice::WriteOnly);
    QTextStream fout(&outputFile);
    float tchival;
    for (int i=0; i<imglist.length(); i++)
    {
        tchival = chi2_distance(hist_features[5],hist_features[i]);
        for (int j=0; j<hist_features[i].size(); j++)
        {
            fout << tchival << " ";
            fout << hist_features[i][j] << ",";
        }
        fout<<endl;
    }
    outputFile.close(); */
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
                plotw->bcidata(mw_raw/2);
            if (rs->start)
                rs->updatesignal(mw_raw/2);
            if (abs(mw_raw)<100) // anti blink artifacts for drawing
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
                    plotw->update_attention(mw_atten);
                if ((opencvstart) && (canchangehue))
                {                                                         
                    curhue=100+mw_atten*4;
                    canchangehue=false;
                }                
                if (paintw_started)
                {
                    paintw->updateattentionplot(mw_atten);
                    if (opencvstart)        // update attention by estimated value, but 1 per sec
                    {
                        setattent(paintw->getestattval());
                        curoverl=elem4;
                    }
                }
              //  if (opencvstart)        // update attention by device value, 1 per sec
              //  {
              //      setattent(mw_atten);
              //      curoverl=elem4;
              //  }
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
                if (paintw_started)
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

void MainWindow::keys_processing()      // processing keys pressing
{
    char key = cv::waitKey(5) % 256;
    if (key == 't') // test stuff button    
        ocvform->directionswitch_by_att=!ocvform->directionswitch_by_att;
    else if (key == 'a') // save and add current overlay to pictures
    {
        save_and_add_overlaypic();
    }
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
        imshow("image", dst);
    }
    else if ((key == ' ') && (!activeflow)) // turn on continuous filtering with mouse move mode
        keepfiltering = !keepfiltering;
    else if (key == 'c') // transfer current overlat picture to MindPlay and MindDraw windows
    {
        if (plotw->start)
        {
            QPixmap pm = QPixmap::fromImage(Mat2QImageRGB(dst));
            plotw->graboverlay(pm);
        }
        if (paintw_started)
        {
            QPixmap pm = QPixmap::fromImage(Mat2QImageRGB(dst));
            paintw->setbackimageoverlay(pm);
        }
    }
    else if (key == 'v') // show/hide openCV filters control form
    {
        if (ocvcontrshow)
            ocvform->hide();
        else
            ocvform->show();
        ocvcontrshow=!ocvcontrshow;
    }
    else if (key == 27)  // 'ESC' press start/stop overlay-hue flow / dreamflow
    {
        activeflow=!activeflow;
        if (activeflow)
        {
            ocvform->hide();
            ocvcontrshow=false;
            keepfiltering=false;
            if (ocvform->dreamflow)
                ocvform->stopdreamflow();
            ocvform->setcameracheckbox(true);
        }
        else
        {
            clear_dst = dst.clone();
            ocvform->setcameracheckbox(false);
            ocvform->show();
            ocvcontrshow=true;            
            if (ocvform->camerainp)
            {
                cam.release();
                ocvform->camerainp = false;
                ocvform->updateformvals();
            }            
        }
    }
    else
    if (key == '0')         // shortcuts for setting Border values
        setborder(100);
    else if (key == '9')
        setborder(90);
    else if (key == '8')
        setborder(80);
    else if (key == '7')
        setborder(70);
    else if ((key == '1') && (ocvform->currfilterarea>50))  // decrease filter area
    {
        ocvform->currfilterarea-=10;
        ocvform->updateformvals();
    }
    else if ((key == '2') && (ocvform->currfilterarea<800)) // increase filter area
    {
        ocvform->currfilterarea+=10;
        ocvform->updateformvals();
    }
    else if ((key == '3') && (ocvform->currfilterrate>2))
    {
        // increase filter rate (smaller value - more often filter applied with mouse move)
        ocvform->currfilterrate--;
        ocvform->updateformvals();
    }
    else if ((key == '4')  && (ocvform->currfilterrate<20))  // decrease filter rate
    {
        ocvform->currfilterrate++;
        ocvform->updateformvals();
    }
    else if (key == '5')
    {
        ocvform->drawbrushcontour=!ocvform->drawbrushcontour;
        ocvform->updateformvals();
        imshow("image", dst);
    }
    else if (key == '6')
    {
        ocvform->plotdroprect=!ocvform->plotdroprect;
        ocvform->updateformvals();
    }

    else if ((key == 'z') && (!activeflow))     // change filter on the left one
    {
        if (ocvform->currfilttype==1)
            ocvform->currfilttype=ocvform->totalfilts;
        else
            ocvform->currfilttype--;
        ocvform->updateformvals();
    }
    else if ((key == 'x') && (!activeflow))     // change filter on the right one
    {
        if (ocvform->currfilttype==ocvform->totalfilts)
            ocvform->currfilttype=1;
        else
            ocvform->currfilttype++;
        ocvform->updateformvals();
    }
    else if ((key == 'r') && (activeflow))      // start receiving camera input
    {
        if (!ocvform->camerainp)
        {
            cam.open(0);           
            ocvform->camerainp = true;
            ocvform->updateformvals();
        }
       // cameraflow->start();
    }
    else if ((key == 's') && (activeflow))        // stop receiving camera input
    {
      //  cameraflow->stop();
        cam.release();
        ocvform->camerainp = false;
        ocvform->updateformvals();
    }
    else if (key == 'h')  // story mode: main pic is fixed, overlay pic change by attention > border
        storymode=!storymode;
    else if ((key == '.') && (ocvform->transp>1))        // decrease transparency for mixer filter
    {
        ocvform->transp--;
        ocvform->updateformvals();
    }
    else if ((key == '/') && (ocvform->transp<99))     // increase transparency for mixer filter
    {
        ocvform->transp++;
        ocvform->updateformvals();
    }
    else if ((key == 'm') && (!activeflow)) // change picture for mixer random pic mode
        ocvform->changerandpic();
    else if (key == 'u')
    {
        ocvform->hueonly=!ocvform->hueonly;
        ocvform->updateformvals();
    }
}

void MainWindow::picfiltUpdate() // function for MindOCV hue-overlay flow updates
{        
    try
    {
        keys_processing();
    }
    catch (...)
    {
        qDebug()<<"key process error";
    }

    if (activeflow)
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
            elem1=prevhue;
            setTrackbarPos("Hue", "image", elem1);
        }

        // change of overlay values untill previous ~ new one, then can change previous overlay
        //if ((abs(curoverl-prevoverl)==0) || ((abs(curoverl-prevoverl)==1)))
        if (abs(curoverl-prevoverl)<2)
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

        checkoverlay(); // check if should change pictures
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    // choose folder for pictures, make icons for new folder
    // define new main and overlay pics, fill vectors for left and right panels
    QString fPath=QFileDialog::getExistingDirectory(this, "Get Any Folder", "D://");
    if (fPath!="")
    {
        setfolderpath(fPath);
        makeicons();

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
        shuffleicons(true);
        leftpw->fillpics();
        shuffleicons(false);
        rightpw->fillpics();
        ProcessingMix();
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

    vector<Mat> channels;
    channels.push_back(resimg[0]);
    channels.push_back(resimg[1]);
    channels.push_back(resimg[2]);
    merge(channels, svd_img);
    imshow("image", svd_img);
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
    int hb = 16, sb = 16, vb = 8;
    int histSize[] = {hb, sb, vb};
    float hranges[] = {0, 180};
    float sranges[] = {0, 256};
    float vranges[] = {0, 256};
    const float* ranges[] = {hranges, sranges, vranges};
    Mat hist;
    int channels[] = {0, 1, 2};
    cv::calcHist(&image, 1, channels,maskx,hist,3,histSize,ranges,true,false);
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
    for (int i=0; i<f1.size(); i++)
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
        farest_pics[i]=chi2distances.at(tl-ocvform->allngb+i).first;
    }
}
