/* source file for ocvcontrols class -
   resposible for MindOCV parameters and actions */

#include "ocvcontrols.h"
#include "ui_ocvcontrols.h"

ocvcontrols::ocvcontrols(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ocvcontrols)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);    

    pichngT = new QTimer(this);     // timer for auto dreamflow mode, when new picture appears by fragments
    pichngT->connect(pichngT,SIGNAL(timeout()), this, SLOT(randpicchange_Update()));
    pichngT->setInterval(changepic_interval*1000);

    dropsT = new QTimer(this);     // timer for expanding window in dreamflow mode
    dropsT->connect(dropsT,SIGNAL(timeout()), this, SLOT(windowsize_Update()));
    dropsT->setInterval(drops_interval);

    seed.setX(1000);
    seed.setY(562);
    x_left = seed.x()-firstdrop_size/2;
    x_right = seed.x()+firstdrop_size/2;
    y_top = seed.y()-firstdrop_size/2;
    y_bottom = seed.y()+firstdrop_size/2;

    wcolor = Scalar(255,255,255);

    ui->spinBox_16->setStyleSheet("QSpinBox { background-color: yellow; }");      

    ui->comboBox->setEnabled(histFeaturesReady);
    connect(this,&ocvcontrols::flow_direction_available,this,&ocvcontrols::set_flow_direction_available);

    updateformvals();

   // setAttribute(Qt::WA_TranslucentBackground,true);
   // setWindowOpacity(0.95);
   // ui->tabWidget->setAutoFillBackground(true);
   // ui->tabWidget->setStyleSheet("background-color:lightgray;");    
}

void ocvcontrols::updateformvals()
{
    ui->spinBox->setValue(currfilterarea);
    ui->spinBox_2->setValue(currfilterrate);  
    ui->spinBox_12->setValue(transp);
    ui->tabWidget->setCurrentIndex(currfilttype-1);
    if (mixtype==1)
        ui->radioButton_4->setChecked(true);
    else if (mixtype==2)
        ui->radioButton_5->setChecked(true);
    else
        ui->radioButton_6->setChecked(true);    
    ui->checkBox_7->setChecked(hueonly);
    ui->checkBox_8->setChecked(polygonmask);
    ui->spinBox_13->setValue(dreamflowrate);
    ui->spinBox_15->setValue(drops_interval);
    ui->spinBox_16->setValue(pointsinpoly);
    ui->checkBox_14->setChecked(plotdroprect);
    ui->checkBox_15->setChecked(drops_from_mousepos);
    ui->checkBox_16->setChecked(drawbrushcontour);
    ui->checkBox_17->setChecked(camerainp);
    ui->checkBox_9->setChecked(changepic_bytime);
    ui->checkBox_10->setChecked(dropsmode);
    if (flowdirection==1)
        ui->comboBox->setCurrentIndex(1);
    else if (flowdirection==-1)
        ui->comboBox->setCurrentIndex(2);
    else
        ui->comboBox->setCurrentIndex(0);
}

ocvcontrols::~ocvcontrols()
{
    delete ui;
}

void ocvcontrols::randpicchange_Update()
{
    changerandpic();
}

void ocvcontrols::drop_center_from_mousepos()
{
    if ((currmousepos.x()>dropsgrow_step*3+firstdrop_size) && (currmousepos.x()<picwidth-dropsgrow_step*3+firstdrop_size) && (currmousepos.y()>dropsgrow_step*3+firstdrop_size) && (currmousepos.y()<picheight-dropsgrow_step*3-firstdrop_size*3))
    {
        seed.setX(currmousepos.x());
        seed.setY(currmousepos.y());
    } else
    {
        seed.setX(dropsgrow_step*3+firstdrop_size+qrand()%(picwidth-dropsgrow_step*3-firstdrop_size*3));
        seed.setY(dropsgrow_step*3+firstdrop_size+qrand()%(picheight-dropsgrow_step*3-firstdrop_size*3));
    }
    x_left = seed.x()-firstdrop_size/2;
    x_right = seed.x()+firstdrop_size/2;
    y_top = seed.y()-firstdrop_size/2;
    y_bottom = seed.y()+firstdrop_size/2;
}

