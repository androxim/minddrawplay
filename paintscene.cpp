#include "paintscene.h"
#include "random"
#include "QtAlgorithms"
#include <QSound>
#include "qmath.h"

// TO DO:
// add transparency control
// 1. attention game with overcoming set of lines
// 2. drawing continuos structure with complexity dependent on attention

paintScene::paintScene(QObject *parent) : QGraphicsScene(parent)
{
    qDeleteAll(this->items());
    pos=0;
    t0=0;
    randcolor=false;
    drawbpoints=false;
    startedline = false;
    drawcontours = false;
    randfixcolor=true;
    fxcolor="orange";
    linecoords = new QPointF[2000];
    linesarr.resize(1000);
    linelengths = new int[1000];
    colorize = new QGraphicsColorizeEffect;
    blur = new QGraphicsBlurEffect;
    for (int i=0; i<1000; i++)
    {
        linesarr[i] = new QPointF[1000];
        linelengths[i] = 0;
    }
    currlinenum = -1;
    currlinedraw = 0;
    timeoff = true;
    angles = new int[5000];
    pointnum = 0; currnumpoint = 0;
    curx = 600; cury=900;
    drawrate = 5;
    previousPoint.setX(curx);
    previousPoint.setY(cury-2);
    randfixcolor = false;
    sceneforfilt.addItem(&itemforfilt);
    resforfilt=QImage(QSize(1500, 800), QImage::Format_ARGB32);
    ptr = new QPainter(&resforfilt);
    drawflow=false;
    //resforfilt.load("D:/PICS/881.jpg");
    //pm = new QPixmap(NULL);
  //  colorize = new QGraphicsColorizeEffect;
  //  blur = new QGraphicsBlurEffect;
  //  qApp->installEventFilter(this);
}

bool paintScene::eventFilter(QObject *target, QEvent *event)
{

}

paintScene::~paintScene()
{

}

void paintScene::init(plotwindow* pww, MainWindow* mw)
{
    pw=pww;
    mww=mw;
    mww->psstart=true;
  //  pw->pssstart=true;
    startedline=false;
    attmodul=false;
    horizline=false; vertline=false;
    counter=1; length=512; sumd=0; t0=0;
    data = new int[length];
    tim = new QTimer(this);
    tim->connect(tim, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    rc=0; gc=0; bc=0; ac=255;
    tx=0; ty=-1; xv=1;
    tim->setInterval(drawrate);
  //  tim->start();
}

QImage paintScene::blurred(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    int i1 = 0;
    int i2 = 3;

    if (alphaOnly)
        i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    return result;
}


QImage paintScene::applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent)
{
    if(src.isNull()) return QImage();   //No need to do anything else!
    if(!effect) return src;             //No need to do anything else!
    if (pmv)
        delete pmv;
    pmv = new QPixmap(QPixmap::fromImage(src));
    itemforfilt.setPixmap(*pmv);
    itemforfilt.setGraphicsEffect(effect);
    resforfilt.fill(Qt::transparent);
    sceneforfilt.render(ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ),paintf->rationmode);
    return resforfilt;
}

QPixmap paintScene::filterpuzzle(QPixmap pm, int att, int rc, int gc, int bc, int ac)
{
  //  blur = new QGraphicsBlurEffect;
  //  blur->setBlurRadius((100-att)/8);
    colorize = new QGraphicsColorizeEffect;
    QColor qcl = QColor(randr,randg,randb);
    colorize->setColor(qcl);
    colorize->setStrength((double)(100-att)/100);
    qbim1 = applyEffectToImage(pm.toImage(), colorize, 0);
   // qbim2 = applyEffectToImage(qbim1, blur, 0);
    return QPixmap::fromImage(qbim1);
}

