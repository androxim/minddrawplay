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
#include "opencv2/highgui.hpp"
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
Mat cartoon(Rect srcRect);              // cartoonize filter
Mat orbdetect(Rect srcRect);            // ORB features detector
Mat mixfilt(Rect srcRect);              // mixer of pics with transparency
Mat ripples(Rect srcRect);             // ripples effect filter
// ==== openCV functions end ====

// ==== openCV variables ====
// different Mat variables, main of them:
// src - main pic on which HUE change applying, chosen from left panel, more clear when attention high
// srccopy - overlay pic, chosen from right panel, more clear when attention low
// dst - resulting overlay pic for area filtering
Mat src, srccopy, dst, dstcopy, prev_dst, clear_dst, img, image, dstg;
Mat edges, mask, trp, randpic, stinp, blob, stpic;
Mat tempimg, dstemp, dst0, dst1, srg, srct, srwt, dstt, svd_img, gray_element;
Mat element, dream0, imghist, pichist, elfont;
vector<Mat> channels;
vector<Mat> curr_img_set; std::set<int> img_num_set;
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
vector<pairt> chi2distances;         // dict of chi2 distances between selected and other pics
bool canchangepic = true;            // to prevent constant change of pics when attention > border => can change pics only when new attention became lower than border
bool color_overlay_flow = false;     // if false - mode when hue/overlay flow is not started / paused and region filtering is available
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

vector <int> iconsarr, riconsarr;   // vectors of right and left panels pictures
void defineiconsarr();              // function for updationg vector of left panel pictures
void define_riconsarr();            // function for updationg vector of right panel pictures
void shuffleicons(bool left);       // function for random shuffling of panels pictures
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
    plotw->paintf = paintw;

    rs = new rawsignal();           // raw signal form
    rs->setGeometry(0,0,1600,80);
    rs->move(158,0);
    // rs->starting();
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
    plotw->ocvf = ocvform;
    paintw->ocvfm = ocvform;

    folderpath = "D:/PICS";       // default path for pictures
    plotw->folderpath = folderpath;
    paintw->setpicfolder(folderpath);
    QDir fd(folderpath);
    imglist = fd.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    leftpw->imgnumber = imglist.length()-2;     // -2 because excluding current main and overlay pics
    rightpw->imgnumber = imglist.length()-2;    
    picsarr = vector<int>(imglist.length());
    ocvform->randpicn = qrand()%imglist.length();
    ui->lineEdit->setText(folderpath);
    for (int i=0; i<imglist.length(); i++)
        imgpaths.push_back(imglist[i]);

    //ocvcontrshow = true;             // if openCV filters control form shown

    pwstart = false;                 // MindPlay window run detector
    paintw_started = false;          // MindDraw window run detector
    opencvstart = false;             // MindOCV window run detector
    storymode = false;               // mode when main pic is fixed, but not overlay
    canchangepuzzle = true;         // when puzzle is completed, to prevent constant pic changes with high attention

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
    deltafr = 3; thetafr = 5; alphafr = 9; betafr = 21; gammafr=33; hgammafr=44;
    zdeltaamp = 5; zthetaamp = 12; zalphaamp = 16; zbetaamp = 30; zgammaamp = 17; zhgammaamp = 12;
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

    opencvinterval = 40;      // timer for key process and color-overlay flow
    picfilt = new QTimer(this);
    picfilt->connect(picfilt, SIGNAL(timeout()), this, SLOT(picfiltUpdate()));
    picfilt->setInterval(opencvinterval);    

    dreamflow_timer = new QTimer(this);     // timer for auto dreamflow mode, when new picture appears by fragments
    dreamflow_timer->connect(dreamflow_timer, SIGNAL(timeout()), this, SLOT(dreamflow_Update()));
    dreamflow_timer->setInterval(ocvform->dreamflowrate);

    int streamflowrate = 100;
    streamflows = new QTimer(this); // timer for streaming flows to MindPlay
    streamflows->connect(streamflows, SIGNAL(timeout()), this, SLOT(streamflows_Update()));
    streamflows->setInterval(streamflowrate);

    puzzleflow = new QTimer(this); // timer for puzzle gathering flow
    puzzleflow->connect(puzzleflow, SIGNAL(timeout()), this, SLOT(puzzleflow_Update()));
    puzzleflow->setInterval(ocvform->puzzleflowrate);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());      

    connect(this,&MainWindow::histFinished,this,&MainWindow::enablenewfolder);

    // making icons of pics (via QtConcurrent), computing color histogram features (in separate thread)
    makeIconsAndHists();
}

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
    imageScaling->setFuture(QtConcurrent::mapped(imglist, rescale));

    histfinished = false; // computing color histogram features in a separate thread
    enablenewfolder();
    thr = std::thread(&MainWindow::makeHistFeatures,this);
    thr.detach();
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
        // case 6: puzzle gathering
        case 7:
        {
            dstt = ripples(rt);
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

    if ((!ocvform->dreamflow) && (ocvform->showlabel))
        sprintf(ocvform->l_str,"Attention: %d",elem4);

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

        if (ocvform->showlabel)
        {
            elfont = dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)).clone();
            if (!ocvform->dreamflow)
                putText(dst, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
        }

        imshow("image", dst);
    }
    else
    {
        if (ocvform->showlabel)
        {
            elfont = image(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)).clone();
            if (!ocvform->dreamflow)
                putText(image, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
        }
        imshow( "image", image );
    }
}