void ocvcontrols::windowsize_Update()
{
    if (x_left>=dropsgrow_step)
        x_left-=dropsgrow_step;
    if (x_right<=picwidth-dropsgrow_step)
        x_right+=dropsgrow_step;
    if (y_top>=dropsgrow_step)
        y_top-=dropsgrow_step;
    if (y_bottom<=picheight-dropsgrow_step)
        y_bottom+=dropsgrow_step;
    if ((x_left<dropsgrow_step) && (x_right>picwidth-dropsgrow_step) && (y_top<dropsgrow_step) && (y_bottom>picheight-dropsgrow_step))
    {
        if (!drops_from_mousepos)
        {
            seed.setX(dropsgrow_step*3+firstdrop_size+qrand()%(picwidth-dropsgrow_step*3-firstdrop_size*3));
            seed.setY(dropsgrow_step*3+firstdrop_size+qrand()%(picheight-dropsgrow_step*3-firstdrop_size*3));
            x_left = seed.x()-firstdrop_size/2;
            x_right = seed.x()+firstdrop_size/2;
            y_top = seed.y()-firstdrop_size/2;
            y_bottom = seed.y()+firstdrop_size/2;
        }
        else
            drop_center_from_mousepos();

        changerandpic();
        wcolor = Scalar(qrand()%256,qrand()%256,qrand()%256);
    }
    if (plotdroprect)
        mww->drawwindow(x_left,y_top,x_right-x_left,y_bottom-y_top);
    mww->setprevdfrect(x_left+3,y_top+3,x_right-x_left-3,y_bottom-y_top-3);

}

void ocvcontrols::on_spinBox_valueChanged(int arg1)
{
    currfilterarea=arg1;
}

void ocvcontrols::on_spinBox_2_valueChanged(int arg1)
{
    currfilterrate=arg1;
}

void ocvcontrols::on_tabWidget_tabBarClicked(int index)
{
    if (index!=-1)    
        currfilttype=index+1;
}

void ocvcontrols::on_spinBox_3_valueChanged(int arg1)
{
    kernel_s=arg1;
}

void ocvcontrols::on_spinBox_4_valueChanged(int arg1)
{
    sigma_color=arg1;
}

void ocvcontrols::on_spinBox_5_valueChanged(int arg1)
{
    sigma_space=arg1;
}

void ocvcontrols::on_spinBox_6_valueChanged(int arg1)
{
    wave_freqs=arg1;
}

void ocvcontrols::on_spinBox_7_valueChanged(int arg1)
{
    wave_amp=arg1;
}

void ocvcontrols::on_spinBox_8_valueChanged(int arg1)
{
    dilation_size=arg1;
}

void ocvcontrols::on_radioButton_clicked()
{
    dilation_elem=1;
}

void ocvcontrols::on_radioButton_2_clicked()
{
    dilation_elem=2;
}

void ocvcontrols::on_radioButton_3_clicked()
{
    dilation_elem=3;
}

void ocvcontrols::on_checkBox_clicked()
{
    attmodul_area=!attmodul_area;
    if (attmodul_area)
        ui->spinBox->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox->setStyleSheet("QSpinBox { background-color: white; }");
}

void ocvcontrols::on_pushButton_clicked()
{
    mww->cancellast();
}

void ocvcontrols::on_pushButton_2_clicked()
{
    mww->cancelall();
}

void ocvcontrols::on_spinBox_9_valueChanged(int arg1)
{
    nfeatures=arg1;
}

void ocvcontrols::on_spinBox_10_valueChanged(int arg1)
{
    nlevels=arg1;
}

void ocvcontrols::on_spinBox_11_valueChanged(int arg1)
{
    edgetreshold=arg1; 
}

void ocvcontrols::on_doubleSpinBox_valueChanged(double arg1)
{
    scalef=arg1;
}

void ocvcontrols::on_spinBox_12_valueChanged(int arg1)
{
    transp=arg1;
}

void ocvcontrols::on_radioButton_4_clicked()
{
    mixtype = 1;
    ui->checkBox_2->setEnabled(false);
    ui->checkBox_4->setEnabled(false);
    ui->checkBox_5->setEnabled(false);
    ui->checkBox_8->setEnabled(false);
    ui->checkBox_9->setEnabled(false);
    ui->spinBox_14->setEnabled(false);
}

