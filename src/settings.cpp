/* source file for MindDraw->Settings window class -
   resposible for some MindDraw parameters */

#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);   
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_pushButton_3_clicked() // "Apply" button
{
    pwd->butterord=ui->spinBox_14->value();
    pwd->lcutoff=ui->spinBox_15->value();
    pwd->hcutoff=ui->spinBox_16->value();    
    pwd->imlength=eslength;
    pwd->stlength=pwd->imlength;       
    pwd->srfr=ui->spinBox_20->value();
    pwd->filtercl->zerophaseinit(pwd->lcutoff,pwd->hcutoff,pwd->butterord,pwd->srfr);
    pwd->update_intervals_spinboxes();
    pwd->setpicfolder(ui->lineEdit->text());
    pwd->attention_volume=ui->checkBox_2->isChecked();
    pwd->adaptivenumparts=ui->checkBox->isChecked();
    pwd->enable_num_intervals(!ui->checkBox->isChecked());    
    close();
}

void Settings::on_pushButton_2_clicked()
{
    close();
}

void Settings::init()
{ 
    eslength=pwd->imlength;    
    ui->checkBox_2->setVisible(false);
    ui->spinBox_14->setValue(pwd->butterord);
    ui->spinBox_15->setValue(pwd->lcutoff);
    ui->spinBox_16->setValue(pwd->hcutoff);    
    ui->spinBox_20->setValue(pwd->srfr);
    ui->checkBox->setChecked(pwd->adaptivenumparts);
    ui->checkBox_2->setChecked(pwd->attention_volume);
    ui->lineEdit->setText(pwd->folderpath); 
}

void Settings::on_spinBox_20_valueChanged(int arg1)
{
    pwd->srfr=arg1;
}

void Settings::on_pushButton_clicked()
{
    QString filePath=QFileDialog::getExistingDirectory(this, "Get Any Folder", "D://");
    if (filePath!="")
        ui->lineEdit->setText(filePath);
}
