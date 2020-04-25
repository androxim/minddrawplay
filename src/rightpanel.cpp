/* source file for right panel class -
   resposible for overlay image choice for MindOCV */

#include "rightpanel.h"
#include "ui_rightpanel.h"

rightpanel::rightpanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rightpanel)
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
    ui->graphicsView_7->installEventFilter(this);

    ui->pushButton_3->setGeometry(4,1,44,23);
    ui->pushButton_2->setGeometry(51,1,44,23);
    ui->pushButton_4->setGeometry(97,1,48,23);

    ui->pushButton->setGeometry(5,1005,138,23);
}

rightpanel::~rightpanel()
{
    delete ui;
}

bool rightpanel::eventFilter(QObject *target, QEvent *event)
{
    if ((target == ui->graphicsView) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updateoverpic(mww->geticonnum(currpos,false));
    if ((target == ui->graphicsView_2) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updateoverpic(mww->geticonnum(currpos+1,false));
    if ((target == ui->graphicsView_3) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updateoverpic(mww->geticonnum(currpos+2,false));
    if ((target == ui->graphicsView_4) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updateoverpic(mww->geticonnum(currpos+3,false));
    if ((target == ui->graphicsView_5) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updateoverpic(mww->geticonnum(currpos+4,false));
    if ((target == ui->graphicsView_6) && (event->type() == QEvent::MouseButtonDblClick))
        mww->updateoverpic(mww->geticonnum(currpos+5,false));

    if ((target != ui->graphicsView_7) && (event->type() == QEvent::Wheel))
    {
        QWheelEvent* wEvent = static_cast<QWheelEvent*>(event);
        QPoint numDegrees = wEvent->angleDelta() / 8;
        if (!numDegrees.isNull())
        {
            QPoint numSteps = numDegrees / 15;
            if ((numSteps.ry()==-1) && (currpos<imgnumber-6))
            {
                currpos++;
                fillpics();
            }
            else if ((numSteps.ry()==1) && (currpos>0))
            {
                currpos--;
                fillpics();;
            }
        }
        wEvent->accept();
    }
    return false;
}

void rightpanel::fillpics()
{
    ui->graphicsView->scene()->clear();
    ui->graphicsView->scene()->addPixmap(mww->imgarray[mww->geticonnum(currpos,false)]);
    ui->graphicsView_2->scene()->clear();
    ui->graphicsView_2->scene()->addPixmap(mww->imgarray[mww->geticonnum(currpos+1,false)]);
    ui->graphicsView_4->scene()->clear();
    ui->graphicsView_3->scene()->addPixmap(mww->imgarray[mww->geticonnum(currpos+2,false)]);
    ui->graphicsView_4->scene()->clear();
    ui->graphicsView_4->scene()->addPixmap(mww->imgarray[mww->geticonnum(currpos+3,false)]);
    ui->graphicsView_5->scene()->clear();
    ui->graphicsView_5->scene()->addPixmap(mww->imgarray[mww->geticonnum(currpos+4,false)]);
    ui->graphicsView_6->scene()->clear();
    ui->graphicsView_6->scene()->addPixmap(mww->imgarray[mww->geticonnum(currpos+5,false)]);
    ui->graphicsView_7->scene()->clear();
    ui->graphicsView_7->scene()->addPixmap(mww->imgarray[mww->getoverpic()]);
}

void rightpanel::on_pushButton_clicked()
{
    mww->shuffleiconss(false);
    fillpics();
}

void rightpanel::on_pushButton_2_clicked()
{
    currpos=imgnumber-6;
    fillpics();
}

void rightpanel::on_pushButton_3_clicked()
{
    // if (currpos>0)
     {
        // currpos--;
         currpos = 0;
         fillpics();;
     }
}

void rightpanel::on_pushButton_4_clicked()
{
    QString filename=QFileDialog::getOpenFileName(this,tr("Open File"),mww->getfolderpath(),"Images (*.png *.bmp *.jpg)");
    if (filename!="")
    {
        int tp = mww->getimagenum(filename);
        if ((tp == mww->getmainpic()) || (tp == mww->getoverpic()))
        {
            QMessageBox msgBox;
            msgBox.setText("Please, choose image different from current main and overlay!");
            msgBox.exec();
        } else
        if (tp == -1)
        {
            QMessageBox msgBox;
            msgBox.setText("Please, choose image from current image folder!");
            msgBox.exec();
        } else
        if (tp > -1)
            mww->updateoverpic(tp);
    }
}