void ocvcontrols::on_radioButton_5_clicked()
{
    mixtype = 2;
    ui->checkBox_2->setEnabled(false);
    ui->checkBox_4->setEnabled(false);    
    ui->checkBox_5->setEnabled(false);
    ui->checkBox_8->setEnabled(false);
    ui->checkBox_9->setEnabled(false);
    ui->spinBox_14->setEnabled(false);
}

void ocvcontrols::randmixer_mode_on()
{
    mixtype = 3;
    if (!drops_by_click_mode)
        changerandpic();
    ui->checkBox_2->setEnabled(true);
    ui->checkBox_4->setEnabled(true);
    ui->checkBox_5->setEnabled(true);
    ui->checkBox_9->setEnabled(true);
    ui->spinBox_14->setEnabled(true);
}

void ocvcontrols::on_radioButton_6_clicked()
{
    randmixer_mode_on();
}

void ocvcontrols::changerandpic()
{    
    if (flowdirection==0)
        randpicn = mww->geticonnum(qrand() % (leftpan->imgnumber),true);
    else
    {
        if (flowdirection==1)
        {
            mww->getchi2dists(randpicn);
            randpicn = mww->nearest_pics[ngbarea/5+qrand()%ngbarea];
        }
        else
        {
            mww->getchi2dists(randpicn);
            randpicn = mww->farest_pics[ngbarea/5+qrand()%ngbarea];
        }
        mww->getchi2dists(randpicn);
        mww->fillmaininpuzzle(randpicn);
        mww->fillpuzzle_withneighbours();
    }    
    QString rpic = mww->getimagepath(randpicn);
    randpic.release();
    randpic = imread(rpic.toStdString());
    cv::resize(randpic, randpic, cv::Size(picwidth,picheight), 0, 0, cv::INTER_LINEAR);    
    mww->just_update_mainpic(randpicn);
}

void ocvcontrols::setcurrdream(int t)
{
    randpicn = t;
    QString rpic = mww->getimagepath(randpicn);
    randpic.release();
    randpic = imread(rpic.toStdString());
    cv::resize(randpic, randpic, cv::Size(picwidth,picheight), 0, 0, cv::INTER_LINEAR);
}

void ocvcontrols::start_stop_dreamflow(bool fl)
{   
    on_checkBox_5_clicked();
    ui->checkBox_5->setChecked(fl);
}

void ocvcontrols::set_flow_direction_available()
{
    ui->comboBox->setEnabled(histFeaturesReady);
}

void ocvcontrols::setcameracheckbox(bool fl)
{
    ui->checkBox_17->setEnabled(fl);
}

void ocvcontrols::on_pushButton_3_clicked()
{
    changerandpic();
}

void ocvcontrols::on_checkBox_2_clicked()
{
    changerandpic_byclick = !changerandpic_byclick;  
}

void ocvcontrols::on_pushButton_4_clicked()
{
    fcolor = QColorDialog::getColor(Qt::white, this, "Select Color", QColorDialog::DontUseNativeDialog);
}

void ocvcontrols::on_checkBox_3_clicked()
{
    randfcolor=!randfcolor;
}

void ocvcontrols::on_checkBox_4_clicked()
{
    changebyattention=!changebyattention;
}

void ocvcontrols::on_checkBox_5_clicked()
{
    dreamflow=!dreamflow;
    if (dreamflow)
    {
        mww->setdream0();        
        mww->dreamflow_timer->start();
        if (dropsmode)
        {            
            mww->setprevdfrect(x_left+3,y_top+3,x_right-x_left-3,y_bottom-y_top-3);
            dropsT->start();            
            ui->checkBox_11->setEnabled(true);
            ui->checkBox_14->setEnabled(true);
            ui->checkBox_15->setEnabled(true);
        }
        ui->checkBox_6->setEnabled(true);
        ui->checkBox_8->setEnabled(true);
        ui->checkBox_10->setEnabled(true);
        ui->checkBox_12->setEnabled(true);
    }
    else
    {
        mww->dreamflow_timer->stop();
        dropsT->stop();
        if (!drops_by_click_mode)
        {
            ui->checkBox_6->setEnabled(false);
            ui->checkBox_8->setEnabled(false);
            ui->checkBox_10->setEnabled(false);
            ui->checkBox_11->setEnabled(false);
            ui->checkBox_12->setEnabled(false);
            ui->checkBox_14->setEnabled(false);
            ui->checkBox_15->setEnabled(false);
        }
    }       
}

