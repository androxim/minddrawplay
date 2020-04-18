/* header file for paintScene class -
   resposible for drawing with brain waves */

#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include <plotwindow.h>
#include <QSound>
#include <valarray>
#include "paintform.h"
#include <complex>

const int TMAX=1024;

class plotwindow;
class MainWindow;
class paintform;

using namespace std;

typedef complex<double> ComplexM;
typedef valarray<ComplexM> CArray;

class paintScene : public QGraphicsScene
{

    Q_OBJECT

public:
    // description of most variables in paintscene.h //

    paintform* paintf;      // pointer on MindDraw window class
    plotwindow* pw;         // pointer on MindPlay window class
    MainWindow* mww;        // pointer on MainWindow class

    int rc, gc, bc, ac, tx, ty, xv, randr, randg, randb;
    int t0, t1, counter, length, pointnum, currnumpoint;
    int attentt, meditt, currlinenum, currlinedraw, drawrate;
    int* linelengths;

    bool drawflow, randcolor, fixcolor, freqcolor, randfixcolor, drawbpoints;
    bool startedline, horizline, vertline, attmodul, drawcontours, timeoff;
    double sumd, meand, angle;

    QTimer* tim;

    QColor drcolor, fxcolor, randfxcl;
    QVector<QPointF*> linesarr;
    QPointF previousPoint, p1, p2;
    QPointF* linecoords;
    qreal curx, cury;

    QImage qbim1, qbim2;
    QPixmap bkgndimg, pm;
    QPalette qptr;
    QPolygonF polygon;

    QPixmap* pmv;
    QByteArray arr;
    QBuffer buffer;

    QGraphicsColorizeEffect *colorize;
    QGraphicsBlurEffect *blur;
    QGraphicsScene sceneforfilt;
    QGraphicsPixmapItem itemforfilt;
    QImage resforfilt;
    QPainter* ptr;

    void init(plotwindow* pww, MainWindow* mw);
    void getdata(int x);
    void drawline();
    void setcolor();
    void applyfilter();
    QPixmap filterpuzzle(QPixmap pm, int att);
    void drawlinebyeeg();
    void k_curve(double x, double y,double lenght, double angle, int n_order);
    void line1(int x_1,int y_1, int x_2,int y_2);
    void drawpolygon(int p, int xc, int yc, double r, double d);
    void fill_lines();
    void applyfilteronbackimg();

    QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent);     
    explicit paintScene(QObject *parent = 0); 

private:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseScrEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void timerUpdate();

};

#endif // PAINTSCENE_H
