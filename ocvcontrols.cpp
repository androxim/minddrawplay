#include "ocvcontrols.h"
#include "ui_ocvcontrols.h"

ocvcontrols::ocvcontrols(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ocvcontrols)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    attmodulation=false;
 //   setAttribute(Qt::WA_TranslucentBackground,true);
 //   setWindowOpacity(0.75);
//    ui->tabWidget->setAutoFillBackground(true);
//    ui->tabWidget->setStyleSheet("background-color:lightgray;");
}

ocvcontrols::updatevals()
{
    ui->spinBox->setValue(filtarea);
    ui->spinBox_2->setValue(filtrate);
    ui->spinBox_3->setValue(kernelsize);
    ui->spinBox_4->setValue(s_color);
    ui->spinBox_5->setValue(s_space);
    ui->spinBox_6->setValue(wave_freq);
    ui->spinBox_7->setValue(wave_amp);
    ui->spinBox_8->setValue(dilate_size);
    if (dilation_el==1)
        ui->radioButton->setChecked(true);
    else if (dilation_el==2)
        ui->radioButton_2->setChecked(true);
    else if (dilation_el==3)
        ui->radioButton_3->setChecked(true);
    ui->tabWidget->setCurrentIndex(filttype);
}

ocvcontrols::~ocvcontrols()
{
    delete ui;
}

void ocvcontrols::on_spinBox_valueChanged(int arg1)
{
    filtarea=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_spinBox_2_valueChanged(int arg1)
{
    filtrate=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_tabWidget_tabBarClicked(int index)
{
    if (index!=-1)
    {
        filttype=index;
        mww->updateocvparams();
    }
}

void ocvcontrols::on_spinBox_3_valueChanged(int arg1)
{
    kernelsize=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_spinBox_4_valueChanged(int arg1)
{
    s_color=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_spinBox_5_valueChanged(int arg1)
{
    s_space=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_spinBox_6_valueChanged(int arg1)
{
    wave_freq=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_spinBox_7_valueChanged(int arg1)
{
    wave_amp=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_spinBox_8_valueChanged(int arg1)
{
    dilate_size=arg1;
    mww->updateocvparams();
}

void ocvcontrols::on_radioButton_clicked()
{
    dilation_el=1;
    mww->updateocvparams();
}

void ocvcontrols::on_radioButton_2_clicked()
{
    dilation_el=2;
    mww->updateocvparams();
}

void ocvcontrols::on_radioButton_3_clicked()
{
    dilation_el=3;
    mww->updateocvparams();
}

void ocvcontrols::on_checkBox_clicked()
{
    attmodulation=!attmodulation;
    mww->updateocvparams();
}

void ocvcontrols::on_pushButton_clicked()
{
    mww->cancellast();
}

void ocvcontrols::on_pushButton_2_clicked()
{
    mww->cancelall();
}
