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
    setWindowOpacity(0.5);
    attention = 0;
    meditation = 0;
    ui->horizontalSlider->setVisible(false);
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

brainlevels::~brainlevels()
{
    delete ui;
}

void brainlevels::on_horizontalSlider_valueChanged(int value)
{
    attention_bord = value;
    plw->set_nemehanika_bord(value);
}
