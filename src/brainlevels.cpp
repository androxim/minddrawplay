#include "brainlevels.h"
#include "ui_brainlevels.h"
#include <QCloseEvent>

brainlevels::brainlevels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::brainlevels)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
    //setAttribute(Qt::WA_TranslucentBackground,true);
    setWindowOpacity(0.7);
    attention_I = true;
    attention = 0;
    meditation = 0;
    ui->horizontalSlider->setVisible(false);
    ui->horizontalSlider_2->setVisible(false);
    ui->horizontalSlider_3->setVisible(false);
    ui->label_2->setStyleSheet("QLabel {color : green;}");
    ui->comboBox->setStyleSheet("QComboBox {color : red;}");
}

void brainlevels::updatelevels(int att, int medit)
{
    attention = att;
    meditation = medit;
    ui->progressBar->setValue(attention);
    ui->progressBar_2->setValue(meditation);
}

void brainlevels::closeEvent(QCloseEvent *event)
{
    mww->set_showlevelscheckbox(false);
    event->accept();
}

void brainlevels::show_attentionborder()
{
    ui->horizontalSlider->setVisible(true);
}

void brainlevels::settonesbordervisible(bool fl)
{
    ui->horizontalSlider_2->setVisible(fl);
    ui->horizontalSlider_3->setVisible(fl);
}

brainlevels::~brainlevels()
{
    delete ui;
}

void brainlevels::on_horizontalSlider_valueChanged(int value)
{
    attention_bord = value;
    plw->set_nemehanika_bord(value);
}

void brainlevels::on_comboBox_currentIndexChanged(int index)
{
    if (index == 0)
        attention_I = true;
    else
        attention_I = false;
}

void brainlevels::on_horizontalSlider_2_valueChanged(int value)
{
    plw->tonesets_border1 = value;
}

void brainlevels::on_horizontalSlider_3_valueChanged(int value)
{
    plw->tonesets_border2 = value;
}