void ProcessingMix() // processing of overlay changes, alphaval - transparency
{
    alphaval = (double) elem5 / 100;    
    if (estattention)   // if MindWave connected and attention values are streaming
    {               
        if (color_overlay_flow)
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

    if (ocvform->showlabel)
    {
        elfont = dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)).clone();
        if (!ocvform->dreamflow)
        {
            sprintf(ocvform->l_str,"Attention: %d",elem4);
            putText(dst, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
        }
    }
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
    if ((!color_overlay_flow) && (event == EVENT_MOUSEMOVE) && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {
        // draw brush contour
        if (ocvform->drawbrushcontour)
        {
            dst0.release();
            dst0 = dst.clone();
            if (ocvform->circle_brush)
                circle(dst0, Point(x, y), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),0,LINE_AA);
            else
                rectangle(dst0, Rect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea), CV_RGB(255, 255, 255),0,LINE_AA);
            imshow("image", dst0);
        }
    }
    if ((!color_overlay_flow) && (event == EVENT_MOUSEMOVE) && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {
        ocvform->currmousepos.setX(x);
        ocvform->currmousepos.setY(y);        
    }
    if ((!color_overlay_flow) && (event == EVENT_MOUSEMOVE) && ((flags ==  EVENT_FLAG_LBUTTON) || (keepfiltering)) && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {      
        curr_iter++; // determines how often with mouse moves will be doing filtering
        if (curr_iter >= ocvform->currfilterrate)
        {
            Rect srcDstRect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea);
            dstt = dst(srcDstRect); // dst - full resulting overlay pic of main (src) and overlay (srccopy)

            applyfilt(ocvform->currfilttype,srcDstRect);           

            if (ocvform->currfilttype==5)
                addWeighted(dst(srcDstRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

            if (ocvform->circle_brush)
            {
                Mat mask_image(dstt.size(), CV_8U, Scalar(0)); // mask to have only circle of region
                circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);
                dstt.copyTo(dst(srcDstRect),mask_image);
            }
            else
                dstt.copyTo(dst(srcDstRect));

            imshow("image", dst);

            curr_iter=0;
        }
    }
    if ((!color_overlay_flow) && (event == EVENT_LBUTTONDOWN)  && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
    {
        Rect srcDstRect(x-ocvform->currfilterarea/2, y-ocvform->currfilterarea/2, ocvform->currfilterarea, ocvform->currfilterarea);
        dstt = dst(srcDstRect); // dst - full resulting overlay pic of main (src) and overlay (srccopy)

        if ((ocvform->currfilttype==5) && (ocvform->mixtype==3) && (ocvform->changerandpic_byclick))
            ocvform->changerandpic();

        applyfilt(ocvform->currfilttype,srcDstRect);

        prev_dst = dst.clone(); // for keeping state before last action (using in "cancel last")

        if (ocvform->currfilttype==5)
            addWeighted(dst(srcDstRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

        if (ocvform->circle_brush)
        {
            Mat mask_image(dstt.size(), CV_8U, Scalar(0)); // mask to have only circle of region
            circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);
            dstt.copyTo(dst(srcDstRect),mask_image);
        }
        else
            dstt.copyTo(dst(srcDstRect));

        imshow("image", dst);
    }
    // starting dreamflow drops by middle mouse down
    if ((!color_overlay_flow) && (event == EVENT_MBUTTONDOWN) && (!ocvform->dreamflow) && (!ocvform->drops_by_click_mode)
        && (y<dst.rows-ocvform->currfilterarea/2) && (x<dst.cols-ocvform->currfilterarea/2) && (y>ocvform->currfilterarea/2) && (x>ocvform->currfilterarea/2))
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
    if ((!color_overlay_flow) && (event == EVENT_MBUTTONUP) && (ocvform->drops_by_click_mode))
    {
       ocvform->start_stop_dreamflow(false);
       ocvform->drops_by_click_mode = false;
    }
    else
    if ((event == EVENT_MBUTTONDOWN) && (color_overlay_flow))
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
        if ((color_overlay_flow) && (!ocvform->hueonly))
        {
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
            imshow("image", dst);
        }
    } else
    if ((event == EVENT_RBUTTONDOWN) && (color_overlay_flow))
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
      //  imshow("image", dst);

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
{                                       // need to check if switch between flow direction invoked before any distances computed
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
    if (ocvform->showlabel)
        elfont.copyTo(dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)));
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
    opencvinterval = i;
    picfilt->setInterval(opencvinterval);
}

void MainWindow::cancellast()   // cancel last filtering action
{
    dst = prev_dst;
    imshow("image", dst);
}

void MainWindow::cancelall()    // cancel all filtering actions
{
    dst = clear_dst.clone();
    imshow("image", dst);
}

void MainWindow::dreamflow_Update() // timer for dreamflow mode, when new pic appears by random fragment over old
{
    int x,y,area;
    Rect srcDstRect; 
    if (ocvform->showlabel)
    {
        elfont.copyTo(dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)));
        elfont.copyTo(dream0(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)));
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
            area = (((ocvform->x_right-ocvform->x_left)/2-5)*elem4)/100+3;
        else
            area = qrand()%((ocvform->x_right-ocvform->x_left)/2-5)+3;
        x = ocvform->x_left + area/2 + qrand() % (ocvform->x_right - ocvform->x_left - area); // define rect based on area size (inside expanding window)
        y = ocvform->y_top + area/2 + qrand() % (ocvform->y_bottom - ocvform->y_top - area);
        srcDstRect = Rect(x-area/2, y-area/2, area, area);
    }

    if (ocvform->multi_img_dflow)
    {
        int t = qrand()%ocvform->multi_set_size;
        ocvform->randpic = curr_img_set[t];       
    }

    dstt = mixfilt(srcDstRect);
    Mat mask_image(dstt.size(), CV_8U, Scalar(0));
    if (!ocvform->polygonmask)
    {
        if (ocvform->circle_brush)
        {
            if (!ocvform->dropsmode)
                circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), ocvform->currfilterarea/2, CV_RGB(255, 255, 255),-1,LINE_AA);
            else
                circle(mask_image, Point(mask_image.rows / 2, mask_image.cols / 2), area/2, CV_RGB(255, 255, 255),-1,LINE_AA);
        }
    }
    else
        fillpolygon(mask_image);
    addWeighted(dst(srcDstRect), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);

    if ((ocvform->polygonmask) || (ocvform->circle_brush))
        dstt.copyTo(dst(srcDstRect),mask_image);
    else
        dstt.copyTo(dst(srcDstRect));

    if (ocvform->showlabel)
    {
        elfont = dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)).clone();
        sprintf(ocvform->l_str,"Attention: %d",elem4);
        putText(dst, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
        putText(dream0, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
    }

    imshow("image", dst);
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
    if ((color_overlay_flow) && (!ocvform->hueonly))
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
    if ((!color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->mixtype==3))
        ocvform->setcurrdream(currmainpic);
    if (plotw->start)
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
    if ((color_overlay_flow) && (!ocvform->hueonly))
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
    color_overlay_flow = false;
    if (mwconnected)
        TG_FreeConnection(connectionId);
    if (plotw->start)
        plotw->quitthreads();
    cv::destroyAllWindows();
    QApplication::quit();
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

void MainWindow::on_pushButton_3_clicked() // MindPlay window run
{
    plotw->show();
    plotw->doplot();
    plotw->appcn=connectWin;
    paintw->scene->init(plotw, this);
    connectWin->ps=paintw->scene;

    paintw->scene->init(plotw, this);
    paintw->move(QApplication::desktop()->screen()->rect().center() - paintw->rect().center()+QPoint(-10,-35));
    paintw->scene->clear();
    paintw->show();
    paintw->loadempty();
    plotw->paintfstart=true;
   // paintw->startpolyt();
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
    //plotw->hide();
    paintw->scene->init(plotw, this);  
    plotw->paintfstart=true;
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

    // opencvinterval = 25;// + (100 - elem4)/2;
    //picfilt->setInterval(opencvinterval);

    if (ocvform->puzzleflow_on)
    {
        if (ocvform->corrcells_by_att)
        {
            ocvform->corr_cell_part = (double)elem4/90;
            if (elem4<10)
                ocvform->corr_cell_part = 0.1;
            if (elem4>ocvform->changepuzzleborder)
                ocvform->corr_cell_part = 1;
            int m = (int)ocvform->cellnums*ocvform->corr_cell_part;
            for (size_t t = 0; t < m; t++)
                fillcell(ocvform->cells_indexes[t],ocvform->cols);
            ocvform->updateformvals();

            if (elem4>ocvform->changepuzzleborder)
            {
                imshow("image", dst);
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
            ocvform->puzzleflowrate = 50 + elem4*2.5;
            puzzleflow->setInterval(ocvform->puzzleflowrate);
            ocvform->updateformvals();
        }
        if (ocvform->cellsize_by_att)
        {
            puzzleflow->stop();
            ocvform->cell_size = 100 + elem4*2;
            ocvform->cols = ocvform->picwidth / ocvform->cell_size;
            ocvform->rows = ocvform->picheight / ocvform->cell_size;
            ocvform->cellnums = ocvform->cols * ocvform->rows;
            fillcells();
            puzzleflow->start();
            ocvform->updateformvals();
        }
    }

    if ((ocvform->attmodul_area) && (!color_overlay_flow))
    {
        // attention modulated filter area with minimum area value, only not activeflow mode
        if (elem4<20)
            elem4=20;
        ocvform->currfilterarea=elem4*5;
        ocvform->updateformvals(); // updating values on openCV filter control form
    }
    if ((!color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->mixtype==3) && (ocvform->changebyattention))
    {
        if ((canchangepic) && (elem4>elem6))
        {
            ocvform->changerandpic();        
            canchangepic = false;
        } else
        if ((!canchangepic) && (elem4<elem6))
            canchangepic = true;
    }
    if ((!color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->mixtype==3) && (ocvform->dreamflow))
    {
        if (ocvform->attent_modulated_dreams)
        {
            ocvform->dreamflowrate=105-elem4;
            dreamflow_timer->setInterval(ocvform->dreamflowrate);            
        }
        if (ocvform->drops_by_att)
        {
            ocvform->drops_interval=120-elem4; // 30+elem4/2;
            ocvform->dropsT->setInterval(ocvform->drops_interval);            
        }
        if (ocvform->poly_by_att)        
            ocvform->pointsinpoly = elem4 / 10 + 3;                    

        if (ocvform->directionswitch_by_att)
        {
            if (elem4>elem6)
                ocvform->flowdirection=1;
            else
                ocvform->flowdirection=-1;            
        }
        if (ocvform->multi_img_by_att)
            ocvform->multi_set_size = (100-elem4)/10 + 1;

        ocvform->updateformvals();
    }
    if ((!color_overlay_flow) && (ocvform->currfilttype==5) && (ocvform->transp_by_att))
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
        ocvform->randpicn = currmainpic;
        ocvform->randpic = src.clone();

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

        init_img_set();

        opencvstart=true;
        plotw->opencvstart=true;
        if (plotw->start)
            plotw->enablehue();
        picfilt->start();
    }
    else
        imshow("image", src);    
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
            curr_img_set[1] = srccopy.clone();
            addWeighted(src, alphaval, srccopy, 1 - alphaval, 0, dst);
          //  imshow("image", dst);
        }
        canchangepic=false;
    }
    if ((elem5<elem6) && (!canchangepic))
        canchangepic=true;
}

