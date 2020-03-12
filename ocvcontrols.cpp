#include "ocvcontrols.h"
#include "ui_ocvcontrols.h"

ocvcontrols::ocvcontrols(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ocvcontrols)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    attmodul_area=false;
    updateformvals();
    pichngT = new QTimer(this);     // timer for auto dreamflow mode, when new picture appears by fragments
    pichngT->connect(pichngT,SIGNAL(timeout()), this, SLOT(randpicchange_Update()));
    pichngT->setInterval(changepic_interval*1000);

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
    ui->spinBox_13->setValue(dreamflowrate);
    ui->checkBox_7->setChecked(hueonly);
    ui->checkBox_8->setChecked(polygonmask);
}

ocvcontrols::~ocvcontrols()
{
    delete ui;
}

void ocvcontrols::randpicchange_Update()
{
    changerandpic();
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
    ui->checkBox_8->setEnabled(false);
    ui->checkBox_9->setEnabled(false);
    ui->spinBox_14->setEnabled(false);
}

void ocvcontrols::on_radioButton_5_clicked()
{
    mixtype = 2;
    ui->checkBox_2->setEnabled(false);
    ui->checkBox_4->setEnabled(false);    
    ui->checkBox_8->setEnabled(false);
    ui->checkBox_9->setEnabled(false);
    ui->spinBox_14->setEnabled(false);
}

void ocvcontrols::on_radioButton_6_clicked()
{
    mixtype = 3;
    changerandpic();  
    ui->checkBox_2->setEnabled(true);
    ui->checkBox_4->setEnabled(true);
    ui->checkBox_8->setEnabled(true);
    ui->checkBox_9->setEnabled(true);
    ui->spinBox_14->setEnabled(true);
}

void ocvcontrols::changerandpic()
{
    randpicn = mww->geticonnum(qrand() % (leftpan->imgnumber),true);    
    QString rpic = mww->getimagepath(randpicn);
    randpic.release();
    randpic = imread(rpic.toStdString());
    cv::resize(randpic, randpic, cv::Size(picwidth,picheight), 0, 0, cv::INTER_LINEAR);
}

void ocvcontrols::stopdreamflow()
{
    if (attent_modulated_dreams)
    {
        on_checkBox_6_clicked();
        ui->checkBox_6->setChecked(false);
    }
    on_checkBox_5_clicked();
    ui->checkBox_5->setChecked(false);
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
    dreamflowmode=!dreamflowmode;
    if (!dreamflowmode)
    {
        autodreamflow = false;
        ui->checkBox_5->setChecked(false);
        mww->dreamflow_timer->stop();
        ui->checkBox_6->setEnabled(false);        
    }
    ui->checkBox_5->setEnabled(dreamflowmode);
    ui->checkBox_8->setEnabled(dreamflowmode);
}

void ocvcontrols::on_checkBox_5_clicked()
{
    autodreamflow=!autodreamflow;
    if (autodreamflow)
    {
        mww->dreamflow_timer->start();
        ui->checkBox_6->setEnabled(true);
    }
    else
    {
        mww->dreamflow_timer->stop();
        ui->checkBox_6->setEnabled(false);
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
