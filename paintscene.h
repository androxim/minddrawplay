#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include <plotwindow.h>
#include <QSound>
#include "paintform.h"

const int TMAX=1024;

class plotwindow;
class MainWindow;
class paintform;

class paintScene : public QGraphicsScene
{

    Q_OBJECT

public:

    plotwindow* pw;  
    MainWindow* mww;
    paintform* paintf;

    QTimer* tim;

    qreal curx, cury;
    int rc, gc, bc, ac, tx, ty, xv, randr, randg, randb;
    int t0, t1, pos, counter, length, pointnum, currnumpoint;
    int* data;
    int* angles;
    int attentt, meditt, currlinenum, currlinedraw;
    int drawrate;
    QPointF* linecoords;
    QVector<QPointF*> linesarr;
    int* linelengths;
    QPixmap bkgndimg;
    double sumd, meand, angle;
    Complex t[TMAX];
    CArray cdata;
    QPalette qptr;
    QPolygonF polygon;
    QColor drcolor, fxcolor, randfxcl;
    bool drawflow;
    bool randcolor, fixcolor, freqcolor, randfixcolor, drawbpoints, startedline, horizline, vertline, attmodul, drawcontours, timeoff;
    QPointF previousPoint, p1, p2;
    QImage qbim1,qbim2;
    QPixmap* pmv;
    QByteArray arr;
    QBuffer buffer;
    QGraphicsColorizeEffect *colorize;
    QGraphicsBlurEffect *blur;
    QGraphicsScene sceneforfilt;
    QGraphicsPixmapItem itemforfilt;
    QImage resforfilt;
    QPainter* ptr;
    QPixmap pm;

    void init(plotwindow* pww, MainWindow* mw);
    void getdata(int x);
    void drawline();
    void setcolor();
    void clearlast();
    void applyfilter();
    QPixmap filterpuzzle(QPixmap pm, int att, int rc, int gc, int bc, int ac);
    void drawlinebyeeg();
    void k_curve(double x, double y,double lenght, double angle, int n_order);
    void line1(int x_1,int y_1, int x_2,int y_2);
    void drawpolygon(int p, int xc, int yc, double r, double d);
    void filllines();
    void applyfilteronbackimg();

    QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent);
    QImage blurred(const QImage& image, const QRect& rect, int radius, bool alphaOnly);
    bool eventFilter(QObject *target, QEvent *event);
    explicit paintScene(QObject *parent = 0);
    ~paintScene();

private:
    void drawBackground(QPainter *p, const QRectF &rect);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseScrEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void timerUpdate();

};

#endif // PAINTSCENE_H
