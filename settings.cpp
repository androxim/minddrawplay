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
    pwd->zerobutter=ui->radioButton_8->isChecked();
    pwd->hnt->osfr=esavg;
    pwd->hnt->imlength=eslength;
    pwd->hnt->imstprop=esprop;
    pwd->hnt->stlength=pwd->hnt->imlength*pwd->hnt->imstprop;
    if (ui->radioButton->isChecked())
        pwd->micrecord=false;
    else
        pwd->micrecord=true;
    pwd->loopnum=ui->spinBox_17->value();
    pwd->hnt->srfr=ui->spinBox_20->value();
    pwd->zerophaseinit(pwd->lcutoff,pwd->hcutoff,pwd->butterord,pwd->hnt->srfr);
    pwd->refresh();
    pwd->setpicfolder(ui->lineEdit->text());
    pwd->attentionvolume=ui->checkBox_2->isChecked();
    pwd->adaptivenumparts=ui->checkBox->isChecked();
    pwd->enablenumparts(!ui->checkBox->isChecked());
    close();
}

void Settings::on_pushButton_2_clicked()
{
    close();
}

void Settings::init()
{
    esavg=pwd->hnt->osfr;
    eslength=pwd->hnt->imlength;
    esprop=pwd->hnt->imstprop;
    ui->frame_11->setVisible(false);
    ui->frame_2->setGeometry(20,290,391,75);
    ui->radioButton_8->setChecked(pwd->zerobutter);
    ui->spinBox_14->setValue(pwd->butterord);
    ui->spinBox_15->setValue(pwd->lcutoff);
    ui->spinBox_16->setValue(pwd->hcutoff);
    ui->spinBox_17->setValue(pwd->loopnum);
    ui->spinBox_20->setValue(pwd->hnt->srfr);
    ui->checkBox->setChecked(pwd->adaptivenumparts);
    ui->checkBox_2->setChecked(pwd->attentionvolume);
    ui->lineEdit->setText(pwd->folderpath);
    if (!pwd->micrecord)
        ui->radioButton->setChecked(true);
    else
        ui->radioButton_2->setChecked(true);
}

void Settings::on_spinBox_20_valueChanged(int arg1)
{
    pwd->hnt->srfr=arg1;
}

void Settings::on_pushButton_clicked()
{
    QString filePath=QFileDialog::getExistingDirectory(this, "Get Any Folder", "D://");
    if (filePath!="")
        ui->lineEdit->setText(filePath);
}