void MainWindow::makeHistFeatures()     // computing image histogram features
{
    QString picst;
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
    if (opencvstart)
        emit ocvform->flow_direction_available();
}

void MainWindow::addScaledImage(int num) // add rescaled image
{
    imgarray[num]=QPixmap::fromImage(imageScaling->resultAt(num));
}

void MainWindow::scalingFinished()  // when all rescaling through QtConcurrent is finished
{
   // qDebug() << time_take.elapsed();
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
        simulateEEG->stop();
        simeeg=false;
        plotw->srfr=512;     // sampling rate
        plotw->imlength=256; // length of single EEG interval
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
                        if (ocvform->attent_modul)
                            setattent(paintw->getestattval());
                           // setattent(mw_atten);
                        else
                            setattent(mw_medit);
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

void MainWindow::simulateEEGUpdate() // simulated EEG data (in development)
{
    const double mean = 0.0;
    const double stddev = 0.5;
    static std::mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<double> dist(mean, stddev);
    double noise = 20*dist(gen);
    deltaamp = zdeltaamp + dist(gen)*5;
    thetaamp = zthetaamp + dist(gen)*12;
    alphaamp = zalphaamp + dist(gen)*13;
    betaamp = zbetaamp + dist(gen)*12;
    gammaamp = zgammaamp + dist(gen)*10;
    hgammaamp = zhgammaamp + dist(gen)*3;
    if (currentel>100)
    {
        deltaphs = qrand() % 20;
        thetaphs = qrand() % 20;
        alphaphs = qrand() % 20;
        betaphs = qrand() % 20;
        gammaphs = qrand() % 20;
        currentel = 0;
    }
    currentel++;
    currentsimdata = deltaamp*sin(deltafr*2*M_PI/srfr*(currentel+deltaphs)) + thetaamp*sin(thetafr*2*M_PI/srfr*(currentel+thetaphs)) + alphaamp*sin(alphafr*2*M_PI/srfr*(currentel+alphaphs)) + betaamp*sin(betafr*2*M_PI/srfr*(currentel+betaphs)) + gammaamp*sin(gammafr*2*M_PI/srfr*(currentel+gammaphs)) + hgammaamp*sin(hgammafr*2*M_PI/srfr*(currentel+gammaphs)) + noise;
    //currentsimdata *= 2;
    rs->show();
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
    dstt = src(cellRect1).clone();    
 //   addWeighted(dst(cellRect2), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);
    if (ocvform->puzzle_edges)
        if (ocvform->white_edges)
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(255,255,255),2);
        else
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(qrand()%256,qrand()%256,qrand()%256),2);
    dstt.copyTo(dst(cellRect2));
        // dstt = curr_img_set[pic2n](cellRect2).clone(); // multi-pics puzzling
    dstt = src(cellRect2).clone();    
   // addWeighted(dst(cellRect1), (double)ocvform->transp / 100, dstt, 1 - (double)ocvform->transp / 100, 0, dstt);
    if (ocvform->puzzle_edges)
        if (ocvform->white_edges)
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(255,255,255),2);
        else
            rectangle(dstt, Point(0,0), Point(dstt.rows-2,dstt.rows-2),Scalar(qrand()%256,qrand()%256,qrand()%256),2);
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
    for (size_t t = 0; t < (int)(ocvform->cellnums*ocvform->corr_cell_part); t++)
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
    if (ocvform->free_cells.size()==1)
        fillcell(ocvform->free_cells[0],ocvform->cols);

    if (ocvform->showlabel)
    {
        elfont = dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)).clone();
        if (ocvform->attent_modul)
        {
            sprintf(ocvform->l_str,"Attention: %d",elem4);
            putText(dst, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
            putText(dream0, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,0,255,255), ocvform->lfont_size);
        }
        else
        {
            sprintf(ocvform->l_str,"Meditation: %d",mw_medit);
            putText(dst, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,255,0,255), ocvform->lfont_size);
            putText(dream0, ocvform->l_str, Point2f(ocvform->l_posx,ocvform->l_posy), FONT_HERSHEY_PLAIN, ocvform->lfont_scale, Scalar(0,255,0,255), ocvform->lfont_size);
        }
    }

    imshow("image", dst);
}