void paintScene::applyfilteronbackimg()
{
    colorize = new QGraphicsColorizeEffect;
    blur = new QGraphicsBlurEffect;
    blur->setBlurRadius((100-pw->attent)/15);
   // colorize->setColor(QColor(pw->alpha*5,256-pw->beta*5,256-pw->gamma*6,pw->meditt*2));
    QColor qcl = QColor(pw->theta*4,pw->beta*4,pw->gamma*4,pw->alpha*6);
   // qcl.setHsv(pw->beta*7,pw->alpha*7,pw->theta*6);
    colorize->setColor(qcl);//QColor(pw->theta*4,pw->beta*4,pw->gamma*4,pw->alpha*4));
    colorize->setStrength((double)pw->attent/50);

    //qbim1 = applyEffectToImage(paintf->qim, blur, 0);
    qbim1 = applyEffectToImage(bkgndimg.toImage(), blur, 0);
    qbim2 = applyEffectToImage(qbim1, colorize, 0);
  // qbim2 = blurred(paintf->qim,QRect(previousPoint.x(),previousPoint.y(),previousPoint.x()+200,previousPoint.y()+200),10,false);

    qptr.setBrush(QPalette::Background, QPixmap::fromImage(qbim2).scaled(paintf->size(), Qt::IgnoreAspectRatio));
    paintf->setPalette(qptr);
    paintf->repaint();

}

void paintScene::applyfilter()
{    
    if ((paintf->qimload) && (paintf->bfiltmode))
    {
        if (!paintf->qim.isNull())
        {      
        colorize = new QGraphicsColorizeEffect;
        blur = new QGraphicsBlurEffect;
        blur->setBlurRadius((100-pw->attent)/15);
       // colorize->setColor(QColor(pw->alpha*5,256-pw->beta*5,256-pw->gamma*6,pw->meditt*2));
        QColor qcl = QColor(pw->theta*4,pw->beta*4,pw->gamma*4,pw->alpha*6);
       // qcl.setHsv(pw->beta*7,pw->alpha*7,pw->theta*6);
        colorize->setColor(qcl);//QColor(pw->theta*4,pw->beta*4,pw->gamma*4,pw->alpha*4));
        colorize->setStrength((double)pw->attent/50);

        //qbim1 = applyEffectToImage(paintf->qim, blur, 0);
        qbim1 = applyEffectToImage(paintf->qim, blur, 0);
        qbim2 = applyEffectToImage(qbim1, colorize, 0);

      // qbim2 = blurred(paintf->qim,QRect(previousPoint.x(),previousPoint.y(),previousPoint.x()+200,previousPoint.y()+200),10,false);

        paintf->pmain = QPixmap::fromImage(qbim2);
    //    if (!paintf->curpic.isNull())
  //           paintf->qim=paintf->pmain.toImage();
        this->clear();
        if (paintf->puzzlemode)
            addPixmap(paintf->pmain.scaled(300,200,paintf->rationmode,Qt::SmoothTransformation));
        else
            addPixmap(paintf->pmain.scaled(1500,800,paintf->rationmode,Qt::SmoothTransformation));
        // addPixmap(paintf->pm.scaledToHeight(this->height()));
        }
    }
}

