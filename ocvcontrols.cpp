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
   // setAttribute(Qt::WA_TranslucentBackground,true);
   // setWindowOpacity(0.95);
   // ui->tabWidget->setAutoFillBackground(true);
   // ui->tabWidget->setStyleSheet("background-color:lightgray;");
}

void ocvcontrols::updateformvals()
{
    ui->spinBox->setValue(currfilterarea);
    ui->spinBox_2->setValue(currfilterrate);
    ui->spinBox_3->setValue(kernel_s);
    ui->spinBox_4->setValue(sigma_color);
    ui->spinBox_5->setValue(sigma_space);
    ui->spinBox_6->setValue(wave_freqs);
    ui->spinBox_7->setValue(wave_amp);
    ui->spinBox_8->setValue(dilation_size);
    ui->spinBox_9->setValue(nfeatures);
    ui->spinBox_10->setValue(nlevels);
    ui->spinBox_11->setValue(edgetreshold);
    ui->doubleSpinBox->setValue(scalef);

    if (dilation_elem==1)
        ui->radioButton->setChecked(true);
    else if (dilation_elem==2)
        ui->radioButton_2->setChecked(true);
    else if (dilation_elem==3)
        ui->radioButton_3->setChecked(true);

    if (mixtype==1)
        ui->radioButton_4->setChecked(true);
    else if (mixtype==2)
        ui->radioButton_5->setChecked(true);

    ui->spinBox_12->setValue(transp);
    ui->tabWidget->setCurrentIndex(currfilttype-1);
}

ocvcontrols::~ocvcontrols()
{
    delete ui;
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
}

void ocvcontrols::on_radioButton_5_clicked()
{
    mixtype = 2;
}

void ocvcontrols::on_radioButton_6_clicked()
{
    mixtype = 3;
    changerandpic();  
}

void ocvcontrols::changerandpic()
{
    randpicn = mww->geticonnum(qrand() % (leftpan->imgnumber),true);    
    QString rpic = mww->getimagepath(randpicn);
    randpic.release();
    randpic = imread(rpic.toStdString());
    // cv::resize(randpic, randpic, cv::Size(src.cols,src.rows), 0, 0, cv::INTER_LINEAR);
}
void ocvcontrols::on_pushButton_3_clicked()
{
    changerandpic();
}

void ocvcontrols::on_checkBox_2_clicked()
{
    changerandpic_byclick = !changerandpic_byclick;  
}