void ocvcontrols::on_spinBox_13_valueChanged(int arg1)
{
    dreamflowrate=arg1;
}

void ocvcontrols::on_checkBox_6_clicked()
{
    attent_modulated_dreams=!attent_modulated_dreams;
    if (attent_modulated_dreams)
        ui->spinBox_13->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox_13->setStyleSheet("QSpinBox { background-color: white; }");
}

void ocvcontrols::on_checkBox_7_clicked()
{
    hueonly=!hueonly;
}

void ocvcontrols::on_checkBox_8_clicked()
{
    polygonmask=!polygonmask;
    ui->spinBox_16->setEnabled(polygonmask);
    ui->checkBox_12->setEnabled(polygonmask);
}

void ocvcontrols::on_checkBox_9_clicked()
{
    changepic_bytime=!changepic_bytime;
    if (changepic_bytime)
        pichngT->start();
    else
        pichngT->stop();
}

void ocvcontrols::on_spinBox_14_valueChanged(int arg1)
{
    changepic_interval=arg1*1000;
}

void ocvcontrols::on_checkBox_10_clicked()
{
    dropsmode=!dropsmode;
    if (dropsmode)
    {
        mww->setdream0();
        mww->setprevdfrect(x_left+3,y_top+3,x_right-x_left-3,y_bottom-y_top-3);
        dropsT->start();
    }
    else
        dropsT->stop();
    ui->spinBox_15->setEnabled(dropsmode);
    ui->checkBox_11->setEnabled(dropsmode);
    ui->checkBox_14->setEnabled(dropsmode);
    ui->checkBox_15->setEnabled(dropsmode);
}

void ocvcontrols::on_checkBox_12_clicked()
{
    poly_by_att=!poly_by_att;
    if (poly_by_att)
        ui->spinBox_16->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox_16->setStyleSheet("QSpinBox { background-color: white; }");
}

void ocvcontrols::on_spinBox_16_valueChanged(int arg1)
{
    pointsinpoly=arg1;
}

void ocvcontrols::on_checkBox_11_clicked()
{
    drops_by_att=!drops_by_att;
    if (drops_by_att)
        ui->spinBox_15->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox_15->setStyleSheet("QSpinBox { background-color: white; }");
}

void ocvcontrols::on_spinBox_15_valueChanged(int arg1)
{
    drops_interval=arg1;
    dropsT->setInterval(drops_interval);
}

void ocvcontrols::on_checkBox_15_clicked()
{
    drops_from_mousepos=!drops_from_mousepos;
}

void ocvcontrols::on_checkBox_14_clicked()
{
    plotdroprect=!plotdroprect;
}

void ocvcontrols::on_checkBox_16_clicked()
{
    drawbrushcontour=!drawbrushcontour;
}

void ocvcontrols::on_checkBox_13_clicked()
{
    transp_by_att=!transp_by_att;
    if (transp_by_att)
        ui->spinBox_12->setStyleSheet("QSpinBox { background-color: yellow; }");
    else
        ui->spinBox_12->setStyleSheet("QSpinBox { background-color: white; }");
}

void ocvcontrols::on_comboBox_currentIndexChanged(int index)
{
    if (index==2)
    {
        flowdirection=-1;
        ui->comboBox->setStyleSheet("QComboBox { color: darkRed; }");
    }
    else if (index==1)
    {
        flowdirection=1;
        ui->comboBox->setStyleSheet("QComboBox { color: darkGreen; }");
    } else
    {
        flowdirection=0;
        ui->comboBox->setStyleSheet("QComboBox { color: black; }");
    }
    if (mww->getchi2distsize()>0)
        mww->fillpuzzle_withneighbours();
}

void ocvcontrols::on_checkBox_17_clicked()
{
    camerainp=!camerainp;
    if (camerainp)
        mww->usingcam(true);
    else
        mww->usingcam(false);
}