void paintScene::drawlinebyeeg()
{
    for (int i=1; i<pointnum; i++)
    {
        setcolor();
        angle = qAtan2(linecoords[i].y()-linecoords[i-1].y(),linecoords[i].x()-linecoords[i-1].x());
        addLine(linecoords[i-1].x(), linecoords[i-1].y(), linecoords[i].x()+qCos(angle+M_PI/2)*t0, linecoords[i].y()+qSin(angle+M_PI/2)*t0, QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
    }
}

void paintScene::getdata(int x)
{
    // two fast blinks - change of mode ?
    if (counter==length)
    {
        counter=1;
        sumd=0;
    }
    else
    {
       // data[counter]=x;
        sumd+=x; meand=sumd/counter;
        counter++;
    }
    //if (x>meand*2)
     //   x=meand;
    if (attmodul)
    {
        if (attentt<10)
            attentt=10;
        t0=((x-meand)*attentt)/40;
    }
    else
        t0=(x-meand)*paintf->eegsize/2;
    //QCursor::setPos(400,t0);
   // qDebug()<<x<<" "<<meand;
}

void paintScene::drawBackground(QPainter *p, const QRectF &rect)
{

}

void paintScene::filllines()
{
   int t;
   int k;
   int p;
   int r;
   int f;
   for (int i=0; i<400; i++)
   {
       t=qrand()%500;
       k=qrand()%1300;
       p=50+qrand()%300;
       r=qrand()%2;
       linelengths[i]=p;
       for (int j=0; j<p; j++)
       {
           if (r==0)
               f=1;
           else
               f=-1;
           linesarr[i][j].setX(k+f*j);
           linesarr[i][j].setY(t);
       }
       currlinenum++;
   }
   randfxcl=QColor(qrand()%256,qrand()%256,qrand()%256,255-attentt*2);   
   timeoff=false;
}

void paintScene::timerUpdate()
{
    setcolor();
    if (linelengths[currlinedraw]==1)
    {
        linesarr[currlinedraw][1].setX(linesarr[currlinedraw][0].x());
        linesarr[currlinedraw][1].setY(linesarr[currlinedraw][0].y());
        linelengths[currlinedraw]=2;
    }
    angle = qAtan2(linesarr[currlinedraw][currnumpoint+1].y()-linesarr[currlinedraw][currnumpoint].y(),linesarr[currlinedraw][currnumpoint+1].x()-linesarr[currlinedraw][currnumpoint].x());
    addLine(linesarr[currlinedraw][currnumpoint].x(), linesarr[currlinedraw][currnumpoint].y(), linesarr[currlinedraw][currnumpoint+1].x()+qCos(angle+M_PI/2)*t0, linesarr[currlinedraw][currnumpoint+1].y()+qSin(angle+M_PI/2)*t0, QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
    currnumpoint++;    
    if (currnumpoint==linelengths[currlinedraw]-1)
    {
       // qDebug()<<currlinedraw;
        randfxcl=QColor(qrand()%256,qrand()%256,qrand()%256,255-attentt*2);
        currnumpoint=0;
      //  pointnum=0;
      //   qDebug()<<currlinedraw<<currlinenum;
        if (currlinedraw==currlinenum)
        {
            tim->stop();
            timeoff=true;
        }
        else
            currlinedraw++;
     //   qDebug()<<currlinedraw;
     //   qDebug()<<currlinenum;
    }

  /*  setcolor();

    angle = qAtan2(cury-previousPoint.y(),curx-previousPoint.x());
    addLine(previousPoint.x(), previousPoint.y(), curx+qCos(angle+M_PI/2)*t0, cury+qSin(angle+M_PI/2)*t0, QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
    previousPoint.setX(curx);
    previousPoint.setY(cury);

    if (cury<1)
        ty=1;
    if (cury>800)
        ty=-1;
    if (attentt>50)
    {   if (curx<1500)
            tx*=-1;
        else
            tx=-1; }
    else
    {
        if (curx>1)
            tx=-1;
        else
            tx=+1; }
    curx+=tx;
    cury+=ty; */
}

void paintScene::setcolor()
{
    if ((pw->start) && (!randfixcolor))
    {
       /* if ((pw->attent>77) && (pw->mindwstart))
        {
            fixcolor=true;
            freqcolor=false;
        } else if ((pw->attent<77) && (pw->mindwstart))
        {
            fixcolor=false;
            freqcolor=true;
        } */
        if (paintf->getattentmode())
        {
            rc=pw->beta*6;
            if (rc>255) rc=255;
            gc=pw->alpha;//130-attentt;
        }
        else
        {
            rc=pw->beta;//255-2*pw->meditt;
           // if (rc<0) rc=0;
            gc=pw->alpha*6;
        }
      //  if (gc>255)
      //      gc=255;
        bc=(pw->beta+pw->alpha)*2;
           if (bc>255) bc=255;
        if (paintf->attentmodu)
            ac=255-attentt*2;
        else
            ac=255-meditt*2;
           if (ac<0) ac=0;
     //   if (bc>255)
      //      bc=255;
       // rc = 50 + pw->theta*5;
       // gc = 250 - pw->alpha*2;
       // bc = 150 - pw->beta*2;
      /*  if ((pw->theta > pw->alpha) && (pw->theta>pw->beta))
        {
            rc=0;
            gc=0;
            bc=255;
        } else
        if ((pw->alpha > pw->theta) && (pw->alpha>pw->beta))
        {
            rc=0;
            gc=255;
            bc=0;
        } else
        if ((pw->beta > pw->theta) && (pw->beta>pw->alpha))
        {
            rc=255;
            gc=0;
            bc=0;
        }*/
       // if (pw->mindwstart)
       //     ac=pw->attent*1.5; //198-pw->delta*3;
        // qDebug()<<rc<<" "<<gc<<" "<<bc<<" "<<ac;
    }
    if (rc>255) rc=255;
    if (gc>255) gc=255;
    if (gc<0) gc=29;
    if (bc>255) bc=255;
    if (bc<0) bc=30;
    if (ac<0) ac=25;
    if (freqcolor)
        drcolor=QColor(rc,gc,bc,255-attentt);
    if (fixcolor)
        drcolor=QColor(fxcolor.red(),fxcolor.green(),fxcolor.blue(),255-attentt);
    if (randcolor)
        drcolor=QColor(qrand()%256,qrand()%256,qrand()%256,255-attentt);
    if (randfixcolor)
        drcolor=QColor(randfxcl.red(),randfxcl.green(),randfxcl.blue(),255-attentt);
    if (paintf->erasepen)
        drcolor=QColor(255,255,255,255);
}

void paintScene::clearlast()
{
//    double angle;
  //  qDebug()<<pointnum;
    if ((pointnum>0))
    {
      //  angle = qAtan2(linecoords[pointnum].y()-linecoords[0].y(),linecoords[pointnum].x()-linecoords[0].x());
        for (int i=0; i<pointnum-1; i++)
        {
            addLine(linecoords[i].x(), linecoords[i].y(), linecoords[i+1].x()+qSin(angles[i])*data[i], linecoords[i+1].y()+qCos(angles[i])*data[i], QPen(QColor(255,255,255,255), paintf->pensize, Qt::SolidLine, Qt::RoundCap));
            addEllipse(linecoords[i].x(), linecoords[i].y(), paintf->pensize, paintf->pensize, QPen(Qt::NoPen), QBrush(QColor(255,255,255,255)));
        }
        pointnum=0;
    }
}

void paintScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
 //   if ((!drawbpoints) && (startedline))
 //       startedline=false;
    if (startedline)
    {
        startedline=false;
        currlinenum++;
        linelengths[currlinenum]=pointnum;
        pointnum=0;
        if (timeoff)
        {
          //  qDebug()<<currlinedraw<<currlinenum;
            if (currlinenum>0)
                currlinedraw++;
            tim->start();
            timeoff=false;
        }
    }
    paintf->getimg2();
}

void paintScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{ 
    if (!horizline)
        cury=event->scenePos().y();
    if (!vertline)
        curx=event->scenePos().x();
    paintf->getimg1();
    if (drawcontours)
        t0=0;

    if ((randfixcolor)) // && (!drawflow)
        randfxcl=QColor(qrand()%256,qrand()%256,qrand()%256,255-attentt*2);

    setcolor();
   // addEllipse(event->scenePos().x(), event->scenePos().y(), paintf->pensize, paintf->pensize, QPen(Qt::NoPen), QBrush(drcolor));

    previousPoint = event->scenePos();

}

void paintScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((drawcontours) && (!startedline) && (!paintf->erasepen))
        startedline=true;

    if (!horizline)
        cury=event->scenePos().y();
    if (!vertline)
        curx=event->scenePos().x();
    double angle = qAtan2(event->scenePos().y()-previousPoint.y(),event->scenePos().x()-previousPoint.x());
   // qDebug()<<qRadiansToDegrees(angle);
    if (drawcontours)
        t0=0;

    setcolor();
    if (paintf->erasepen)
        addLine(previousPoint.x(), previousPoint.y(), event->scenePos().x(), event->scenePos().y(), QPen(drcolor, paintf->pensize*2, Qt::SolidLine, Qt::RoundCap));
    else if ((!horizline) && (!vertline))
        addLine(previousPoint.x(), previousPoint.y(), event->scenePos().x()+qCos(angle+M_PI/2)*t0, event->scenePos().y()+qSin(angle+M_PI/2)*t0, QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
    else if (horizline)
        addLine(previousPoint.x(), cury, event->scenePos().x()+qCos(angle+M_PI/2)*t0, cury+qSin(angle+M_PI/2)*t0, QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
    else if (vertline)
        addLine(curx, previousPoint.y(), curx+qCos(angle+M_PI/2)*t0, event->scenePos().y()+qSin(angle+M_PI/2)*t0, QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));

    if (startedline)
    {
       // qDebug()<<"2";
        linesarr[currlinenum+1][pointnum].setX(curx);
        linesarr[currlinenum+1][pointnum].setY(cury);
        pointnum++;
    }
    previousPoint = event->scenePos();
}

void paintScene::k_curve(double x, double y,double lenght, double angle, int n_order)
{
    if (n_order>0)
    {
        lenght/=3;
        k_curve(x,y,lenght,angle,(n_order-1));
        x+=(lenght*cosl(angle*(M_PI/180)));
        y+=(lenght*sinl(angle*(M_PI/180)));
        k_curve(x,y,lenght,(angle-60),(n_order-1));
        x+=(lenght*cosl((angle-60)*(M_PI/180)));
        y+=(lenght*sinl((angle-60)*(M_PI/180)));
        k_curve(x,y,lenght,(angle+60),(n_order-1));
        x+=(lenght*cosl((angle+60)*(M_PI/180)));
        y+=(lenght*sinl((angle+60)*(M_PI/180)));
        k_curve(x,y,lenght,angle,(n_order-1));
    }
    else
        line1(x,y,(int)(x+lenght*cosl(angle*(M_PI/180))+0.5),(int)(y+lenght*sinl(angle*(M_PI/180))));
}

void paintScene::line1(int x_1,int y_1, int x_2,int y_2)
{
    int x1=x_1;
    int y1=y_1;
    int x2=x_2;
    int y2=y_2;
    if(x_1>x_2)
    {
        x1=x_2;
        y1=y_2;
        x2=x_1;
        y2=y_1;
    }
    int dx=abs(x2-x1);
    int dy=abs(y2-y1);
    int inc_dec=((y2>=y1)?1:-1);
    if(dx>dy)
    {
        int two_dy=(2*dy);
        int two_dy_dx=(2*(dy-dx));
        int p=((2*dy)-dx);
        int x=x1;
        int y=y1;
        addLine(x,y,x+1,y+1,QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
        while(x<x2)
        {
            x++;
            if(p<0)
                p+=two_dy;
            else
            {
                y+=inc_dec;
                p+=two_dy_dx;
            }
            addLine(x,y,x+1,y+1,QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
        }
    }
    else
    {
        int two_dx=(2*dx);
        int two_dx_dy=(2*(dx-dy));
        int p=((2*dx)-dy);
        int x=x1;
        int y=y1;
        addLine(x,y,x+1,y+1,QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
        while(y!=y2)
        { y+=inc_dec;
            if(p<0)
                p+=two_dx;
            else
            {x++;
                p+=two_dx_dy;
            }
            addLine(x,y,x+1,y+1,QPen(drcolor, paintf->pensize, Qt::SolidLine, Qt::RoundCap));
        }
    }
}

void paintScene::drawpolygon(int p, int xc, int yc, double r, double d)
{
    polygon.clear();
    double x,y,t;
    for (int i=0; i<p; i++)
    {
        t = 2*M_PI*((double)i/p+d);
        x = xc + cos(t)*r;
        y = yc + sin(t)*r;
        polygon<<QPointF(x,y);
    }
    addPolygon(polygon,QPen(QColor(qrand()%256,qrand()%256,qrand()%256), 2, Qt::SolidLine, Qt::RoundCap));
}

void paintScene::drawline()
{
    int x0, x1, y0, y1;
    x0=p1.x(); y0=p1.y(); x1=previousPoint.x(); y1=previousPoint.y();
    double angle = qAtan2(y1-y0,x1-x0);
    //  qDebug()<<x0<<" "<<y0;
    //  qDebug()<<x1<<" "<<y1;
    //  qDebug()<<angle;
    int t1 = x1-x0; int t2=abs(y1-y0);
    int p = t1 / t2;
    int k=0;
    while (k<t1)
    {
        if (k==p)
        {
            y0++;
            p=p+p;
        }
        setcolor();
        addLine(x0, y0, x0+1+qSin(angle)*t0, y0+qCos(angle)*t0, QPen(QColor(rc,gc,bc,ac), paintf->pensize, Qt::SolidLine, Qt::RoundCap));
        k++;
        paintf->delay(1);
    }
    /* for (int i=x0; i<x1; i++)
    {
        setcolor();
        addLine(i, y0, i+1+qSin(angle)*t0, y1+qCos(angle)*t0, QPen(QColor(rc,gc,bc,ac), paintf->pensize, Qt::SolidLine, Qt::RoundCap));
        paintf->delay(1);
    }*/
}
