#include "leftpanel.h"
#include "ui_leftpanel.h"

leftpanel::leftpanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::leftpanel)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(Qt::FramelessWindowHint);

    rationmode = Qt::IgnoreAspectRatio;

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setAlignment(Qt::AlignCenter);
    ui->graphicsView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_2->setAlignment(Qt::AlignCenter);
    ui->graphicsView_3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_3->setAlignment(Qt::AlignCenter);
    ui->graphicsView_4->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_4->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_4->setAlignment(Qt::AlignCenter);
    ui->graphicsView_5->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_5->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_5->setAlignment(Qt::AlignCenter);
    ui->graphicsView_6->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_6->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_6->setAlignment(Qt::AlignCenter);
    ui->graphicsView_7->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_7->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_7->setAlignment(Qt::AlignCenter);

    ui->graphicsView->setFrameStyle(0);
    ui->graphicsView_2->setFrameStyle(0);
    ui->graphicsView_3->setFrameStyle(0);
    ui->graphicsView_4->setFrameStyle(0);
    ui->graphicsView_5->setFrameStyle(0);
    ui->graphicsView_6->setFrameStyle(0);
    ui->graphicsView_7->setFrameStyle(3);

    ui->graphicsView_7->setFrameShape(QFrame::Box);
    ui->graphicsView_7->setFrameShadow(QFrame::Raised);
    ui->graphicsView_7->setLineWidth(4);

    QGraphicsScene* scene = new QGraphicsScene();
    scene->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView->setScene(scene);
    QGraphicsScene* scene2 = new QGraphicsScene();
    scene2->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView_2->setScene(scene2);
    QGraphicsScene* scene3 = new QGraphicsScene();
    scene3->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView_3->setScene(scene3);
    QGraphicsScene* scene4 = new QGraphicsScene();
    scene4->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView_4->setScene(scene4);
    QGraphicsScene* scene5 = new QGraphicsScene();
    scene5->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView_5->setScene(scene5);
    QGraphicsScene* scene6 = new QGraphicsScene();
    scene6->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView_6->setScene(scene6);
    QGraphicsScene* scene7 = new QGraphicsScene();
    scene7->setSceneRect(QRectF(0,0, imgsize, imgsize));
    ui->graphicsView_7->setScene(scene7);    

    ui->graphicsView->setGeometry(5,25,138,138);
    ui->graphicsView_2->setGeometry(5,165,138,138);
    ui->graphicsView_3->setGeometry(5,305,138,138);
    ui->graphicsView_4->setGeometry(5,445,138,138);
    ui->graphicsView_5->setGeometry(5,585,138,138);
    ui->graphicsView_6->setGeometry(5,725,138,138);
    ui->graphicsView_7->setGeometry(5,865,138,138);

    ui->graphicsView->installEventFilter(this);
    ui->graphicsView_2->installEventFilter(this);
    ui->graphicsView_3->installEventFilter(this);
    ui->graphicsView_4->installEventFilter(this);
    ui->graphicsView_5->installEventFilter(this);
    ui->graphicsView_6->installEventFilter(this);

    ui->pushButton->setGeometry(5,1,138,23);
}

void leftpanel::fillpics()
{   
    ui->graphicsView->scene()->addPixmap(mww->imgarray[mww->geticon(0)]);
    ui->graphicsView_2->scene()->addPixmap(mww->imgarray[mww->geticon(1)]);
    ui->graphicsView_3->scene()->addPixmap(mww->imgarray[mww->geticon(2)]);
    ui->graphicsView_4->scene()->addPixmap(mww->imgarray[mww->geticon(3)]);
    ui->graphicsView_5->scene()->addPixmap(mww->imgarray[mww->geticon(4)]);
    ui->graphicsView_6->scene()->addPixmap(mww->imgarray[mww->geticon(5)]);
    ui->graphicsView_7->scene()->addPixmap(mww->imgarray[mww->getmainpic()]);
}

void leftpanel::updateplaypic()
{
   // if (mww->plotw->start)
   //     mww->plotw->grabopencv(ocvpic);
}

bool leftpanel::eventFilter(QObject *target, QEvent *event)
{
    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updatemainpic(mww->geticon(0));
    if ((target == ui->graphicsView_2) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updatemainpic(mww->geticon(1));
    if ((target == ui->graphicsView_3) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updatemainpic(mww->geticon(2));
    if ((target == ui->graphicsView_4) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updatemainpic(mww->geticon(3));
    if ((target == ui->graphicsView_5) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updatemainpic(mww->geticon(4));
    if ((target == ui->graphicsView_6) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updatemainpic(mww->geticon(5));
}

void leftpanel::on_pushButton_clicked()
{
    mww->shuffleiconss();
    fillpics();
}

leftpanel::~leftpanel()
{
    delete ui;
}