void MainWindow::keys_processing()      // processing keys pressing
{
    char key = cv::waitKey(5) % 256;
    if (key == 't') // test stuff button    
    {                
      //  neurostyle();        
    }
    if (key == 'p') // puzzle gathering mode
    {
        ocvform->puzzleflow_on = !ocvform->puzzleflow_on;
        ocvform->updateformvals();
        if (ocvform->puzzleflow_on)
        {
            fillcells();
            puzzleflow->start();
        }
        else
            puzzleflow->stop();
    }
    else if (key == 'w')      // swap main and overlay pics
        swap_main_overlay();
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
    else if ((key == ' ') && (!color_overlay_flow)) // turn on continuous filtering with mouse move mode
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
    {
        color_overlay_flow=!color_overlay_flow;
        if (color_overlay_flow)
        {
            ocvform->hide();
            ocvform->formshown=false;
            keepfiltering=false;
            if (ocvform->dreamflow)
                ocvform->start_stop_dreamflow(false);
            ocvform->setcameracheckbox(true);
        }
        else
        {
            clear_dst = dst.clone();
            ocvform->setcameracheckbox(false);
            ocvform->show();
            ocvform->formshown=true;
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
    else if (key == '-')                        // hide / show label of attention
    {
        elfont = dst(Rect(ocvform->l_posx-5,ocvform->l_posy-60,ocvform->lw,ocvform->lh)).clone();
        ocvform->showlabel = !ocvform->showlabel;
    }
    else if ((key == 'z') && (!color_overlay_flow))     // change filter on the left one
    {
        if (ocvform->currfilttype==1)
            ocvform->currfilttype=ocvform->totalfilts;
        else
            ocvform->currfilttype--;
        ocvform->updateformvals();
    }
    else if ((key == 'x') && (!color_overlay_flow))     // change filter on the right one
    {
        if (ocvform->currfilttype==ocvform->totalfilts)
            ocvform->currfilttype=1;
        else
            ocvform->currfilttype++;
        ocvform->updateformvals();
    }
    else if ((key == 'r') && (color_overlay_flow))      // start receiving camera input for overlay flow
    {
        if (!ocvform->camerainp)
        {
            cam.open(0);           
            ocvform->camerainp = true;
            ocvform->updateformvals();
        }
    }
    else if ((key == 's') && (color_overlay_flow))        // stop receiving camera input for overlay flow
    {
        cam.release();
        ocvform->camerainp = false;
        ocvform->updateformvals();
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
    else if ((key == 'm') && (!color_overlay_flow)) // change brush type: circle / box
    {
        ocvform->circle_brush = !ocvform->circle_brush;
        ocvform->updateformvals();
    }
    else if (key == 'u')
    {
        ocvform->hueonly=!ocvform->hueonly;
        ocvform->updateformvals();
    }
    else if ((key == 'f') && (!color_overlay_flow)) // start / stop timer for pics change
    {
        ocvform->changepic_bytime=!ocvform->changepic_bytime;
        if (ocvform->changepic_bytime)
            ocvform->pichngT->start();
        else
            ocvform->pichngT->stop();
        ocvform->updateformvals();
    }
    else if (key == 'g')                   // switch flow direction by attention > border
        ocvform->directionswitch_by_att=!ocvform->directionswitch_by_att;
    else if (key == 'h')                    // story mode: main pic is fixed, overlay pic change by attention > border
        storymode=!storymode;                  
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

    if (color_overlay_flow)
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
        imgarray.clear();

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
