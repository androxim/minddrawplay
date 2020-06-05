/* header file for MyItem class -
   resposible for moving item game object parameters and functions */

#ifndef MYITEM_H
#define MYITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "paintform.h"
#include "plotwindow.h"

class paintform;

class MyItem : public QGraphicsItem
{
public:
    MyItem();
    bool hitborder, cantgo, godown;
    int startX, startY;
    paintform* pfw;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void advance(int phase);

private:
    qreal speed;
    void docollision();
};

#endif // MYITEM_H
