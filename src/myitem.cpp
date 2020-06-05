/* source file for MyItem class -
   resposible for moving item game object parameters and functions */

#include "myitem.h"
#include "qdebug.h"

MyItem::MyItem()
{
    startX = 0;
    startY = 570;
    setPos(mapToParent(startX,startY));
    speed = 5;
    hitborder = false;
    cantgo = false;
    godown = false;
}

QRectF MyItem::boundingRect() const
{
    return QRectF(0,0,30,30);
}

void MyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect();
    QBrush brush(Qt::gray);

    if (scene()->collidingItems(this).isEmpty())
    {
        brush.setColor(QColor(qrand()%256,qrand()%256,qrand()%256));
        hitborder = false;
        if (pos().y()<570)
        {
            setPos(pos().x()+40,startY);
            pfw->playsometone();
        }        
    }
    else
    {
        brush.setColor(Qt::red);
        if (!hitborder)
            pfw->playsometone();
        hitborder = true;
    }    
    if (cantgo)
        brush.setColor(Qt::yellow);
    painter->fillRect(rect,brush);
    painter->drawRect(rect);
}

void MyItem::advance(int phase)
{
    if (!phase) return;
    if (cantgo) return;
    if (!hitborder)
        setPos(mapToParent(speed,0));
    else
    {
        if (!godown)
            setPos(mapToParent(0,-speed));
        else if (pos().y()<startY)
            setPos(mapToParent(0,speed));
        else
            cantgo=true;
    }
    if (pos().x()>1500)
    {
        pfw->newroundmovegame();
        setPos(startX,startY);
    }
}

void MyItem::docollision()
{

}
